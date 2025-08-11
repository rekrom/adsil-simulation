#pragma once

#include <memory>
#include <viewer/interfaces/Camera.hpp>
#include <viewer/interfaces/IInputManager.hpp>
#include <vehicle/Car.hpp>
#include <simulation/implementations/FrameBufferManager.hpp>
#include <core/Logger.hpp>

namespace simulation
{
    /**
     * @brief Simulation-level input handler that uses the viewer's input system
     *
     * This class handles high-level simulation input logic (car movement, frame navigation)
     * by using the low-level input queries provided by the viewer's InputManager.
     * This provides better separation of concerns compared to direct GLFW calls.
     */
    class InputManager
    {
    public:
        /**
         * @brief Constructor that takes the viewer's input manager
         * @param viewerInputManager The low-level input manager from the viewer system
         */
        explicit InputManager(std::shared_ptr<viewer::input::IInputManager> viewerInputManager);

        /**
         * @brief Process simulation-level input logic
         * @param deltaTime Time elapsed since last frame
         * @param car The car object to control
         * @param camera The camera object (for potential camera locks/controls)
         * @param frameBuffer The frame buffer manager for navigation
         */
        void processInput(float deltaTime,
                          Car &car,
                          std::shared_ptr<simulation::FrameBufferManager> &frameBuffer);

    private:
        std::shared_ptr<viewer::input::IInputManager> viewerInputManager_;
    };

} // namespace simulation
