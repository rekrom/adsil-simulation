#include <viewer/imgui/FrameManagerInspectorPanel.hpp>
#include <imgui.h>

namespace viewer::imgui
{

    void FrameManagerInspectorPanel::draw(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        if (!frameBuffer)
            return;

        ImGui::Begin("Frame Manager");

        drawFrameInfoSection(frameBuffer);
        drawNavigationControls(frameBuffer);
        drawPlaybackControls(frameBuffer); // <-- New
        drawJumpToFrame(frameBuffer);

        ImGui::End();
    }

    void FrameManagerInspectorPanel::drawFrameInfoSection(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        ImGui::Text("Current Frame: %d", frameBuffer->getCurrentFrameIndex());
        ImGui::Text("Total Frames: %d", frameBuffer->getTotalFrameCount());

        const auto &currentFrame = frameBuffer->getCurrentFrame();
        if (currentFrame)
        {
            ImGui::Text("Timestamp: %.4f", currentFrame->timestamp);
        }
    }

    void FrameManagerInspectorPanel::drawNavigationControls(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        if (ImGui::Button("⏪ Step Backward"))
        {
            frameBuffer->stepBackward();
        }
        ImGui::SameLine();
        if (ImGui::Button("Step Forward ⏩"))
        {
            frameBuffer->stepForward();
        }
    }

    void FrameManagerInspectorPanel::drawPlaybackControls(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        ImGui::Spacing();
        if (ImGui::Button("▶ Play"))
        {
            frameBuffer->play();
        }
        ImGui::SameLine();
        if (ImGui::Button("⏸ Pause"))
        {
            frameBuffer->pause();
        }
        ImGui::SameLine();

        static float playbackSpeed_ = frameBuffer->getFPS();
        if (ImGui::SliderFloat("Speed (fps)", &playbackSpeed_, 0.1f, 60.0f, "%.1f"))
        {
            frameBuffer->setFPS(playbackSpeed_);
        }
    }

    void FrameManagerInspectorPanel::drawJumpToFrame(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        ImGui::Spacing();
        ImGui::InputInt("Jump to Frame", &jumpTarget_);
        jumpTarget_ = std::clamp(jumpTarget_, 0, frameBuffer->getTotalFrameCount() - 1);

        if (ImGui::Button("Jump"))
        {
            frameBuffer->seek(jumpTarget_);
        }
    }

}
