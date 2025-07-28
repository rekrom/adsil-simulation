#pragma once

#include <memory>
#include <viewer/interfaces/Camera.hpp>
#include <vehicle/Car.hpp>
#include <simulation/implementations/FrameBufferManager.hpp>
#include <core/Logger.hpp>
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
