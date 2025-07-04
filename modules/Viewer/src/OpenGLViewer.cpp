#include <glad/glad.h> // ✅ En önce

#include <viewer/implementations/OpenGLViewer.hpp>
#include <iostream>

namespace viewer
{
    OpenGLViewer::OpenGLViewer(int width, int height, const std::string &title)
        : width_(width), height_(height), title_(title), window_(nullptr), camera_(
                                                                               glm::vec3(0.0f, 20.0f, 5.0f), // position
                                                                               glm::vec3(0.0f, 1.0f, 0.0f),  // up direction
                                                                               90.0f,                        // yaw (looking toward -Z)
                                                                               -89.0f                        // pitch
                                                                               ),
          lastX_(width / 2.0f), lastY_(height / 2.0f),
          firstMouse_(true), rightMousePressed_(false),
          deltaTime_(0.0f), lastFrame_(0.0f), renderingMode_(viewer::RenderingMode::Perspective)
    {
    }

    OpenGLViewer::~OpenGLViewer()
    {
        entities_.clear();
        cleanup();
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
    }

    void OpenGLViewer::updateDeltaTime()
    {
        float currentFrame = glfwGetTime();
        deltaTime_ = currentFrame - lastFrame_;
        lastFrame_ = currentFrame;
    }
    void OpenGLViewer::processInput(float dt)
    {
        if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
            camera_.processKeyboard('W', dt);
        if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
            camera_.processKeyboard('S', dt);
        if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
            camera_.processKeyboard('A', dt);
        if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
            camera_.processKeyboard('D', dt);
        if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS)
            camera_.processKeyboard('Q', dt);
        if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS)
            camera_.processKeyboard('E', dt);
    }

    void OpenGLViewer::setupCallbacks()
    {
        glfwSetWindowUserPointer(window_, this);

        glfwSetCursorPosCallback(window_, [](GLFWwindow *win, double xpos, double ypos)
                                 {
        auto *viewer = static_cast<OpenGLViewer *>(glfwGetWindowUserPointer(win));
        if (!viewer->rightMousePressed_)
            return;

        if (viewer->firstMouse_)
        {
            viewer->lastX_ = xpos;
            viewer->lastY_ = ypos;
            viewer->firstMouse_ = false;
        }

        float xoffset = xpos - viewer->lastX_;
        float yoffset = viewer->lastY_ - ypos;
        viewer->lastX_ = xpos;
        viewer->lastY_ = ypos;

        viewer->camera_.processMouseMovement(xoffset, yoffset); });

        glfwSetMouseButtonCallback(window_, [](GLFWwindow *win, int button, int action, int)
                                   {
        auto *viewer = static_cast<OpenGLViewer *>(glfwGetWindowUserPointer(win));
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                viewer->rightMousePressed_ = true;
                viewer->firstMouse_ = true;
            }
            else if (action == GLFW_RELEASE)
            {
                viewer->rightMousePressed_ = false;
            }
        } });

        glfwSetScrollCallback(window_, [](GLFWwindow *win, double, double yoffset)
                              {
        auto *viewer = static_cast<OpenGLViewer *>(glfwGetWindowUserPointer(win));
        viewer->camera_.processMouseScroll(yoffset); });
    }

    void OpenGLViewer::setRenderingMode(RenderingMode mode)
    {
        renderingMode_ = mode;
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
            e->initGL();

        while (!glfwWindowShouldClose(window_))
        {

            updateDeltaTime();
            processInput(deltaTime_);

            // Start new ImGui frame
            imguiLayer_.beginFrame();

            // GUI interaction logic
            imguiLayer_.drawViewerPanel(camera_, renderingMode_, displayedFPS_);
            imguiLayer_.drawUI(entities_);
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            render();

            // End and render ImGui
            imguiLayer_.endFrame();
            glfwSwapBuffers(window_);
            glfwPollEvents();
        }
    }

    glm::mat4 OpenGLViewer::getProjectionMatrix() const
    {
        float aspect = static_cast<float>(width_) / height_;
        if (renderingMode_ == RenderingMode::Perspective)
        {
            return glm::perspective(glm::radians(camera_.getFov()), aspect, 0.1f, 1000.0f);
        }
        else
        {
            float orthoScale = camera_.getFov(); // You may rename this
            return glm::ortho(-orthoScale * aspect, orthoScale * aspect,
                              -orthoScale, orthoScale,
                              0.1f, 1000.0f);
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

    void OpenGLViewer::render()
    {
        updateFPSCounter();

        glm::mat4 view = camera_.getViewMatrix();
        glm::mat4 projection = getProjectionMatrix();

        std::vector<std::shared_ptr<Entity>> opaque;
        std::vector<std::shared_ptr<Entity>> transparent;

        for (auto &e : entities_)
        {
            if (e->isTransparent())
                transparent.push_back(e);
            else
                opaque.push_back(e);
        }

        std::sort(transparent.begin(), transparent.end(),
                  [this](const std::shared_ptr<Entity> &a, const std::shared_ptr<Entity> &b)
                  {
                      glm::vec3 camPos = camera_.getPosition();
                      glm::vec3 aPos = a->getCenter(); // You'll implement this
                      glm::vec3 bPos = b->getCenter();
                      return glm::distance(camPos, aPos) > glm::distance(camPos, bPos); // back-to-front
                  });

        for (auto &e : opaque)
            e->render(view, projection);

        glDepthMask(GL_FALSE); // ⛔ prevent depth writes
        for (auto &e : transparent)
            e->render(view, projection);
        glDepthMask(GL_TRUE); // ✅ restore
    }

    void OpenGLViewer::cleanup()
    {
        // 1. renderables_ temizlenmeli
        for (auto &e : entities_)
        {
            e->cleanup(); // OpenGL context hâlâ aktifken
        }

        entities_.clear(); // ardından shared_ptr’lar yok edilir

        imguiLayer_.shutdown();

        if (window_)
        {
            glfwDestroyWindow(window_);
            glfwTerminate();
        }
    }

    void OpenGLViewer::addEntity(const std::shared_ptr<Entity> &e)
    {
        entities_.push_back(e);
    }

}
