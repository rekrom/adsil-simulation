#include <viewer/implementations/DeviceRenderable.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <iostream>
#include <core/RotationUtils.hpp>

namespace viewer
{
    DeviceRenderable::DeviceRenderable(std::shared_ptr<Device> device)
        : device_(std::move(device)) {}

    DeviceRenderable::DeviceRenderable(std::shared_ptr<Device> device, glm::vec3 color)
        : device_(std::move(device)), color_(color)
    {
    }

    DeviceRenderable::~DeviceRenderable()
    {
        glDeleteVertexArrays(1, &cubeVAO_);
        glDeleteBuffers(1, &cubeVBO_);
        glDeleteVertexArrays(1, &arrowVAO_);
        glDeleteBuffers(1, &arrowVBO_);
        glDeleteProgram(shader_);
        cleanup();
    }

    void DeviceRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void DeviceRenderable::createBuffers()
    {
        float r = color_.r;
        float g = color_.g;
        float b = color_.b;

        float cubeVertices[] = {
            // positions          // colors
            -0.5f,
            -0.5f,
            -0.5f,
            r,
            g,
            b,
            0.5f,
            -0.5f,
            -0.5f,
            r,
            g,
            b,
            0.5f,
            0.5f,
            -0.5f,
            r,
            g,
            b,
            -0.5f,
            0.5f,
            -0.5f,
            r,
            g,
            b,
            -0.5f,
            -0.5f,
            0.5f,
            r,
            g,
            b,
            0.5f,
            -0.5f,
            0.5f,
            r,
            g,
            b,
            0.5f,
            0.5f,
            0.5f,
            r,
            g,
            b,
            -0.5f,
            0.5f,
            0.5f,
            r,
            g,
            b,
        };

        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0, // front face
            4, 5, 6, 6, 7, 4, // back face
            4, 7, 3, 3, 0, 4, // left face
            1, 5, 6, 6, 2, 1, // right face
            3, 2, 6, 6, 7, 3, // top face
            0, 1, 5, 5, 4, 0  // bottom face
        };

        glGenVertexArrays(1, &cubeVAO_);
        glGenBuffers(1, &cubeVBO_);
        glGenBuffers(1, &EBO_);

        glBindVertexArray(cubeVAO_);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        // Arrow buffers (unchanged)
        float arrowVerts[] = {
            0.f, 0.f, 0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 0.3f, 1.f, 0.f, 0.f};

        glGenVertexArrays(1, &arrowVAO_);
        glGenBuffers(1, &arrowVBO_);
        glBindVertexArray(arrowVAO_);
        glBindBuffer(GL_ARRAY_BUFFER, arrowVBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(arrowVerts), arrowVerts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void DeviceRenderable::createShader()
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
        })";

        const char *fSrc = R"(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vColor, 1.0);
        })";

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

    void DeviceRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        std::cout << device_->getName() << " [POS] " << device_->getGlobalTransform().getPosition().toString() << std::endl;
        std::cout << device_->getName() << " [ORI] " << device_->getGlobalTransform().getOrientation().toString() << std::endl;
        // glm::mat4 model = device_->getGlobalTransform().getModelMatrix();
        glm::mat4 model(1.0f);
        model = glm::translate(model, device_->getGlobalTransform().getPosition().toGlmVec3());
        Vector orientation_(0, 0, 0);
        Vector dir(device_->getGlobalTransform().getOrientation().x(), device_->getGlobalTransform().getOrientation().y(), device_->getGlobalTransform().getOrientation().z());

        orientation_ = RotationUtils::eulerFromDirection(dir);
        model *= glm::toMat4(orientation_.toGlmQuat());

        model = glm::scale(model, glm::vec3(0.51f)); // <-- Check if this differs!

        glUseProgram(shader_);

        GLint modelLoc = glGetUniformLocation(shader_, "model");
        GLint viewLoc = glGetUniformLocation(shader_, "view");
        GLint projLoc = glGetUniformLocation(shader_, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw cube with indices
        glBindVertexArray(cubeVAO_);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Draw arrow
        glBindVertexArray(arrowVAO_);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void DeviceRenderable::cleanup()
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
        if (EBO_)
        {
            glDeleteBuffers(1, &EBO_);
            EBO_ = 0;
        }
        if (arrowVBO_)
        {
            glDeleteBuffers(1, &arrowVBO_);
            arrowVBO_ = 0;
        }
        if (arrowVAO_)
        {
            glDeleteVertexArrays(1, &arrowVAO_);
            arrowVAO_ = 0;
        }
        if (shader_)
        {
            glDeleteProgram(shader_);
            shader_ = 0;
        }
    }
}
