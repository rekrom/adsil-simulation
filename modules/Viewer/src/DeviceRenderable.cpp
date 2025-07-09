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
        // cleanup();
    }

    void DeviceRenderable::initGL()
    {
        createShader();
        createBuffers();

        if (showFoV_)
        {
            fovRenderable_ = std::make_unique<FoVPyramidRenderable>(device_, 0.25F);
            fovRenderable_->initGL();
        }
        // std::cout << "[DeviceRenderable] initGL done!" << std::endl;
    }

    void DeviceRenderable::createBuffers()
    {
        float r = color_.r;
        float g = color_.g;
        float b = color_.b;

        float cubeVertices[] = {
            // positions          // colors
            -0.5F,
            -0.5F,
            -0.5F,
            r,
            g,
            b,
            0.5F,
            -0.5F,
            -0.5F,
            r,
            g,
            b,
            0.5F,
            0.5F,
            -0.5F,
            r,
            g,
            b,
            -0.5F,
            0.5F,
            -0.5F,
            r,
            g,
            b,
            -0.5F,
            -0.5F,
            0.5F,
            r,
            g,
            b,
            0.5F,
            -0.5F,
            0.5F,
            r,
            g,
            b,
            0.5F,
            0.5F,
            0.5F,
            r,
            g,
            b,
            -0.5F,
            0.5F,
            0.5F,
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

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glGenBuffers(1, &ebo_);

        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
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
            0.F, 0.F, 0.F, 1.F, 0.F, 0.F,
            0.F, 0.F, 0.3F, 1.F, 0.F, 0.F};

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
        // std::cout << "[DeviceRenderable] Compiling shaders..." << std::endl;

        shader_ = shader::ShaderUtils::createProgramFromFiles("device");

        uniforms_.model = glGetUniformLocation(shader_, "model");
        uniforms_.view = glGetUniformLocation(shader_, "view");
        uniforms_.projection = glGetUniformLocation(shader_, "projection");
    }

    void DeviceRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!device_)
            return;

        glm::mat4 model = device_->getGlobalTransform().getModelMatrix();

        model = glm::scale(model, glm::vec3(0.51f)); // <-- Check if this differs!

        glUseProgram(shader_);

        glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniforms_.projection, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw cube with indices
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Draw arrow
        glBindVertexArray(arrowVAO_);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        glUseProgram(0);

        if (showFoV_ && fovRenderable_)
            fovRenderable_->render(view, projection);
    }

    void DeviceRenderable::enableFoV(bool enable)
    {
        showFoV_ = enable;
        if (!enable)
            fovRenderable_.reset();
    }

    void DeviceRenderable::cleanup()
    {
        if (vbo_)
        {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0;
        }
        if (vao_)
        {
            glDeleteVertexArrays(1, &vao_);
            vao_ = 0;
        }
        if (ebo_)
        {
            glDeleteBuffers(1, &ebo_);
            ebo_ = 0;
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
        if (fovRenderable_)
        {
            fovRenderable_->cleanup();
            fovRenderable_.reset();
        }
    }

    glm::vec3 DeviceRenderable::getCenter() const
    {
        return device_->getGlobalTransform().getPosition().toGlmVec3(); // Or however you access position
    }
}
