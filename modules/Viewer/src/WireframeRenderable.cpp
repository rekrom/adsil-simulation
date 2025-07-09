#include <viewer/implementations/WireframeRenderable.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace viewer
{

    WireframeRenderable::WireframeRenderable(const std::vector<Point> &lines, glm::vec3 color)
        : lines_(lines), color_(color) {}

    WireframeRenderable::~WireframeRenderable()
    {
        // cleanup();
    }

    void WireframeRenderable::cleanup()
    {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteProgram(shader_);
    }

    void WireframeRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void WireframeRenderable::createShader()
    {
        // std::cout << "[WireframeRenderable] Compiling shaders..." << std::endl;

        shader_ = shader::ShaderUtils::createProgramFromFiles("wireframe");
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

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void WireframeRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        glUseProgram(shader_);

        glm::mat4 model = glm::mat4(1.0F);
        glUniformMatrix4fv(glGetUniformLocation(shader_, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(shader_, "uColor"), 1, glm::value_ptr(color_));

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines_.size()));
        glBindVertexArray(0);
    }

    glm::vec3 WireframeRenderable::getCenter() const
    {
        glm::vec3 center(0, 0, 0);
        return center;
    }

}
