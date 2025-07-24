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
#include <viewer/interfaces/IInputManager.hpp>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <viewer/common/RenderingTypes.hpp> // instead of defining the enum

namespace simulation
{
    class FrameBufferManager;
}

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

        void setEntities(SharedVec<Entity> e);

        void setRenderingMode(RenderingMode mode);

        void setFrameManager(std::shared_ptr<simulation::FrameBufferManager> frameBuffer);

        void setSelectedPointCloudEntity(const std::shared_ptr<viewer::PointCloudEntity> &entity);

        RenderingMode getRenderingMode() const;

        Camera &getCamera() { return camera_; }

        float getDeltaTime() const;

        void render(); // Placeholder: actual rendering logic goes here
        void cleanup();
        bool shouldClose() const;
        void initGraphics();
        void initEntities();

        std::shared_ptr<input::IInputManager> getInputManager() const { return inputManager_; }

    private:
        void init(int width = 1280, int height = 720, const std::string &title = "OpenGL Viewer");

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

        SharedVec<Entity> entities_;

        std::shared_ptr<input::IInputManager> inputManager_;

        std::shared_ptr<simulation::FrameBufferManager> frameManager_;
    };
}