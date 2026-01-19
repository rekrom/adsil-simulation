#include <viewer/renderables/AxisRenderable.hpp>
#include <iostream>

namespace viewer
{
    AxisRenderable::~AxisRenderable() = default; // RAII handles cleanup!

    void AxisRenderable::cleanup()
    {
        // RAII: Simply reset the optionals - destructors handle OpenGL cleanup
        shader_.reset();
        vbo_.reset();
        vao_.reset();
    }

    void AxisRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void AxisRenderable::createShader()
    {
        shader_.emplace(shader::ShaderUtils::createProgramFromFiles("axis"));
    }

    void AxisRenderable::createBuffers()
    {
        const float data[] = {
            // pos                 // colour
            0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, //  X
            0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, //  Y
            0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1  //  Z
        };

        vao_.emplace();
        vbo_.emplace();

        vao_->bind();
        vbo_->bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        gl::VertexArray::unbind();
    }

    void AxisRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!shader_ || !vao_)
        {
            return;
        }

        shader_->use();

        // Upload view and projection matrices
        GLint viewLoc = shader_->getUniformLocation("view");
        GLint projLoc = shader_->getUniformLocation("projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
        glLineWidth(2.5F);

        vao_->bind();
        glDrawArrays(GL_LINES, 0, 6);
        gl::VertexArray::unbind();
    }

    glm::vec3 AxisRenderable::getCenter() const
    {
        glm::vec3 center(0, 0, 0);
        return center;
    }

}
