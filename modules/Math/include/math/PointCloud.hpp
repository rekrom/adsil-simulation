#pragma once

#include "Point.hpp"
#include <vector>
#include <memory>
#include <string>

namespace math
{
    class PointCloud
    {
    public:
        PointCloud() = default;
        PointCloud(const std::vector<math::Point> &points);

        void addPoint(const Point &point);
        void addPoints(const std::vector<math::Point> &newPoints);
        const std::vector<math::Point> &getPoints() const;

        std::size_t size() const;

        bool empty() const;

        void clear();

        // Merge two point clouds
        PointCloud operator+(const PointCloud &other) const;

        std::string toString() const;

    private:
        std::vector<math::Point> points_;
    };
}