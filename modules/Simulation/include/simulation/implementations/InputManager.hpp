#pragma once

#include <memory>
#include <viewer/interfaces/Camera.hpp>
#include <core/Car.hpp>
#include <simulation/implementations/FrameBufferManager.hpp>

namespace simulation
{

    class InputManager
    {
    public:
        void processInput(float deltaTime,
                          Car &car,
                          Camera &camera,
                          std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);
    };

} // namespace simulation
