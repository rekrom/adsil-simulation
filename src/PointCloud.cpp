#include "PointCloud.hpp"
#include <sstream>

PointCloud::PointCloud(const std::vector<Point> &points)
    : points_(points) {}

void PointCloud::addPoint(const Point &point)
{
    points_.emplace_back(point);
}

const std::vector<Point> &PointCloud::getPoints() const
{
    return points_;
}

std::size_t PointCloud::size() const
{
    return points_.size();
}

PointCloud PointCloud::operator+(const PointCloud &other) const
{
    std::vector<Point> combined = points_;
    combined.insert(combined.end(), other.points_.begin(), other.points_.end());
    return PointCloud(combined);
}

std::string PointCloud::toString() const
{
    std::ostringstream oss;
    oss << "PointCloud(" << points_.size() << " points)";
    return oss.str();
}
