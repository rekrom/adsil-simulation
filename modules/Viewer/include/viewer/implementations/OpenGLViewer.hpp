#pragma once

// 1. GLAD daima en önce
#include <glad/glad.h>

// 2. GLFW bundan sonra
#include <GLFW/glfw3.h>

// 3. Diğer includes
#include <viewer/interfaces/Camera.hpp>
#include <viewer/implementations/Renderable.hpp>
#include <viewer/implementations/ImGuiLayer.hpp>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace viewer
{
    class Renderable;

    class OpenGLViewer
    {
    public:
        OpenGLViewer(int width = 1280, int height = 720, const std::string &title = "OpenGL Viewer");
        ~OpenGLViewer();

        void run(); // Main rendering loop

        void addRenderable(const std::shared_ptr<Renderable> &r);

    private:
        void init(int width = 1280, int height = 720, const std::string &title = "OpenGL Viewer");
        void render(); // Placeholder: actual rendering logic goes here
        void cleanup();

        void setupCallbacks();
        void processInput(float deltaTime);
        void updateDeltaTime();

    private:
        GLFWwindow *window_;
        int width_;
        int height_;
        std::string title_;

        ImGuiLayer imguiLayer_;
        Camera camera_;
        float lastX_;
        float lastY_;
        bool firstMouse_;
        bool rightMousePressed_;

        float deltaTime_;
        float lastFrame_;

        std::vector<std::shared_ptr<Renderable>> renderables_;
    };
}