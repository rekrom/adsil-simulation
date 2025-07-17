#pragma once

#include <core/PointCloud.hpp>
#include <core/ResourceLocator.hpp>
#include <functional>
#include <deque>
#include <string>
#include <memory>
#include <simulation/implementations/Frame.hpp>

namespace simulation
{
    class FrameBufferManager
    {
    public:
        FrameBufferManager(int windowSize = 3);

        void update(float deltaTime); // Called every simulation tick
        void play();
        void pause();
        void seek(int frameId);
        void stepForward();
        void stepBackward();

        bool isPlaying() const;

        std::shared_ptr<PointCloud> getCurrentCloud() const;
        double getCurrentTimestamp() const;

        void setOnFrameChanged(std::function<void(int, std::shared_ptr<PointCloud>, double)> cb);

    private:
        Frame frame_;

        std::deque<Frame> frameWindow_;
        std::string frameDir_;
        int currentFrameIndex_ = 0;
        int totalFrameCount_ = 0;

        int windowSize_;
        bool isPlaying_ = false;
        float playbackTimer_ = 0.0f;
        float frameInterval_ = 0.10f; // e.g. 10 FPS

        std::function<void(int, std::shared_ptr<PointCloud>, double)> onFrameChanged_;

        void loadWindowAround(int centerFrame);
        Frame loadFrame(int frameIndex);
        void fireCallback();
    };
}
