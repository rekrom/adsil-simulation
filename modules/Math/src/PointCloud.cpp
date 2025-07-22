#include <math/PointCloud.hpp>
#include <sstream>

namespace math
{
    PointCloud::PointCloud(const std::vector<math::Point> &points)
        : points_(points) {}

    void PointCloud::addPoint(const Point &point)
    {
        points_.emplace_back(point);
    }

    void PointCloud::addPoints(const std::vector<math::Point> &newPoints)
    {
        points_.insert(points_.end(), newPoints.begin(), newPoints.end());
    }

    const std::vector<math::Point> &PointCloud::getPoints() const
    {
        return points_;
    }

    void PointCloud::clear()
    {
        points_.clear();
    }

    std::size_t PointCloud::size() const
    {
        return points_.size();
    }

    bool PointCloud::empty() const
    {
        return points_.empty();
    }

    PointCloud PointCloud::operator+(const PointCloud &other) const
    {
        std::vector<math::Point> combined = points_;
        combined.insert(combined.end(), other.points_.begin(), other.points_.end());
        return PointCloud(combined);
    }

    std::string PointCloud::toString() const
    {
        std::ostringstream oss;
        oss << "PointCloud(" << points_.size() << " points)";
        return oss.str();
    }
}