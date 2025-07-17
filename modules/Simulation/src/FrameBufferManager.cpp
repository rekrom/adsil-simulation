#include <simulation/implementations/FrameBufferManager.hpp>
#include <adapter/AdapterManager.hpp> // Assuming this loads PointCloud
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
        // Count files in folder that match frame_XXXXX.json
        for (const auto &entry : fs::directory_iterator(frameDir_))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
                ++totalFrameCount_;
        }

        loadWindowAround(currentFrameIndex_);
        fireCallback();
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
        if (currentFrameIndex_ + 1 < totalFrameCount_)
        {
            ++currentFrameIndex_;
            loadWindowAround(currentFrameIndex_);
            fireCallback();
        }
    }

    void FrameBufferManager::stepBackward()
    {
        if (currentFrameIndex_ - 1 >= 0)
        {
            --currentFrameIndex_;
            loadWindowAround(currentFrameIndex_);
            fireCallback();
        }
    }

    std::shared_ptr<PointCloud> FrameBufferManager::getCurrentCloud() const
    {
        if (!frameWindow_.empty())
            return frameWindow_[windowSize_].cloud;
        return nullptr;
    }

    double FrameBufferManager::getCurrentTimestamp() const
    {
        if (!frameWindow_.empty())
            return frameWindow_[windowSize_].timestamp;
        return 0.0;
    }

    void FrameBufferManager::setOnFrameChanged(std::function<void(int, std::shared_ptr<PointCloud>, double)> cb)
    {
        onFrameChanged_ = std::move(cb);
    }

    void FrameBufferManager::loadWindowAround(int centerFrame)
    {
        frameWindow_.clear();
        for (int offset = -windowSize_; offset <= windowSize_; ++offset)
        {
            int index = centerFrame + offset;
            if (index < 0 || index >= totalFrameCount_)
            {
                frameWindow_.emplace_back(simulation::Frame{
                    .cloud = nullptr,
                    .timestamp = 0.0,
                    .linearAcceleration = {},
                    .angularVelocity = {},
                    .filePath = ""});
            }
            else
            {
                frameWindow_.emplace_back(loadFrame(index));
            }
        }
    }

    Frame FrameBufferManager::loadFrame(int index)
    {
        std::ostringstream filename;
        filename << "frame_" << std::setw(5) << std::setfill('0') << index << ".json";
        std::string path = core::ResourceLocator::getJsonPathForScene(filename.str());

        adapter::AdapterManager adapters;
        auto frame = adapters.fromJson<simulation::Frame>(path);
        frame.filePath = path; // filePath JSON'da yoksa burada setlenir
        return frame;
    }

    void FrameBufferManager::fireCallback()
    {
        if (onFrameChanged_)
        {
            auto &frame = frameWindow_[windowSize_];
            onFrameChanged_(currentFrameIndex_, frame.cloud, frame.timestamp);
        }
    }
}
