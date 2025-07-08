#include <viewer/implementations/ShapeRenderable.hpp>
#include <core/PointCloud.hpp>
#include <glad/glad.h>
#include <iostream>

namespace viewer
{

    ShapeRenderable::ShapeRenderable(std::shared_ptr<ShapeBase> shape, const glm::vec3 &color)
        : shape_(std::move(shape)), color_(color)
    {
    }

    ShapeRenderable::~ShapeRenderable()
    {
        // cleanup();
    }

    void ShapeRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void ShapeRenderable::createShader()
    {
        const char *vSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        })";

        const char *fSrc = R"(
        #version 330 core
        uniform vec3 uniformColor;
        uniform float alpha;

        out vec4 FragColor;

        void main() {
            FragColor = vec4(uniformColor, alpha);
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

        uniforms_.model = glGetUniformLocation(shader_, "model");
        uniforms_.view = glGetUniformLocation(shader_, "view");
        uniforms_.projection = glGetUniformLocation(shader_, "projection");
        uniforms_.alpha = glGetUniformLocation(shader_, "alpha");
        uniforms_.color = glGetUniformLocation(shader_, "color");
        uniforms_.useUniformColor = glGetUniformLocation(shader_, "useUniformColor");
        uniforms_.uniformColor = glGetUniformLocation(shader_, "uniformColor");
    }

    void ShapeRenderable::createBuffers()
    {
        if (!shape_)
            return;

        auto pointCloud = shape_->surfaceMesh(2048); // or wireframe()
        const auto &points = pointCloud->getPoints();
        vertexCount_ = points.size();

        std::vector<float> vertices;
        vertices.reserve(vertexCount_ * 3);
        for (const auto &p : points)
        {
            vertices.push_back(p.x());
            vertices.push_back(p.y());
            vertices.push_back(p.z());
        }

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        std::cout << "[ShapeRenderable] Initialized with " << vertexCount_ << " vertices." << std::endl;
    }

    void ShapeRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (vao_ == 0 || shader_ == 0)
            return;

        glUseProgram(shader_);

        // Set uniforms
        glm::mat4 model = glm::translate(glm::mat4(1.0f), getCenter()); // center as position
        glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(uniforms_.projection, 1, GL_FALSE, &projection[0][0]);

        glUniform1f(uniforms_.alpha, alpha_);
        glUniform1i(uniforms_.useUniformColor, true);
        glUniform3fv(uniforms_.uniformColor, 1, &color_[0]);

        glBindVertexArray(vao_);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertexCount_));
        glBindVertexArray(0);
    }

    void ShapeRenderable::cleanup()
    {
        if (vao_)
            glDeleteVertexArrays(1, &vao_);
        if (vbo_)
            glDeleteBuffers(1, &vbo_);
        vao_ = 0;
        vbo_ = 0;
        vertexCount_ = 0;
    }

    glm::vec3 ShapeRenderable::getCenter() const
    {
        if (!shape_)
            return glm::vec3(0.0f);

        auto origin = shape_->getOrigin();
        return glm::vec3(origin.x(), origin.y(), origin.z());
    }

}
