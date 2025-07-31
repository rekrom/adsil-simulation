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
                                    Camera &camera,
                                    std::shared_ptr<simulation::FrameBufferManager> &frameBuffer)
    {

        if (viewerInputManager_->isKeyJustPressed(GLFW_KEY_L))
        {
            LOGGER_INFO("Toggling camera lock state");
            camera.setIsLocked(!camera.getIsLocked());
        }

        // Don't process input if camera is locked
        if (camera.getIsLocked())
        {
            return;
        }

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
        if (viewerInputManager_->isKeyJustPressed(GLFW_KEY_RIGHT))
        {
            frameBuffer->stepForward();
        }
        if (viewerInputManager_->isKeyJustPressed(GLFW_KEY_LEFT))
        {
            frameBuffer->stepBackward();
        }
        if (viewerInputManager_->isKeyJustPressed(GLFW_KEY_SPACE))
        {
            frameBuffer->togglePlayPause();
        }
    }

} // namespace simulation
