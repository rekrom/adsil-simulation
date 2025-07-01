#pragma once

#include <core/TransformNode.hpp>
#include <geometry/implementations/Device.hpp>
#include <vector>
#include <memory>

struct CarConfig
{
    std::shared_ptr<core::TransformNode> transformNode;
    std::vector<std::shared_ptr<Device>> transmitters;
    std::vector<std::shared_ptr<Device>> receivers;

    CarConfig(std::shared_ptr<core::TransformNode> node,
              const std::vector<std::shared_ptr<Device>> &tx,
              const std::vector<std::shared_ptr<Device>> &rx)
        : transformNode(std::move(node)), transmitters(tx), receivers(rx)
    {
        assert(transformNode && "CarConfig requires valid transformNode");
    }
};
