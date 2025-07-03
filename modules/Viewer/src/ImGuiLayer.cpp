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
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
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

    void ImGuiLayer::drawUI(const std::vector<std::shared_ptr<Renderable>> &renderables)
    {
        for (const auto &r : renderables)
        {
            auto carRenderable = std::dynamic_pointer_cast<CarRenderable>(r);
            if (!carRenderable)
                continue;

            auto car = carRenderable->getCar(); // Assume CarRenderable::getCar() returns std::shared_ptr<Car>

            if (ImGui::Begin("Device FOV Control"))
            {

                ImGui::Text("Transmitters:");
                for (auto &tx : car->getTransmitters())
                {
                    // float hFov = tx->getHorizontalFoV();
                    // float vFov = tx->getVerticalFoV();

                    // std::string labelH = "TX " + tx->getId() + " - Horizontal FoV";
                    // std::string labelV = "TX " + tx->getId() + " - Vertical FoV";

                    // if (ImGui::SliderFloat(labelH.c_str(), &hFov, 1.0f, 180.0f))
                    // {
                    //     tx->setHorizontalFoV(hFov);
                    // }
                    // if (ImGui::SliderFloat(labelV.c_str(), &vFov, 1.0f, 180.0f))
                    // {
                    //     tx->setVerticalFoV(vFov);
                    // }
                }

                ImGui::Separator();
                ImGui::Text("Receivers:");
                for (auto &rx : car->getReceivers())
                {
                    // float hFov = rx->getHorizontalFoV();
                    // float vFov = rx->getVerticalFoV();

                    // std::string labelH = "RX " + rx->getId() + " - Horizontal FoV";
                    // std::string labelV = "RX " + rx->getId() + " - Vertical FoV";

                    // if (ImGui::SliderFloat(labelH.c_str(), &hFov, 1.0f, 180.0f))
                    // {
                    //     rx->setHorizontalFoV(hFov);
                    // }
                    // if (ImGui::SliderFloat(labelV.c_str(), &vFov, 1.0f, 180.0f))
                    // {
                    //     rx->setVerticalFoV(vFov);
                    // }
                }
            }
            ImGui::End();
        }
    }
}
