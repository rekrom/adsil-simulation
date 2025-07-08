#pragma once

#include <memory>
#include <viewer/interfaces/Camera.hpp>
#include <core/Car.hpp>

namespace simulation
{

    class InputManager
    {
    public:
        void processInput(float deltaTime,
                          Car &car,
                          Camera &camera);
    };

} // namespace simulation
