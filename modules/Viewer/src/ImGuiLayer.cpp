// ImGuiLayer.cpp
#include <viewer/implementations/ImGuiLayer.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace viewer
{
    void ImGuiLayer::init(void *glfwWindow)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)glfwWindow, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void ImGuiLayer::shutdown()
    {
        if (ImGui::GetCurrentContext() != nullptr)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }
        else
        {
            std::cerr << "[WARN] ImGui shutdown called without valid context." << std::endl;
        }
    }

    void ImGuiLayer::beginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::endFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiLayer::drawViewerPanel(Camera &camera, RenderingMode &mode, int &fps)
    {
        viewerInspector_.draw(camera, mode, fps);
    }

    void ImGuiLayer::drawUI(const SharedVec<Entity> &entities)
    {
        for (const auto &entity : entities)
        {
            if (!entity || !entity->isVisible())
                continue;

            // Identify CarEntity
            auto carEntity = std::dynamic_pointer_cast<viewer::CarEntity>(entity);
            if (carEntity)
            {
                carInspector_.draw(carEntity);
                continue;
            }

            // (optional) Handle other types, like CubeEntity, etc.
        }

        if (frameBuffer_)
        {
            frameManagerInspector_.draw(frameBuffer_);
        }
    }

    void ImGuiLayer::setFrameBuffer(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        frameBuffer_ = frameBuffer;
    }

}
