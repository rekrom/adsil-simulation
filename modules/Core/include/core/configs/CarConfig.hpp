#pragma once

#include <core/Transform.hpp>
#include <device/Device.hpp>
#include <vector>
#include <memory>

struct CarConfig
{
    std::shared_ptr<core::TransformNode> transformNode; // optionally provide pre-made transform node
    Transform transform;                                // fallback if transformNode not provided

    std::vector<std::shared_ptr<Device>> transmitters;
    std::vector<std::shared_ptr<Device>> receivers;

    CarConfig() = default;

    // Convenience constructor for quick initialization with transform
    CarConfig(const Transform &t,
              const std::vector<std::shared_ptr<Device>> &tx = {},
              const std::vector<std::shared_ptr<Device>> &rx = {})
        : transform(t), transmitters(tx), receivers(rx) {}

    // Optional: you can add other config parameters as needed
};
