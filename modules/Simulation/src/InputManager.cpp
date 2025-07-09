#include <simulation/implementations/InputManager.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
namespace simulation
{

    void InputManager::processInput(float deltaTime,
                                    Car &car,
                                    Camera &camera)
    {
        GLFWwindow *window = glfwGetCurrentContext();
        if (camera.getIsLocked())
        {
            return;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            car.moveForward(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            car.moveForward(-deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            car.rotateYaw(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            car.rotateYaw(-deltaTime);
        }

        // Add camera movement if needed
    }

} // namespace simulation
