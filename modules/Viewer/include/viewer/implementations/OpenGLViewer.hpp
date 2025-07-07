#pragma once

// 1. GLAD daima en önce
#include <glad/glad.h>

// 2. GLFW bundan sonra
#include <GLFW/glfw3.h>

// 3. Diğer includes
#include <viewer/interfaces/Camera.hpp>
#include <viewer/implementations/Renderable.hpp>
#include <viewer/entities/Entity.hpp>
#include <viewer/implementations/ImGuiLayer.hpp>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <viewer/common/RenderingTypes.hpp> // instead of defining the enum

namespace viewer
{
    class Renderable;

    class OpenGLViewer
    {
    public:
        OpenGLViewer(int width = 1280, int height = 720, const std::string &title = "OpenGL Viewer");
        ~OpenGLViewer();

        void run(); // Main rendering loop

        void addEntity(const std::shared_ptr<Entity> &e);

        void setRenderingMode(RenderingMode mode);

        RenderingMode getRenderingMode() const;

    private:
        void init(int width = 1280, int height = 720, const std::string &title = "OpenGL Viewer");
        void render(); // Placeholder: actual rendering logic goes here
        void cleanup();

        void setupCallbacks();
        void processInput(float deltaTime);
        void updateDeltaTime();

        glm::mat4 getProjectionMatrix() const;
        void updateFPSCounter();
        int getFPS();

    private:
        GLFWwindow *window_;
        int width_;
        int height_;
        std::string title_;
        Camera camera_;
        float lastX_;
        float lastY_;
        bool firstMouse_;
        bool rightMousePressed_;
        float deltaTime_;
        float lastFrame_;
        RenderingMode renderingMode_;
        ImGuiLayer imguiLayer_;
        int displayedFPS_ = 0;

        SharedVec<Renderable> renderables_;
        SharedVec<Entity> entities_;
    };
}