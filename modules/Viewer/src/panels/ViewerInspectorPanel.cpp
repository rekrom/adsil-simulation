#include <viewer/imgui/ViewerInspectorPanel.hpp>
#include "imgui.h"

namespace viewer::imgui
{
    void ViewerInspectorPanel::draw(Camera &camera, RenderingMode &mode, int &fps)
    {
        if (ImGui::Begin("Viewer Inspector"))
        {
            if (showFPSSettings_)
                drawFPS(fps);

            if (showCameraSettings_)
                drawCameraSection(camera);

            if (showRenderingSettings_)
                drawRenderingModeSection(mode);
        }
        ImGui::End();
    }

    void ViewerInspectorPanel::drawCameraSection(Camera &camera)
    {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool isLocked = camera.getIsLocked();
            if (ImGui::Checkbox("Lock", &isLocked))
            {
                camera.setIsLocked(isLocked);
            }

            glm::vec3 pos = camera.getPosition();
            if (ImGui::DragFloat3("Position", &pos[0], 0.1F))
            {
                // camera.setPosition(pos);
            }

            float pitch = 0.0F; // camera.getPitch();
            float yaw = 0.0F;   // camera.getYaw();
            if (ImGui::SliderFloat("Pitch", &pitch, -89.0F, 89.0F))
            {
                // camera.setPitch(pitch);
            }
            if (ImGui::SliderFloat("Yaw", &yaw, -180.0F, 180.0F))
            {
                // camera.setYaw(yaw);
            }
        }
    }

    void ViewerInspectorPanel::drawRenderingModeSection(RenderingMode &mode)
    {
        if (ImGui::CollapsingHeader("Rendering Mode", ImGuiTreeNodeFlags_DefaultOpen))
        {
            int currentMode = static_cast<int>(mode);
            if (ImGui::Combo("Mode", &currentMode, "Perspective\0Orthogonal\0"))
            {
                mode = static_cast<RenderingMode>(currentMode);
            }
        }
    }

    void ViewerInspectorPanel::drawFPS(int &fps)
    {
        if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("FPS: %d", fps);
        }
    }

}
