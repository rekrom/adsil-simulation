// ImGuiLayer.cpp
#include <viewer/implementations/ImGuiLayer.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

namespace viewer
{
    void ImGuiLayer::init(void *glfwWindow)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Enable docking
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Optional: Enable viewports (allows ImGui windows to be moved outside the main window)
        // Uncomment the line below if you want this feature:
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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

    void ImGuiLayer::beginDockSpace()
    {
        // Create a fullscreen window for the dockspace
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground; // Make background transparent so OpenGL shows through

        // Create the dockspace window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        // Create the dockspace
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        // Optional: Add a menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Show Demo Window", nullptr, nullptr);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void ImGuiLayer::endDockSpace()
    {
        ImGui::End(); // End the dockspace window
    }

    void ImGuiLayer::endFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle viewports if enabled
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
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

        if (selectedPointCloudEntity_)
        {
            selectedPointCloudInspector_.draw(frameBuffer_, selectedPointCloudEntity_);
        }
    }

    void ImGuiLayer::setFrameBuffer(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        frameBuffer_ = frameBuffer;
    }

    void ImGuiLayer::setSelectedPointCloudEntity(const std::shared_ptr<viewer::PointCloudEntity> &entity)
    {
        selectedPointCloudEntity_ = entity;
    }
}
