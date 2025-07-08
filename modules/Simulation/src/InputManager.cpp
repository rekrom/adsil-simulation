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
            std::cout << "camera is locked" << std::endl;
            return;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            car.moveForward(deltaTime * 10.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            car.moveForward(-deltaTime * 10.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            car.rotateYaw(deltaTime * 10.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            car.rotateYaw(-deltaTime * 10.0f);
        }

        // Add camera movement if needed
    }

} // namespace simulation
