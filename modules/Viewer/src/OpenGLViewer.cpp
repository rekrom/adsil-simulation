#include <glad/glad.h> // ✅ En önce

#include <viewer/implementations/OpenGLViewer.hpp>
#include <iostream>

namespace viewer
{
    OpenGLViewer::OpenGLViewer(int width, int height, const std::string &title)
        : width_(width), height_(height), title_(title), window_(nullptr), camera_(
                                                                               glm::vec3(0.0f, 3.0f, 10.0f), // position
                                                                               glm::vec3(0.0f, 1.0f, 0.0f),  // up direction
                                                                               -90.0f,                       // yaw (looking toward -Z)
                                                                               -15.0f                        // pitch
                                                                               ),
          lastX_(width / 2.0f), lastY_(height / 2.0f),
          firstMouse_(true), rightMousePressed_(false),
          deltaTime_(0.0f), lastFrame_(0.0f)
    {
    }

    OpenGLViewer::~OpenGLViewer()
    {
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

        glEnable(GL_DEPTH_TEST);
        setupCallbacks(); // Add this line just before returning
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
        for (auto &r : renderables_)
            r->initGL();

        while (!glfwWindowShouldClose(window_))
        {
            updateDeltaTime();
            processInput(deltaTime_);

            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            render();

            glfwSwapBuffers(window_);
            glfwPollEvents();
        }
    }

    void OpenGLViewer::render()
    {
        glm::mat4 view = camera_.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera_.getFov()),
            static_cast<float>(width_) / height_,
            0.1f, 1000.0f);
        for (auto &r : renderables_)
        {
            r->render(view, projection);
        }
    }

    void OpenGLViewer::cleanup()
    {
        if (window_)
        {
            glfwDestroyWindow(window_);
            glfwTerminate();
        }
    }

    void OpenGLViewer::addRenderable(const std::shared_ptr<Renderable> &r)
    {
        renderables_.push_back(r);
    }
}
