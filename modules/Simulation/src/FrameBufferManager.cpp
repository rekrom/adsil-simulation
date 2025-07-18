#include <simulation/implementations/FrameBufferManager.hpp>
#include <core/PointCloud.hpp>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>

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
    }

    void FrameBufferManager::update(float deltaTime)
    {
        if (!isPlaying_ || totalFrameCount_ == 0)
            return;

        playbackTimer_ += deltaTime;
        if (playbackTimer_ >= frameInterval_)
        {
            playbackTimer_ = 0.0f;
            stepForward();
        }
    }

    void FrameBufferManager::play() { isPlaying_ = true; }
    void FrameBufferManager::pause() { isPlaying_ = false; }

    void FrameBufferManager::seek(int frameIndex)
    {
        if (frameIndex < 0 || frameIndex >= totalFrameCount_)
            return;

        currentFrameIndex_ = frameIndex;
        loadWindowAround(currentFrameIndex_);
        fireCallback();
    }

    void FrameBufferManager::stepForward()
    {
        if (currentFrameIndex_ + 1 >= totalFrameCount_)
            return;

        ++currentFrameIndex_;

        // Remove the oldest frame at the front (left side)
        if (!frameWindow_.empty())
            frameWindow_.pop_front();

        // Load new frame at the back (right side)
        int newFrameIndex = currentFrameIndex_ + windowSize_;
        if (newFrameIndex < totalFrameCount_)
            frameWindow_.push_back(loadFrame(newFrameIndex));
        else
            frameWindow_.push_back(std::make_shared<Frame>()); // placeholder if out of range

        // Fire callback with center frame
        const auto &center = frameWindow_[windowSize_];
        if (onFrameChanged_ && center && center->cloud)
            onFrameChanged_(currentFrameIndex_, center->cloud, center->timestamp);
    }

    void FrameBufferManager::stepBackward()
    {
        if (currentFrameIndex_ - 1 < 0)
            return;

        --currentFrameIndex_;

        // Remove the newest frame at the back (right side)
        if (!frameWindow_.empty())
            frameWindow_.pop_back();

        // Load new frame at the front (left side)
        int newFrameIndex = currentFrameIndex_ - windowSize_;
        if (newFrameIndex >= 0)
            frameWindow_.push_front(loadFrame(newFrameIndex));
        else
            frameWindow_.push_front(std::make_shared<Frame>()); // placeholder if out of range

        // Fire callback with center frame
        const auto &center = frameWindow_[windowSize_];
        if (onFrameChanged_ && center && center->cloud)
            onFrameChanged_(currentFrameIndex_, center->cloud, center->timestamp);
    }

    std::shared_ptr<PointCloud> FrameBufferManager::getCurrentCloud() const
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

    void FrameBufferManager::setOnFrameChanged(std::function<void(int, std::shared_ptr<PointCloud>, double)> cb)
    {
        onFrameChanged_ = std::move(cb);
        // 🔥 Trigger immediately for current frame, if valid
        if (!frameWindow_.empty() && frameWindow_[windowSize_]->cloud)
        {
            onFrameChanged_(currentFrameIndex_,
                            frameWindow_[windowSize_]->cloud,
                            frameWindow_[windowSize_]->timestamp);
        }
    }

    void FrameBufferManager::loadWindowAround(int centerFrame)
    {
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
        if (onFrameChanged_)
        {
            auto &frame = frameWindow_[windowSize_];
            onFrameChanged_(currentFrameIndex_, frame->cloud, frame->timestamp);
        }
    }
}
