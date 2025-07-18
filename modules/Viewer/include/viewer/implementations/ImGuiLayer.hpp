// ImGuiLayer.hpp
#pragma once

#include <memory>
#include <vector>

#include <core/Car.hpp>
#include <viewer/interfaces/Camera.hpp>
#include <viewer/implementations/Renderable.hpp>
#include <viewer/implementations/CarRenderable.hpp>
#include <viewer/implementations/DeviceRenderable.hpp>
#include <viewer/imgui/panels.hpp>

namespace simulation
{
    class FrameBufferManager;
}

namespace viewer
{

    class ImGuiLayer
    {
    public:
        void init(void *glfwWindow);
        void shutdown();
        void beginFrame();
        void endFrame();

        void drawViewerPanel(Camera &camera, RenderingMode &mode, int &fps);
        void drawUI(const SharedVec<Entity> &entities);
        void setFrameBuffer(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);

    private:
        imgui::CarInspectorPanel carInspector_;
        imgui::ViewerInspectorPanel viewerInspector_;
        imgui::FrameManagerInspectorPanel frameManagerInspector_;

        std::shared_ptr<simulation::FrameBufferManager> frameBuffer_; // ✅ NEW
    };

} // namespace viewer
