#include <viewer/renderables/DeviceRenderable.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <iostream>
#include <math/RotationUtils.hpp>

namespace viewer
{
    DeviceRenderable::DeviceRenderable(std::shared_ptr<Device> device)
        : device_(std::move(device)) {}

    DeviceRenderable::DeviceRenderable(std::shared_ptr<Device> device, glm::vec3 color)
        : device_(std::move(device))
    {
        this->setColor(color);
        fovRenderable_ = std::make_unique<FoVPyramidRenderable>(device_, color);
    }

    DeviceRenderable::~DeviceRenderable() = default;  // RAII handles cleanup!

    void DeviceRenderable::initGL()
    {
        createShader();
        createBuffers();

        fovRenderable_->initGL();
    }

    void DeviceRenderable::createBuffers()
    {
        glm::vec3 color = getColor();
        float r = color.r;
        float g = color.g;
        float b = color.b;

        float cubeVertices[] = {
            // positions          // colors
            -0.5F, -0.5F, -0.5F, r, g, b,
            0.5F, -0.5F, -0.5F, r, g, b,
            0.5F, 0.5F, -0.5F, r, g, b,
            -0.5F, 0.5F, -0.5F, r, g, b,
            -0.5F, -0.5F, 0.5F, r, g, b,
            0.5F, -0.5F, 0.5F, r, g, b,
            0.5F, 0.5F, 0.5F, r, g, b,
            -0.5F, 0.5F, 0.5F, r, g, b,
        };

        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0, // front face
            4, 5, 6, 6, 7, 4, // back face
            4, 7, 3, 3, 0, 4, // left face
            1, 5, 6, 6, 2, 1, // right face
            3, 2, 6, 6, 7, 3, // top face
            0, 1, 5, 5, 4, 0  // bottom face
        };

        vao_.emplace();
        vbo_.emplace();
        ebo_.emplace();

        vao_->bind();

        vbo_->bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        ebo_->bind(GL_ELEMENT_ARRAY_BUFFER);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        gl::VertexArray::unbind();
    }

    void DeviceRenderable::createShader()
    {
        shader_.emplace(shader::ShaderUtils::createProgramFromFiles("device"));

        uniforms_.model = shader_->getUniformLocation("model");
        uniforms_.view = shader_->getUniformLocation("view");
        uniforms_.projection = shader_->getUniformLocation("projection");
    }

    void DeviceRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!device_ || !shader_ || !vao_)
            return;

        glm::mat4 model = device_->getGlobalTransform().getModelMatrix();

        shader_->use();

        glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniforms_.projection, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw cube with indices
        vao_->bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        gl::VertexArray::unbind();

        // Draw arrow if initialized
        if (arrowVAO_)
        {
            arrowVAO_->bind();
            glDrawArrays(GL_LINES, 0, 2);
            gl::VertexArray::unbind();
        }
        glUseProgram(0);
    }

    void DeviceRenderable::enableFoV(bool enable)
    {
        showFoV_ = enable;
        if (!enable)
            fovRenderable_.reset();
    }

    glm::vec3 DeviceRenderable::getFovPyramidColor() const
    {
        return fovRenderable_->getColor();
    }

    void DeviceRenderable::setFovPyramidColor(glm::vec3 color)
    {
        fovRenderable_->setColor(color);
    }

    void DeviceRenderable::cleanup()
    {
        // RAII: Simply reset the optionals - destructors handle OpenGL cleanup
        vbo_.reset();
        vao_.reset();
        ebo_.reset();
        arrowVBO_.reset();
        arrowVAO_.reset();
        shader_.reset();
        
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

    std::vector<std::shared_ptr<Renderable>> DeviceRenderable::getSubRenderables() const
    {
        if (showFoV_ && fovRenderable_)
            return {fovRenderable_};
        return {};
    }
}
