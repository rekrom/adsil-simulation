// ImGuiLayer.hpp
#pragma once

#include <memory>
#include <vector>

#include <core/Car.hpp>
#include <viewer/interfaces/Camera.hpp>
#include <viewer/implementations/Renderable.hpp>
#include <viewer/implementations/CarRenderable.hpp>
#include <viewer/implementations/DeviceRenderable.hpp>
#include <viewer/imgui/CarInspectorPanel.hpp>
#include <viewer/imgui/ViewerInspectorPanel.hpp>

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

        void drawUI(const std::vector<std::shared_ptr<Entity>> &entities);

    private:
        imgui::CarInspectorPanel carInspector_;
        imgui::ViewerInspectorPanel viewerInspector_;
    };

} // namespace viewer
