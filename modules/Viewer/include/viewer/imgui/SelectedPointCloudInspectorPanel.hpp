#pragma once

#include <memory>
#include <imgui.h>
#include <math/PointCloud.hpp>
#include <simulation/implementations/FrameBufferManager.hpp>
#include <viewer/entities/PointCloudEntity.hpp>

namespace viewer::imgui
{
    class SelectedPointCloudInspectorPanel
    {
    public:
        void draw(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer,
                  const std::shared_ptr<viewer::PointCloudEntity> &pointCloudEntity);

    private:
        void drawPointCloudInfoSection(const std::shared_ptr<math::PointCloud> &pointCloud);
        void drawPointCloudStatsSection(const std::shared_ptr<math::PointCloud> &pointCloud);
        void drawPointCloudVisualizationSection(const std::shared_ptr<viewer::PointCloudEntity> &pointCloudEntity);
        void drawPointCloudDataSection(const std::shared_ptr<math::PointCloud> &pointCloud);
        void drawFrameInfoSection(const std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);

        // Visualization settings
        bool showPointCloudData_ = false;
        bool showFrameInfo_ = true;
        bool showStats_ = true;
        bool showVisualization_ = true;

        // Point cloud data display settings
        int maxPointsToShow_ = 100;
        int pointDisplayOffset_ = 0;
        bool showAsTable_ = true;

        // Filter settings
        bool enablePositionFilter_ = false;
        float minPosition_[3] = {-10.0f, -10.0f, -10.0f};
        float maxPosition_[3] = {10.0f, 10.0f, 10.0f};

        // Statistics cache
        mutable bool statsValid_ = false;
        mutable math::Point centerPoint_;
        mutable float boundingBoxMin_[3] = {0.0f, 0.0f, 0.0f};
        mutable float boundingBoxMax_[3] = {0.0f, 0.0f, 0.0f};
        mutable float averageDistance_ = 0.0f;

        void calculateStatistics(const std::shared_ptr<math::PointCloud> &pointCloud) const;
        bool isPointInFilter(const math::Point &point) const;
    };
}
