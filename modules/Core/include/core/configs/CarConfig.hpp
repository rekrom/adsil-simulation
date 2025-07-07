#pragma once

#include <spatial/implementations/TransformNode.hpp>
#include <geometry/implementations/Device.hpp>
#include <vector>
#include <memory>

struct CarDimension final
{
    float length;
    float width;
    float height;

    constexpr CarDimension(float l = 0.0f, float w = 0.0f, float h = 0.0f)
        : length(l), width(w), height(h) {}
};

struct CarConfig
{
    std::shared_ptr<spatial::TransformNode> transformNode;
    SharedVec<Device> transmitters;
    SharedVec<Device> receivers;
    CarDimension dimension;

    CarConfig(std::shared_ptr<spatial::TransformNode> node,
              const SharedVec<Device> &tx,
              const SharedVec<Device> &rx, CarDimension dims)
        : transformNode(std::move(node)), transmitters(tx), receivers(rx), dimension(dims)
    {
        assert(transformNode && "CarConfig requires valid transformNode");
    }
};
