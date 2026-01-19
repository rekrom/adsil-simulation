#include <viewer/renderables/GroundRenderable.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <viewer/shaders/ShaderUtils.hpp>

namespace viewer
{

    GroundRenderable::~GroundRenderable() = default;  // RAII handles cleanup!

    void GroundRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void GroundRenderable::createShader()
    {
        shader_.emplace(shader::ShaderUtils::createProgramFromFiles("ground"));
    }

    void GroundRenderable::createBuffers()
    {
        // Creates a simple XZ grid from -10 to 10
        std::vector<float> vertices;
        const int N = 20; // from -10 to 10
        float z_offset = -2.0F;
        for (int i = -N; i <= N; ++i)
        {
            // Lines parallel to Z axis
            vertices.push_back((float)i);
            vertices.push_back((float)-N);
            vertices.push_back(z_offset);
            vertices.push_back((float)i);
            vertices.push_back((float)N);
            vertices.push_back(z_offset);

            // Lines parallel to X axis
            vertices.push_back((float)-N);
            vertices.push_back((float)i);
            vertices.push_back(z_offset);
            vertices.push_back((float)N);
            vertices.push_back((float)i);
            vertices.push_back(z_offset);
        }

        vertexCount_ = vertices.size() / 3;

        vao_.emplace();
        vbo_.emplace();
        vao_->bind();

        vbo_->bind(GL_ARRAY_BUFFER);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
            vertices.data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        gl::VertexArray::unbind();
    }

    void GroundRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!shader_ || !vao_)
            return;

        shader_->use();

        GLint viewLoc = shader_->getUniformLocation("view");
        GLint projLoc = shader_->getUniformLocation("projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glLineWidth(1.0F);

        vao_->bind();
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertexCount_));
        gl::VertexArray::unbind();
    }

    void GroundRenderable::cleanup()
    {
        // RAII: Simply reset the optionals
        vbo_.reset();
        vao_.reset();
        shader_.reset();
    }

    glm::vec3 GroundRenderable::getCenter() const
    {
        return transformNode_->getGlobalTransform().getPosition().toGlmVec3();
    }
}
