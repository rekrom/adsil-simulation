// ImGuiLayer.hpp
#pragma once

#include <memory>
#include <vector>

#include <vehicle/Car.hpp>
#include <viewer/interfaces/Camera.hpp>
#include <viewer/renderables/Renderable.hpp>
#include <viewer/renderables/CarRenderable.hpp>
#include <viewer/renderables/DeviceRenderable.hpp>
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

        void beginDockSpace();
        void endDockSpace();

        // Unified method to render complete UI
        void renderUI(Camera &camera, RenderingMode &mode, int &fps, const SharedVec<Entity> &entities);

        void drawViewerPanel(Camera &camera, RenderingMode &mode, int &fps);
        void drawUI(const SharedVec<Entity> &entities);
        void setFrameBuffer(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);
        void setSelectedPointCloudEntity(const std::shared_ptr<viewer::PointCloudEntity> &entity);

    private:
        imgui::CarInspectorPanel carInspector_;
        imgui::ViewerInspectorPanel viewerInspector_;
        imgui::FrameManagerInspectorPanel frameManagerInspector_;
        imgui::SelectedPointCloudInspectorPanel selectedPointCloudInspector_;

        std::shared_ptr<simulation::FrameBufferManager> frameBuffer_;        // ✅ NEW
        std::shared_ptr<viewer::PointCloudEntity> selectedPointCloudEntity_; // ✅ NEW
    };

} // namespace viewer
