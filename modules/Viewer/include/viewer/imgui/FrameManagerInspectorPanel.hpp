#pragma once

#include <memory>
#include <simulation/implementations/FrameBufferManager.hpp>

namespace viewer::imgui
{

    class FrameManagerInspectorPanel
    {
    public:
        void draw(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);

    private:
        void drawFrameInfoSection(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);
        void drawNavigationControls(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);
        void drawJumpToFrame(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);

        int jumpTarget_ = 0;
    };
}
