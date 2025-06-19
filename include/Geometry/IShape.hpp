#pragma once

#include "Core/PointCloud.hpp"
#include <string>

class IShape
{
public:
    virtual ~IShape() = default;

    virtual std::shared_ptr<PointCloud> surfaceMesh(int quality = 16) const = 0;
    virtual std::vector<Point> wireframe() const = 0;
    virtual std::string toString() const = 0;
};
