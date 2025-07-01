#include <viewer/implementations/CarRenderable.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace viewer
{

    CarRenderable::CarRenderable(std::shared_ptr<Car> car)
        : car_(std::move(car))
    {
    }

    CarRenderable::~CarRenderable()
    {
        cleanup();
    }

    void CarRenderable::cleanup()
    {
        if (cubeVBO_)
        {
            glDeleteBuffers(1, &cubeVBO_);
            cubeVBO_ = 0;
        }
        if (cubeVAO_)
        {
            glDeleteVertexArrays(1, &cubeVAO_);
            cubeVAO_ = 0;
        }
        if (shader_)
        {
            glDeleteProgram(shader_);
            shader_ = 0;
        }

        for (auto &deviceRenderable : txRenderables_)
        {
            deviceRenderable->cleanup();
        }
        for (auto &deviceRenderable : rxRenderables_)
        {
            deviceRenderable->cleanup();
        }
        txRenderables_.clear();
        rxRenderables_.clear();
    }
    void CarRenderable::initGL()
    {
        createShader();
        createBuffers();

        // Create DeviceRenderable for each device in the car

        txRenderables_.clear();
        rxRenderables_.clear();

        for (const auto &device : car_->getTransmitters())
        {
            txRenderables_.push_back(std::make_unique<DeviceRenderable>(device, glm::vec3(0.0f, 0.0f, 1.0f)));
            txRenderables_.back()->initGL();
        }
        for (const auto &device : car_->getReceivers())
        {
            rxRenderables_.push_back(std::make_unique<DeviceRenderable>(device, glm::vec3(1.0f, 0.0f, 0.0f)));
            rxRenderables_.back()->initGL();
        }
    }

    void CarRenderable::createBuffers()
    {
        float r = carColor_.r;
        float g = carColor_.g;
        float b = carColor_.b;

        float cubeVertices[] = {
            // Front face
            -0.5f, -0.25f, -1.0f, r, g, b,
            0.5f, -0.25f, -1.0f, r, g, b,
            0.5f, 0.25f, -1.0f, r, g, b,
            0.5f, 0.25f, -1.0f, r, g, b,
            -0.5f, 0.25f, -1.0f, r, g, b,
            -0.5f, -0.25f, -1.0f, r, g, b,

            // Back face
            -0.5f, -0.25f, 1.0f, r, g, b,
            0.5f, -0.25f, 1.0f, r, g, b,
            0.5f, 0.25f, 1.0f, r, g, b,
            0.5f, 0.25f, 1.0f, r, g, b,
            -0.5f, 0.25f, 1.0f, r, g, b,
            -0.5f, -0.25f, 1.0f, r, g, b,

            // Left face
            -0.5f, 0.25f, 1.0f, r, g, b,
            -0.5f, 0.25f, -1.0f, r, g, b,
            -0.5f, -0.25f, -1.0f, r, g, b,
            -0.5f, -0.25f, -1.0f, r, g, b,
            -0.5f, -0.25f, 1.0f, r, g, b,
            -0.5f, 0.25f, 1.0f, r, g, b,

            // Right face
            0.5f, 0.25f, 1.0f, r, g, b,
            0.5f, 0.25f, -1.0f, r, g, b,
            0.5f, -0.25f, -1.0f, r, g, b,
            0.5f, -0.25f, -1.0f, r, g, b,
            0.5f, -0.25f, 1.0f, r, g, b,
            0.5f, 0.25f, 1.0f, r, g, b,

            // Top face
            -0.5f, 0.25f, -1.0f, r, g, b,
            0.5f, 0.25f, -1.0f, r, g, b,
            0.5f, 0.25f, 1.0f, r, g, b,
            0.5f, 0.25f, 1.0f, r, g, b,
            -0.5f, 0.25f, 1.0f, r, g, b,
            -0.5f, 0.25f, -1.0f, r, g, b,

            // Bottom face
            -0.5f, -0.25f, -1.0f, r, g, b,
            0.5f, -0.25f, -1.0f, r, g, b,
            0.5f, -0.25f, 1.0f, r, g, b,
            0.5f, -0.25f, 1.0f, r, g, b,
            -0.5f, -0.25f, 1.0f, r, g, b,
            -0.5f, -0.25f, -1.0f, r, g, b};

        glGenVertexArrays(1, &cubeVAO_);
        glGenBuffers(1, &cubeVBO_);

        glBindVertexArray(cubeVAO_);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void CarRenderable::createShader()
    {
        const char *vSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        out vec3 vColor;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            vColor = aColor;
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

        const char *fSrc = R"(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vColor, 1.0);
        }
    )";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vSrc, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fSrc, nullptr);
        glCompileShader(fs);

        shader_ = glCreateProgram();
        glAttachShader(shader_, vs);
        glAttachShader(shader_, fs);
        glLinkProgram(shader_);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void CarRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!car_)
            return;

        // Calculate model matrix from car's transform
        glm::mat4 model = car_->getGlobalTransform().getModelMatrix();

        glUseProgram(shader_);

        glUniformMatrix4fv(glGetUniformLocation(shader_, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(cubeVAO_);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Render all devices on the car
        renderDevices(view, projection);

        glUseProgram(0);
    }

    void CarRenderable::renderDevices(const glm::mat4 &view, const glm::mat4 &projection)
    {
        for (const auto &deviceRenderable : txRenderables_)
        {
            deviceRenderable->render(view, projection);
        }

        for (const auto &deviceRenderable : rxRenderables_)
        {
            deviceRenderable->render(view, projection);
        }
    }

} // namespace viewer
