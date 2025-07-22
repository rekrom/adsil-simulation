#pragma once

#include <math/PointCloud.hpp>
#include <core/ResourceLocator.hpp>
#include <functional>
#include <deque>
#include <string>
#include <memory>
#include <simulation/implementations/Frame.hpp>
#include <adapter/AdapterManager.hpp> // Assuming this loads PointCloud
#include <simulation/interfaces/IFrameObserver.hpp>
#include <core/Logger.hpp>

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

        std::shared_ptr<PointCloud> getCurrentCloud() const;
        double getCurrentTimestamp() const;
        int getCurrentFrameIndex() const { return currentFrameIndex_; }
        int getTotalFrameCount() const { return totalFrameCount_; }

        std::shared_ptr<Frame> getCurrentFrame() const;

        // void setOnFrameChanged(std::function<void(int, std::shared_ptr<PointCloud>, double)> cb);

        void addFrameObserver(const std::shared_ptr<IFrameObserver> &observer);

    private:
        std::unique_ptr<adapter::AdapterManager> adapters_;
        std::shared_ptr<Frame> frame_;

        std::deque<std::shared_ptr<Frame>> frameWindow_;
        std::string frameDir_;
        int currentFrameIndex_ = 0;
        int totalFrameCount_ = 0;

        int windowSize_;
        bool isPlaying_ = false;
        float playbackTimer_ = 0.0f;
        float frameInterval_ = 0.10f; // e.g. 10 FPS

        std::vector<std::weak_ptr<IFrameObserver>> frameObservers_; // avoid ownership cycle

        std::function<void(int, std::shared_ptr<PointCloud>, double)> onFrameChanged_;

        void loadWindowAround(int centerFrame);
        std::shared_ptr<Frame> loadFrame(int frameIndex);
        void fireCallback();
    };
}
