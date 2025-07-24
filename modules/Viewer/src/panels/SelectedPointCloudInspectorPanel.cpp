#include "viewer/imgui/SelectedPointCloudInspectorPanel.hpp"
#include <algorithm>
#include <cmath>

namespace viewer::imgui
{
    void SelectedPointCloudInspectorPanel::draw(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer,
                                                const std::shared_ptr<viewer::PointCloudEntity> &pointCloudEntity)
    {
        if (ImGui::Begin("Captured Point Cloud Inspector"))
        {
            auto currentCloud = frameBuffer ? frameBuffer->getCurrentCloud() : nullptr;

            if (!currentCloud || currentCloud->empty())
            {
                ImGui::Text("No point cloud data available");
                ImGui::End();
                return;
            }

            // Invalidate stats cache when point cloud changes
            static std::weak_ptr<math::PointCloud> lastCloud;
            if (lastCloud.lock() != currentCloud)
            {
                statsValid_ = false;
                lastCloud = currentCloud;
            }

            if (showFrameInfo_)
                drawFrameInfoSection(frameBuffer);

            if (showStats_)
                drawPointCloudStatsSection(currentCloud);

            if (showVisualization_ && pointCloudEntity)
                drawPointCloudVisualizationSection(pointCloudEntity);

            drawPointCloudInfoSection(currentCloud);

            if (showPointCloudData_)
                drawPointCloudDataSection(currentCloud);
        }
        ImGui::End();
    }

    void SelectedPointCloudInspectorPanel::drawFrameInfoSection(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        if (ImGui::CollapsingHeader("Frame Information", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Show Frame Info", &showFrameInfo_);

            if (frameBuffer)
            {
                ImGui::Text("Current Frame: %d / %d",
                            frameBuffer->getCurrentFrameIndex(),
                            frameBuffer->getTotalFrameCount());
                ImGui::Text("Timestamp: %.4f", frameBuffer->getCurrentTimestamp());
            }
        }
    }

    void SelectedPointCloudInspectorPanel::drawPointCloudInfoSection(const std::shared_ptr<math::PointCloud> &pointCloud)
    {
        if (ImGui::CollapsingHeader("Point Cloud Information", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Total Points: %zu", pointCloud->size());
            ImGui::Text("Memory Usage: %.2f KB",
                        static_cast<float>((static_cast<double>(pointCloud->size()) * 3 * sizeof(float)) / 1024.0));

            ImGui::Separator();

            ImGui::Checkbox("Show Statistics", &showStats_);
            ImGui::SameLine();
            ImGui::Checkbox("Show Visualization Controls", &showVisualization_);
            ImGui::Checkbox("Show Raw Point Data", &showPointCloudData_);

            if (showPointCloudData_)
            {
                ImGui::Separator();
                ImGui::Text("Data Display Settings:");
                ImGui::SliderInt("Max Points to Show", &maxPointsToShow_, 10, 1000);
                ImGui::SliderInt("Display Offset", &pointDisplayOffset_, 0,
                                 std::max(0, static_cast<int>(pointCloud->size()) - maxPointsToShow_));
                ImGui::Checkbox("Show as Table", &showAsTable_);
            }
        }
    }

    void SelectedPointCloudInspectorPanel::drawPointCloudStatsSection(const std::shared_ptr<math::PointCloud> &pointCloud)
    {
        if (ImGui::CollapsingHeader("Point Cloud Statistics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (!statsValid_)
            {
                calculateStatistics(pointCloud);
            }

            ImGui::Text("Bounding Box:");
            ImGui::Text("  Min: (%.3f, %.3f, %.3f)", boundingBoxMin_[0], boundingBoxMin_[1], boundingBoxMin_[2]);
            ImGui::Text("  Max: (%.3f, %.3f, %.3f)", boundingBoxMax_[0], boundingBoxMax_[1], boundingBoxMax_[2]);

            float dimensions[3] = {
                boundingBoxMax_[0] - boundingBoxMin_[0],
                boundingBoxMax_[1] - boundingBoxMin_[1],
                boundingBoxMax_[2] - boundingBoxMin_[2]};
            ImGui::Text("  Dimensions: (%.3f, %.3f, %.3f)", dimensions[0], dimensions[1], dimensions[2]);

            ImGui::Separator();
            ImGui::Text("Center Point: (%.3f, %.3f, %.3f)",
                        centerPoint_.x(), centerPoint_.y(), centerPoint_.z());
            ImGui::Text("Average Distance from Center: %.3f", averageDistance_);

            ImGui::Separator();

            // Point density estimation
            float volume = dimensions[0] * dimensions[1] * dimensions[2];
            if (volume > 0.0f)
            {
                float density = static_cast<float>(pointCloud->size()) / volume;
                ImGui::Text("Point Density: %.3f points/unit³", density);
            }

            // Quick statistics refresh button
            if (ImGui::Button("Refresh Statistics"))
            {
                statsValid_ = false;
            }
        }
    }

    void SelectedPointCloudInspectorPanel::drawPointCloudVisualizationSection(const std::shared_ptr<viewer::PointCloudEntity> &pointCloudEntity)
    {
        if (ImGui::CollapsingHeader("Visualization Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool visible = pointCloudEntity->isVisible();
            if (ImGui::Checkbox("Visible", &visible))
            {
                pointCloudEntity->setVisible(visible);
            }

            glm::vec3 color = pointCloudEntity->getColor();
            float colorArr[3] = {color.r, color.g, color.b};
            if (ImGui::ColorEdit3("Point Color", colorArr))
            {
                pointCloudEntity->setColor(glm::vec3(colorArr[0], colorArr[1], colorArr[2]));
            }

            // Point size control (if the renderable supports it)
            static float pointSize = 5.0f;
            if (ImGui::SliderFloat("Point Size", &pointSize, 1.0f, 20.0f))
            {
                pointCloudEntity->setPointSize(pointSize);
            }

            // Alpha control
            static float alpha = 1.0f;
            if (ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f))
            {
                pointCloudEntity->setAlpha(alpha);
            }
        }
    }

    void SelectedPointCloudInspectorPanel::drawPointCloudDataSection(const std::shared_ptr<math::PointCloud> &pointCloud)
    {
        if (ImGui::CollapsingHeader("Raw Point Data", ImGuiTreeNodeFlags_None))
        {
            // Filter controls
            ImGui::Checkbox("Enable Position Filter", &enablePositionFilter_);
            if (enablePositionFilter_)
            {
                ImGui::DragFloat3("Min Position", minPosition_, 0.1f);
                ImGui::DragFloat3("Max Position", maxPosition_, 0.1f);
            }

            ImGui::Separator();

            const auto &points = pointCloud->getPoints();
            int startIdx = std::max(0, pointDisplayOffset_);
            int endIdx = std::min(static_cast<int>(points.size()), startIdx + maxPointsToShow_);

            int visibleCount = 0;
            for (int i = startIdx; i < endIdx; ++i)
            {
                if (enablePositionFilter_ && !isPointInFilter(points[i]))
                    continue;
                visibleCount++;
            }

            ImGui::Text("Showing %d points (indices %d-%d)", visibleCount, startIdx, endIdx - 1);

            if (showAsTable_)
            {
                if (ImGui::BeginTable("PointCloudData", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    ImGui::TableSetupColumn("Index");
                    ImGui::TableSetupColumn("X");
                    ImGui::TableSetupColumn("Y");
                    ImGui::TableSetupColumn("Z");
                    ImGui::TableHeadersRow();

                    for (int i = startIdx; i < endIdx; ++i)
                    {
                        const auto &point = points[i];
                        if (enablePositionFilter_ && !isPointInFilter(point))
                            continue;

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", i);
                        ImGui::TableNextColumn();
                        ImGui::Text("%.3f", point.x());
                        ImGui::TableNextColumn();
                        ImGui::Text("%.3f", point.y());
                        ImGui::TableNextColumn();
                        ImGui::Text("%.3f", point.z());
                    }
                    ImGui::EndTable();
                }
            }
            else
            {
                for (int i = startIdx; i < endIdx; ++i)
                {
                    const auto &point = points[i];
                    if (enablePositionFilter_ && !isPointInFilter(point))
                        continue;

                    ImGui::Text("[%d]: (%.3f, %.3f, %.3f)",
                                i, point.x(), point.y(), point.z());
                }
            }
        }
    }

    void SelectedPointCloudInspectorPanel::calculateStatistics(const std::shared_ptr<math::PointCloud> &pointCloud) const
    {
        if (!pointCloud || pointCloud->empty())
            return;

        const auto &points = pointCloud->getPoints();

        // Initialize bounding box
        boundingBoxMin_[0] = boundingBoxMax_[0] = points[0].x();
        boundingBoxMin_[1] = boundingBoxMax_[1] = points[0].y();
        boundingBoxMin_[2] = boundingBoxMax_[2] = points[0].z();

        // Calculate center and bounding box
        double sumX = 0.0, sumY = 0.0, sumZ = 0.0;
        for (const auto &point : points)
        {
            sumX += point.x();
            sumY += point.y();
            sumZ += point.z();

            boundingBoxMin_[0] = std::min(boundingBoxMin_[0], point.x());
            boundingBoxMin_[1] = std::min(boundingBoxMin_[1], point.y());
            boundingBoxMin_[2] = std::min(boundingBoxMin_[2], point.z());

            boundingBoxMax_[0] = std::max(boundingBoxMax_[0], point.x());
            boundingBoxMax_[1] = std::max(boundingBoxMax_[1], point.y());
            boundingBoxMax_[2] = std::max(boundingBoxMax_[2], point.z());
        }

        centerPoint_ = math::Point(
            static_cast<float>(sumX / static_cast<double>(points.size())),
            static_cast<float>(sumY / static_cast<double>(points.size())),
            static_cast<float>(sumZ / static_cast<double>(points.size())));

        // Calculate average distance from center
        double totalDistance = 0.0;
        for (const auto &point : points)
        {
            float dx = point.x() - centerPoint_.x();
            float dy = point.y() - centerPoint_.y();
            float dz = point.z() - centerPoint_.z();
            totalDistance += std::sqrt(dx * dx + dy * dy + dz * dz);
        }
        averageDistance_ = static_cast<float>(totalDistance / static_cast<double>(points.size()));

        statsValid_ = true;
    }

    bool SelectedPointCloudInspectorPanel::isPointInFilter(const math::Point &point) const
    {
        if (!enablePositionFilter_)
            return true;

        return point.x() >= minPosition_[0] && point.x() <= maxPosition_[0] &&
               point.y() >= minPosition_[1] && point.y() <= maxPosition_[1] &&
               point.z() >= minPosition_[2] && point.z() <= maxPosition_[2];
    }
}
