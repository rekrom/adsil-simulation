#pragma once

#include <glm/vec2.hpp>

namespace viewer::input
{

    class IInputManager
    {
    public:
        virtual ~IInputManager() = default;

        // Input update cycle
        virtual void update() = 0;

        // Callbacks from GLFW
        // virtual void onKeyCallback(int key, int action) = 0;
        virtual void onKeyCallback(int key, int scancode, int action, int mods) = 0;

        virtual void onMouseButtonCallback(int button, int action) = 0;
        virtual void onCursorPosCallback(double xpos, double ypos) = 0;
        virtual void onScrollCallback(double xoffset, double yoffset) = 0;

        // Query methods
        virtual bool isKeyPressed(int key) const = 0;
        virtual bool isKeyJustPressed(int key) const = 0;

        virtual bool isMouseButtonPressed(int button) const = 0;
        virtual bool isMouseButtonJustPressed(int button) const = 0;

        virtual glm::vec2 getMouseDelta() const = 0;
        virtual glm::vec2 getScrollDelta() const = 0;
    };

} // namespace input
