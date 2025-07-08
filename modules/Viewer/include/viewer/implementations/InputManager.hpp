#pragma once
#include <viewer/interfaces/IInputManager.hpp>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

namespace viewer::input
{

    class InputManager : public IInputManager
    {
    public:
        void update() override;

        void onKeyCallback(int key, int action) override;
        void onMouseButtonCallback(int button, int action) override;
        void onCursorPosCallback(double xpos, double ypos) override;
        void onScrollCallback(double xoffset, double yoffset) override;

        bool isKeyPressed(int key) const override;
        bool isKeyJustPressed(int key) const override;

        bool isMouseButtonPressed(int button) const override;
        bool isMouseButtonJustPressed(int button) const override;

        glm::vec2 getMouseDelta() const override;
        glm::vec2 getScrollDelta() const override;

    private:
        std::unordered_map<int, bool> currentKeys_;
        std::unordered_map<int, bool> previousKeys_;

        std::unordered_map<int, bool> currentMouseButtons_;
        std::unordered_map<int, bool> previousMouseButtons_;

        glm::vec2 lastMousePos_{0.0f};
        glm::vec2 currentMousePos_{0.0f};
        glm::vec2 mouseDelta_{0.0f};

        glm::vec2 scrollOffset_{0.0f};
    };

} // namespace input