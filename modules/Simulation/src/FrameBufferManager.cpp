#include <simulation/implementations/FrameBufferManager.hpp>
#include <math/PointCloud.hpp>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>

namespace simulation
{
    namespace fs = std::filesystem;

    FrameBufferManager::FrameBufferManager(int windowSize)
        : frameDir_(core::ResourceLocator::getJsonPathForScene("")),
          windowSize_(windowSize),
          currentFrameIndex_(0),
          totalFrameCount_(0),
          isPlaying_(false),
          frameInterval_(0.10f), // ~10Hz
          playbackTimer_(0.0f)
    {
        adapters_ = std::make_unique<adapter::AdapterManager>();

        // Count files in folder that match frame_XXXXX.json
        for (const auto &entry : fs::directory_iterator(frameDir_))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
                ++totalFrameCount_;
        }

        loadWindowAround(currentFrameIndex_);

        // Start preloading the next frame
        startPreloadingNextFrame();
    }

    void FrameBufferManager::update(float deltaTime)
    {
        if (!isPlaying_ || totalFrameCount_ == 0)
            return;

        playbackTimer_ += deltaTime;
        if (playbackTimer_ >= frameInterval_)
        {
            playbackTimer_ = 0.0f;
            if (canAdvance(+1))
            {
                stepForward();
            }
            else
            {
                isPlaying_ = false;
            }
        }
    }

    void FrameBufferManager::play() { isPlaying_ = true; }
    void FrameBufferManager::pause() { isPlaying_ = false; }
    void FrameBufferManager::togglePlayPause() { isPlaying_ = !isPlaying_; }

    void FrameBufferManager::seek(int frameId)
    {
        if (frameId < 0 || frameId >= totalFrameCount_)
            return;

        currentFrameIndex_ = frameId;
        loadWindowAround(currentFrameIndex_);
        fireCallback();
    }

    bool FrameBufferManager::canAdvance(int direction) const
    {
        int targetIndex = currentFrameIndex_ + direction;
        return (targetIndex >= 0 && targetIndex < totalFrameCount_);
    }

    void FrameBufferManager::notifyObservers()
    {
        if (frameWindow_.empty())
        {
            LOGGER_WARN("Frame window is empty, cannot notify observers.");
            return;
        }

        const auto &frame = frameWindow_[windowSize_];

        if (onFrameChanged_)
            onFrameChanged_(currentFrameIndex_, frame->cloud, frame->timestamp);

        for (auto it = frameObservers_.begin(); it != frameObservers_.end();)
        {
            if (auto observer = it->lock())
            {
                // LOGGER_WARN("Notifying frame [" + observer->getObserverName() + "] of frame change");
                observer->onFrameChanged(frame);
                ++it;
            }
            else
            {
                it = frameObservers_.erase(it);
            }
        }
    }

    void FrameBufferManager::shiftWindow(int direction)
    {
        if (direction == +1)
        {
            if (!frameWindow_.empty() && frameWindow_.front())
                frameWindow_.front()->clear();
            frameWindow_.pop_front();

            int newFrameIndex = currentFrameIndex_ + windowSize_;
            if (newFrameIndex < totalFrameCount_)
            {
                // Try to use preloaded frame if available
                if (hasPreloadedFrame_.load() && preloadedFrameIndex_ == newFrameIndex)
                {
                    std::lock_guard<std::mutex> lock(preloadMutex_);
                    frameWindow_.push_back(preloadedFrame_);
                    preloadedFrame_.reset();
                    hasPreloadedFrame_.store(false);
                }
                else
                {
                    frameWindow_.push_back(loadFrame(newFrameIndex));
                }
            }
            else
            {
                frameWindow_.push_back(std::make_shared<Frame>());
            }

            // Start preloading the next frame in background
            startPreloadingNextFrame();
        }
        else if (direction == -1)
        {
            if (!frameWindow_.empty() && frameWindow_.back())
                frameWindow_.back()->clear();
            frameWindow_.pop_back();

            int newFrameIndex = currentFrameIndex_ - windowSize_;
            if (newFrameIndex >= 0)
                frameWindow_.push_front(loadFrame(newFrameIndex));
            else
                frameWindow_.push_front(std::make_shared<Frame>());
        }
    }

    void FrameBufferManager::advanceFrame(int direction)
    {
        currentFrameIndex_ += direction;
    }

    void FrameBufferManager::stepForward()
    {
        if (canAdvance(+1))
        {
            advanceFrame(+1);
            shiftWindow(+1);
            notifyObservers();
        }
    }

    void FrameBufferManager::stepBackward()
    {
        if (canAdvance(-1))
        {
            advanceFrame(-1);
            shiftWindow(-1);
            notifyObservers();
        }
    }

    std::shared_ptr<math::PointCloud> FrameBufferManager::getCurrentCloud() const
    {
        if (!frameWindow_.empty())
            return frameWindow_[windowSize_]->cloud;
        return nullptr;
    }

    double FrameBufferManager::getCurrentTimestamp() const
    {
        if (!frameWindow_.empty())
            return frameWindow_[windowSize_]->timestamp;
        return 0.0;
    }

    void FrameBufferManager::loadWindowAround(int centerFrame)
    {
        // Step 1: Clear internal data of each frame before dropping references
        for (auto &frame : frameWindow_)
        {
            if (frame)
                frame->clear();
        }

        frameWindow_.clear();
        for (int offset = -windowSize_; offset <= windowSize_; ++offset)
        {
            int index = centerFrame + offset;
            if (index < 0 || index >= totalFrameCount_)
            {
                frameWindow_.emplace_back(std::make_shared<Frame>());
            }
            else
            {
                frameWindow_.emplace_back(loadFrame(index));
            }
        }
    }

    std::shared_ptr<Frame> FrameBufferManager::loadFrame(int index)
    {
        std::ostringstream filename;
        filename << "frame_" << std::setw(5) << std::setfill('0') << index << ".json";

        std::string path = core::ResourceLocator::getJsonPathForScene(filename.str());
        auto frame = adapters_->fromJson<std::shared_ptr<simulation::Frame>>(path);

        frame->filePath = path; // filePath JSON'da yoksa burada setlenir
        return frame;
    }

    void FrameBufferManager::fireCallback()
    {
        if (!onFrameChanged_ && frameObservers_.empty())
            return;

        if (frameWindow_.size() <= static_cast<size_t>(windowSize_))
            return;

        auto &frame = frameWindow_[windowSize_];
        if (!frame)
            return;

        if (onFrameChanged_)
        {
            onFrameChanged_(currentFrameIndex_, frame->cloud, frame->timestamp);
        }

        for (auto it = frameObservers_.begin(); it != frameObservers_.end();)
        {
            if (auto observer = it->lock())
            {
                observer->onFrameChanged(frame);
                ++it;
            }
            else
            {
                it = frameObservers_.erase(it);
            }
        }
    }

    std::shared_ptr<Frame> FrameBufferManager::getCurrentFrame() const
    {
        if (frameWindow_.size() > static_cast<size_t>(windowSize_))
            return frameWindow_[windowSize_];
        return nullptr;
    }

    void FrameBufferManager::addFrameObserver(const std::shared_ptr<IFrameObserver> &observer)
    {
        frameObservers_.push_back(observer);
    }

    void FrameBufferManager::startPreloadingNextFrame()
    {
        // Don't start if already preloading
        if (preloadInProgress_.load())
            return;

        // Calculate which frame to preload (next one after current window)
        int nextFrameIndex = currentFrameIndex_ + windowSize_ + 1;

        if (nextFrameIndex >= totalFrameCount_)
            return; // No more frames to preload

        preloadInProgress_.store(true);

        std::thread([this, nextFrameIndex]()
                    {
            try
            {
                std::ostringstream filename;
                filename << "frame_" << std::setw(5) << std::setfill('0') << nextFrameIndex << ".json";
                std::string path = core::ResourceLocator::getJsonPathForScene(filename.str());
                
                // Create a new adapter for thread safety
                adapter::AdapterManager threadAdapters;
                auto frame = threadAdapters.fromJson<std::shared_ptr<simulation::Frame>>(path);
                frame->filePath = path;

                {
                    std::lock_guard<std::mutex> lock(preloadMutex_);
                    preloadedFrame_ = frame;
                    preloadedFrameIndex_ = nextFrameIndex;
                    hasPreloadedFrame_.store(true);
                }
            }
            catch (const std::exception &e)
            {
                LOGGER_WARN("Failed to preload frame: " + std::string(e.what()));
            }

            preloadInProgress_.store(false); })
            .detach();
    }

}
