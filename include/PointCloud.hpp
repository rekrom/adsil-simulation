#pragma once

#include "Point.hpp"
#include <vector>
#include <memory>
#include <string>

class PointCloud
{
public:
    PointCloud() = default;
    PointCloud(const std::vector<Point> &points);

    void addPoint(const Point &point);
    const std::vector<Point> &getPoints() const;

    std::size_t size() const;

    // Merge two point clouds
    PointCloud operator+(const PointCloud &other) const;

    std::string toString() const;

private:
    std::vector<Point> points_;
};
