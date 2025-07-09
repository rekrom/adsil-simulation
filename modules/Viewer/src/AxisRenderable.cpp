#include <viewer/implementations/AxisRenderable.hpp>
#include <iostream>

namespace viewer
{
    AxisRenderable::~AxisRenderable()
    {
        // cleanup();
    }

    void AxisRenderable::cleanup()
    {
        if (shader_)
            glDeleteProgram(shader_);
        if (vbo_)
            glDeleteBuffers(1, &vbo_);
        if (vao_)
            glDeleteVertexArrays(1, &vao_);
    }

    void AxisRenderable::initGL()
    {
        createShader();
        createBuffers();
        // std::cout << "[AxisRenderable] initGL done!" << std::endl;
    }

    void AxisRenderable::createShader()
    {
        // std::cout << "[AxisRenderable] Compiling shaders..." << std::endl;

        shader_ = shader::ShaderUtils::createProgramFromFiles("axis");
    }

    void AxisRenderable::createBuffers()
    {
        const float data[] = {
            // pos                 // colour
            0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, //  X
            0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, //  Y
            0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1  //  Z
        };
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void AxisRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        glUseProgram(shader_);

        // Upload view and projection matrices
        GLint viewLoc = glGetUniformLocation(shader_, "view");
        GLint projLoc = glGetUniformLocation(shader_, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
        glLineWidth(2.5F);

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);
    }

    glm::vec3 AxisRenderable::getCenter() const
    {
        glm::vec3 center(0, 0, 0);
        return center;
    }

}
