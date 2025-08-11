#include <simulation/implementations/InputManager.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

namespace simulation
{
    InputManager::InputManager(std::shared_ptr<viewer::input::IInputManager> viewerInputManager)
        : viewerInputManager_(std::move(viewerInputManager))
    {
        if (!viewerInputManager_)
        {
            throw std::invalid_argument("InputManager requires a valid viewer input manager");
        }
    }

    void InputManager::processInput(float deltaTime,
                                    Car &car,
                                    std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {
        // Car movement controls using the viewer's input system
        if (viewerInputManager_->isKeyPressed(GLFW_KEY_W))
        {
            car.moveForward(deltaTime);
        }
        if (viewerInputManager_->isKeyPressed(GLFW_KEY_S))
        {
            car.moveForward(-deltaTime);
        }
        if (viewerInputManager_->isKeyPressed(GLFW_KEY_Q))
        {
            car.rotateYaw(deltaTime);
        }
        if (viewerInputManager_->isKeyPressed(GLFW_KEY_E))
        {
            car.rotateYaw(-deltaTime);
        }

        // Frame buffer navigation controls
        if (viewerInputManager_->isKeyJustPressed(GLFW_KEY_KP_ADD))
        {
            frameBuffer->stepForward();
        }
        if (viewerInputManager_->isKeyJustPressed(GLFW_KEY_KP_SUBTRACT))
        {
            frameBuffer->stepBackward();
        }
        if (viewerInputManager_->isKeyJustPressed(GLFW_KEY_SPACE))
        {
            frameBuffer->togglePlayPause();
        }
    }

} // namespace simulation
