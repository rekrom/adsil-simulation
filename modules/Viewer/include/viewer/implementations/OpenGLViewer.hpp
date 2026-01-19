#pragma once

// 1. GLAD daima en önce
#include <glad/glad.h>

// 2. GLFW bundan sonra
#include <GLFW/glfw3.h>

// 3. Diğer includes
#include <viewer/interfaces/IViewer.hpp>
#include <viewer/interfaces/Camera.hpp>
#include <viewer/renderables/Renderable.hpp>
#include <viewer/entities/Entity.hpp>
#include <viewer/implementations/ImGuiLayer.hpp>
#include <viewer/interfaces/IInputManager.hpp>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <viewer/common/RenderingTypes.hpp> // instead of defining the enum
#include <core/Logger.hpp>

namespace simulation
{
    class FrameBufferManager;
}

namespace viewer
{
    class Renderable;

    class OpenGLViewer : public IViewer
    {
    public:
        OpenGLViewer(int width = 1280, int height = 720, const std::string &title = "OpenGL Viewer");
        ~OpenGLViewer() override;

        void run(); // Main rendering loop

        void addEntity(const std::shared_ptr<Entity> &e) override;

        void setEntities(SharedVec<Entity> e) override;

        void setRenderingMode(RenderingMode mode) override;

        void setFrameManager(std::shared_ptr<simulation::FrameBufferManager> frameBuffer) override;

        void setSelectedPointCloudEntity(const std::shared_ptr<viewer::PointCloudEntity> &entity) override;

        RenderingMode getRenderingMode() const override;

        Camera &getCamera() { return camera_; }

        float getDeltaTime() const override;

        void render() override;
        void cleanup() override;
        bool shouldClose() const override;
        void initGraphics() override;
        void initEntities() override;

        std::shared_ptr<input::IInputManager> getInputManager() const override { return inputManager_; }

    private:
        void init(int width = 1280, int height = 720, const std::string &title = "OpenGL Viewer");

        void setupCallbacks();
        void processInput(float deltaTime);
        void updateDeltaTime();

        glm::mat4 getProjectionMatrix() const;
        void updateFPSCounter();
        int getFPS();

        void renderEntities();

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