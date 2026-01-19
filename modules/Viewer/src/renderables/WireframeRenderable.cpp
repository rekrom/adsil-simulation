#include <viewer/renderables/WireframeRenderable.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace viewer
{

    WireframeRenderable::WireframeRenderable(const std::vector<math::Point> &lines, glm::vec3 color)
        : lines_(lines), color_(color) {}

    WireframeRenderable::~WireframeRenderable() = default;  // RAII handles cleanup!

    void WireframeRenderable::cleanup()
    {
        // RAII: Simply reset the optionals
        vao_.reset();
        vbo_.reset();
        shader_.reset();
    }

    void WireframeRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void WireframeRenderable::createShader()
    {
        shader_.emplace(shader::ShaderUtils::createProgramFromFiles("wireframe"));
    }

    void WireframeRenderable::createBuffers()
    {
        std::vector<float> vertices;
        for (const auto &pt : lines_)
        {
            vertices.push_back(pt.x());
            vertices.push_back(pt.y());
            vertices.push_back(pt.z());
        }

        vao_.emplace();
        vbo_.emplace();

        vao_->bind();
        vbo_->bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        gl::VertexArray::unbind();
    }

    void WireframeRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!shader_ || !vao_)
            return;

        shader_->use();

        glm::mat4 model = glm::mat4(1.0F);
        glUniformMatrix4fv(shader_->getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(shader_->getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shader_->getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(shader_->getUniformLocation("uColor"), 1, glm::value_ptr(color_));

        vao_->bind();
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines_.size()));
        gl::VertexArray::unbind();
    }

    glm::vec3 WireframeRenderable::getCenter() const
    {
        glm::vec3 center(0, 0, 0);
        return center;
    }

}
