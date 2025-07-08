#include <viewer/implementations/InputManager.hpp>

namespace viewer::input
{

    void InputManager::update()
    {
        previousKeys_ = currentKeys_;
        previousMouseButtons_ = currentMouseButtons_;

        mouseDelta_ = currentMousePos_ - lastMousePos_;
        lastMousePos_ = currentMousePos_;

        scrollOffset_ = glm::vec2(0.0f); // reset every frame
    }

    void InputManager::onKeyCallback(int key, int action)
    {
        currentKeys_[key] = (action != GLFW_RELEASE);
    }

    void InputManager::onMouseButtonCallback(int button, int action)
    {
        currentMouseButtons_[button] = (action != GLFW_RELEASE);
    }

    void InputManager::onCursorPosCallback(double xpos, double ypos)
    {
        currentMousePos_ = glm::vec2(xpos, ypos);
    }

    void InputManager::onScrollCallback(double xoffset, double yoffset)
    {
        scrollOffset_ = glm::vec2(xoffset, yoffset);
    }

    bool InputManager::isKeyPressed(int key) const
    {
        auto it = currentKeys_.find(key);
        return it != currentKeys_.end() && it->second;
    }

    bool InputManager::isKeyJustPressed(int key) const
    {
        auto it = currentKeys_.find(key);
        auto prevIt = previousKeys_.find(key);
        return it != currentKeys_.end() && it->second &&
               (prevIt == previousKeys_.end() || !prevIt->second);
    }

    bool InputManager::isMouseButtonPressed(int button) const
    {
        auto it = currentMouseButtons_.find(button);
        return it != currentMouseButtons_.end() && it->second;
    }

    bool InputManager::isMouseButtonJustPressed(int button) const
    {
        auto it = currentMouseButtons_.find(button);
        auto prevIt = previousMouseButtons_.find(button);
        return it != currentMouseButtons_.end() && it->second &&
               (prevIt == previousMouseButtons_.end() || !prevIt->second);
    }

    glm::vec2 InputManager::getMouseDelta() const
    {
        return mouseDelta_;
    }

    glm::vec2 InputManager::getScrollDelta() const
    {
        return scrollOffset_;
    }

} // namespace input
