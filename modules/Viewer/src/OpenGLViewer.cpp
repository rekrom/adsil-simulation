#include <glad/glad.h> // ✅ En önce

#include <viewer/implementations/OpenGLViewer.hpp>
#include <iostream>
#include <viewer/implementations/InputManager.hpp> // to construct it

namespace viewer
{
    OpenGLViewer::OpenGLViewer(int width, int height, const std::string &title)
        : window_(nullptr), width_(width), height_(height), title_(title), camera_(
                                                                               glm::vec3(-20.0F, 0.0F, 5.0F), // position
                                                                               glm::vec3(0.0F, 0.0F, 1.0F),   // up direction
                                                                               glm::radians(0.0F),            // yaw
                                                                               glm::radians(0.0F)             // pitch
                                                                               ),
          lastX_(static_cast<float>(width) / 2.0F), lastY_(static_cast<float>(height) / 2.0F),
          firstMouse_(true), rightMousePressed_(false),
          deltaTime_(0.0F), lastFrame_(0.0F),
          renderingMode_(viewer::RenderingMode::Perspective),
          imguiLayer_(),    // ✅ açıkça ekle
          displayedFPS_(0), // ✅ tekrar ekle (netlik için)
          entities_(),      // ✅ boş başlat
          inputManager_(std::make_shared<input::InputManager>())
    {
    }

    OpenGLViewer::~OpenGLViewer()
    {
    }

    void OpenGLViewer::initGraphics()
    {
        init(width_, height_, title_);
    }

    void OpenGLViewer::initEntities()
    {
        for (auto &e : entities_)
        {
            e->initGL();
        }
    }

    void OpenGLViewer::init(int width, int height, const std::string &title)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        // OpenGL 3.3 Core Profile
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!window_)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window_);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *, int w, int h)
                                       { glViewport(0, 0, w, h); });

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // Depth buffer should be writable!

        setupCallbacks(); // Add this line just before returning

        imguiLayer_.init(window_);
        imguiLayer_.setFrameBuffer(frameManager_);
    }

    void OpenGLViewer::updateDeltaTime()
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime_ = currentFrame - lastFrame_;
        lastFrame_ = currentFrame;
    }

    void OpenGLViewer::processInput(float deltaTime)
    {

        if (inputManager_->isKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window_, true);

        if (inputManager_->isKeyPressed(GLFW_KEY_UP))
            camera_.processKeyboard('W', deltaTime);
        if (inputManager_->isKeyPressed(GLFW_KEY_DOWN))
            camera_.processKeyboard('S', deltaTime);
        if (inputManager_->isKeyPressed(GLFW_KEY_LEFT))
            camera_.processKeyboard('A', deltaTime);
        if (inputManager_->isKeyPressed(GLFW_KEY_RIGHT))
            camera_.processKeyboard('D', deltaTime);
        if (inputManager_->isKeyPressed(GLFW_KEY_KP_4))
            camera_.processKeyboard('Q', deltaTime);
        if (inputManager_->isKeyPressed(GLFW_KEY_KP_6))
            camera_.processKeyboard('E', deltaTime);

        if (inputManager_->isKeyPressed(GLFW_KEY_KP_8))
            camera_.processKeyboard('Z', deltaTime);
        if (inputManager_->isKeyPressed(GLFW_KEY_KP_2))
            camera_.processKeyboard('C', deltaTime);

        if (inputManager_->isKeyJustPressed(GLFW_KEY_L))
            camera_.processKeyboard('L', deltaTime);

        if (inputManager_->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
        {
            glm::vec2 delta = inputManager_->getMouseDelta();
            camera_.processMouseMovement(delta.x, delta.y);
        }

        glm::vec2 scroll = inputManager_->getScrollDelta();
        if (scroll.y != 0.0F)
            camera_.processMouseScroll(scroll.y);
    }

    void OpenGLViewer::setupCallbacks()
    {
        glfwSetWindowUserPointer(window_, this);

        glfwSetCursorPosCallback(window_, [](GLFWwindow *win, double xpos, double ypos)
                                 {
        auto* viewer = static_cast<OpenGLViewer*>(glfwGetWindowUserPointer(win));
        if (viewer && viewer->inputManager_) {
            viewer->inputManager_->onCursorPosCallback(xpos, ypos);
        } });

        glfwSetMouseButtonCallback(window_, [](GLFWwindow *win, int button, int action, int)
                                   {
    auto* viewer = static_cast<OpenGLViewer*>(glfwGetWindowUserPointer(win));
    if (viewer && viewer->inputManager_) {
        viewer->inputManager_->onMouseButtonCallback(button, action);
    } });

        glfwSetScrollCallback(window_, [](GLFWwindow *win, double xoffset, double yoffset)
                              {
    auto* viewer = static_cast<OpenGLViewer*>(glfwGetWindowUserPointer(win));
    if (viewer && viewer->inputManager_) {
        viewer->inputManager_->onScrollCallback(xoffset, yoffset);
    } });
        glfwSetKeyCallback(window_, [](GLFWwindow *win, int key, int scancode, int action, int mods)
                           {
        auto* viewer = static_cast<OpenGLViewer*>(glfwGetWindowUserPointer(win));
        if (viewer && viewer->inputManager_) {
            viewer->inputManager_->onKeyCallback(key, scancode, action, mods);
        } });
    }

    void OpenGLViewer::setRenderingMode(RenderingMode mode)
    {
        renderingMode_ = mode;
    }

    void OpenGLViewer::setFrameManager(std::shared_ptr<simulation::FrameBufferManager> frameBuffer)
    {
        frameManager_ = std::move(frameBuffer);
    }

    void OpenGLViewer::setSelectedPointCloudEntity(const std::shared_ptr<viewer::PointCloudEntity> &entity)
    {
        imguiLayer_.setSelectedPointCloudEntity(entity);
    }

    RenderingMode OpenGLViewer::getRenderingMode() const
    {
        return renderingMode_;
    }

    void OpenGLViewer::run()
    {
        try
        {
            init(width_, height_, title_);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Initialization error: " << e.what() << std::endl;
            return;
        }
        for (auto &e : entities_)
        {
            e->initGL();
        }

        while (!glfwWindowShouldClose(window_))
        {

            render();
        }
    }

    bool OpenGLViewer::shouldClose() const
    {
        return glfwWindowShouldClose(window_);
    }

    glm::mat4 OpenGLViewer::getProjectionMatrix() const
    {
        float aspect = static_cast<float>(width_) / static_cast<float>(height_);
        if (renderingMode_ == RenderingMode::Perspective)
        {
            return glm::perspective(glm::radians(camera_.getFov()), aspect, 0.1F, 1000.0F);
        }
        else
        {
            float orthoScale = camera_.getFov(); // You may rename this
            return glm::ortho(-orthoScale * aspect, orthoScale * aspect,
                              -orthoScale, orthoScale,
                              0.1F, 1000.0F);
        }
    }

    void OpenGLViewer::updateFPSCounter()
    {
        static int frameCount_ = 0;
        static double lastTime = glfwGetTime();

        ++frameCount_;
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= 1.0)
        {
            // std::cout << "FPS: " << frameCount << std::endl;
            displayedFPS_ = frameCount_;

            frameCount_ = 0;
            lastTime = currentTime;
        }
    }

    int OpenGLViewer::getFPS()
    {
        return displayedFPS_;
    }

    float OpenGLViewer::getDeltaTime() const
    {
        return deltaTime_;
    }

    void OpenGLViewer::render()
    {
        updateDeltaTime();
        inputManager_->update();
        processInput(deltaTime_);

        // Clear screen FIRST
        glClearColor(0.1F, 0.1F, 0.15F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///////// render start here
        updateFPSCounter();
        renderEntities();
        // /// render ends here

        // Render complete UI
        imguiLayer_.renderUI(camera_, renderingMode_, displayedFPS_, entities_);

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    void OpenGLViewer::renderEntities()
    {
        glm::mat4 view = camera_.getViewMatrix();
        glm::mat4 projection = getProjectionMatrix();

        SharedVec<Entity> opaqueEntities;
        SharedVec<Entity> transparentEntities;

        for (auto &entity : entities_)
        {
            if (!entity || !entity->getRenderable())
                continue;

            if (entity->isTransparent())
                transparentEntities.push_back(entity);
            else
                opaqueEntities.push_back(entity);
        }

        if (!transparentEntities.empty())
        {
            std::sort(transparentEntities.begin(), transparentEntities.end(),
                      [this](const std::shared_ptr<Entity> &a, const std::shared_ptr<Entity> &b)
                      {
                          glm::vec3 camPos = camera_.getPosition();
                          glm::vec3 aPos = a->getCenter();
                          glm::vec3 bPos = b->getCenter();
                          return glm::distance(camPos, aPos) > glm::distance(camPos, bPos); // back-to-front
                      });
        }

        for (auto &entity : opaqueEntities)
        {
            entity->render(view, projection);
        }

        glDepthMask(GL_FALSE);
        for (auto &entity : transparentEntities)
        {
            entity->render(view, projection);
        }
        glDepthMask(GL_TRUE);
    }

    void OpenGLViewer::cleanup()
    {
        // 1. entities_ temizlenmeli
        for (auto &e : entities_)
        {
            e->cleanup(); // OpenGL context hâlâ aktifken
        }

        entities_.clear(); // ardından shared_ptr’lar yok edilir

        imguiLayer_.shutdown();

        if (window_)
        {
            glfwDestroyWindow(window_);
            window_ = nullptr;

            glfwTerminate(); // Only once per process; OK here unless you have multiple windows
        }
    }

    void OpenGLViewer::addEntity(const std::shared_ptr<Entity> &e)
    {
        entities_.push_back(e);
    }

    void OpenGLViewer::setEntities(SharedVec<Entity> e)
    {
        entities_ = e;
    }
}
