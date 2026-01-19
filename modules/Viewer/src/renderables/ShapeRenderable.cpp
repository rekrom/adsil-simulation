#include <viewer/renderables/ShapeRenderable.hpp>
#include <math/PointCloud.hpp>
#include <glad/glad.h>
#include <iostream>

namespace viewer
{

    ShapeRenderable::ShapeRenderable(std::shared_ptr<ShapeBase> shape, const glm::vec3 &color)
        : shape_(std::move(shape))
    {
        setColor(color);
    }

    ShapeRenderable::~ShapeRenderable() = default; // RAII handles cleanup!

    void ShapeRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void ShapeRenderable::createShader()
    {
        shader_.emplace(shader::ShaderUtils::createProgramFromFiles("shape"));

        uniforms_.model = shader_->getUniformLocation("model");
        uniforms_.view = shader_->getUniformLocation("view");
        uniforms_.projection = shader_->getUniformLocation("projection");
        uniforms_.alpha = shader_->getUniformLocation("alpha");
        uniforms_.color = shader_->getUniformLocation("color");
        uniforms_.useUniformColor = shader_->getUniformLocation("useUniformColor");
        uniforms_.uniformColor = shader_->getUniformLocation("uniformColor");
    }

    void ShapeRenderable::createBuffers()
    {
        if (!shape_)
            return;

        auto pointCloud = shape_->getSurfaceMeshPCD(); // or wireframe()
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

        vao_.emplace();
        vbo_.emplace();

        vao_->bind();
        vbo_->bind(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        gl::VertexArray::unbind();
    }

    void ShapeRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!vao_ || !shader_)
            return;

        shader_->use();

        // Set uniforms
        glm::mat4 model = glm::translate(glm::mat4(1.0F), getCenter()); // center as position
        glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(uniforms_.projection, 1, GL_FALSE, &projection[0][0]);

        glUniform1f(uniforms_.alpha, alpha_);
        glUniform1i(uniforms_.useUniformColor, true);
        glUniform3fv(uniforms_.uniformColor, 1, &getColor()[0]);

        vao_->bind();
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertexCount_));
        gl::VertexArray::unbind();
    }

    void ShapeRenderable::cleanup()
    {
        // RAII: Simply reset the optionals
        vao_.reset();
        vbo_.reset();
        shader_.reset();
        vertexCount_ = 0;
    }

    glm::vec3 ShapeRenderable::getCenter() const
    {
        if (!shape_)
            return glm::vec3(0.0F);

        const auto globalPos = shape_->getTransformNode()->getGlobalTransform().getPosition();
        return {globalPos.x(), globalPos.y(), globalPos.z()};
    }

}
