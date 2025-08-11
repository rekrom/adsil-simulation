#include <viewer/renderables/FovPyramidRenderable.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>
#include <math/RotationUtils.hpp>

namespace viewer
{

    FoVPyramidRenderable::FoVPyramidRenderable(std::shared_ptr<Device> device,
                                               glm::vec3 color,
                                               float alpha)
        : device_(std::move(device))
    {
        // Default color: blue for TX, red for RX (can be overridden)

        setColor(color);
        setAlpha(alpha);
    }

    FoVPyramidRenderable::~FoVPyramidRenderable()
    {
        // cleanup();
    }

    void FoVPyramidRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void FoVPyramidRenderable::createShader()
    {
        shader_ = shader::ShaderUtils::createProgramFromFiles("fov_pyramid");

        uniforms_.model = glGetUniformLocation(shader_, "model");
        uniforms_.view = glGetUniformLocation(shader_, "view");
        uniforms_.projection = glGetUniformLocation(shader_, "projection");
        uniforms_.color = glGetUniformLocation(shader_, "color");
        uniforms_.alpha = glGetUniformLocation(shader_, "alpha");
    }

    void FoVPyramidRenderable::createBuffers()
    {
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 18, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void FoVPyramidRenderable::updateVertices()
    {
        float fovH = device_->getHorizontalFovRad();
        float fovV = device_->getVerticalFovRad();
        float range = device_->getRange();

        float halfW = range * tanf(fovH / 2.0F);
        float halfH = range * tanf(fovV / 2.0F);

        glm::vec3 apex(0.0F, 0.0F, 0.0F);
        glm::vec3 v1(range, -halfW, halfH);
        glm::vec3 v2(range, halfW, halfH);
        glm::vec3 v3(range, halfW, -halfH);
        glm::vec3 v4(range, -halfW, -halfH);

        std::vector<glm::vec3> triangleVertices = {
            apex, v1, v2, // front face
            apex, v2, v3, // right face
            apex, v3, v4, // back face
            apex, v4, v1, // left face

            // base (as two triangles)
            v1, v2, v3,
            v3, v4, v1};

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            static_cast<GLsizeiptr>(sizeof(glm::vec3) * triangleVertices.size()),
            triangleVertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void FoVPyramidRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!device_)
            return;

        if (dirty_)
        {
            updateVertices();
            dirty_ = false;
        }

        glm::mat4 model = device_->getGlobalTransform().getModelMatrix();

        glBindVertexArray(vao_);
        glUseProgram(shader_);

        glUniformMatrix4fv(uniforms_.model, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniforms_.projection, 1, GL_FALSE, glm::value_ptr(projection));

        // --- Wireframe pass (black) ---
        glm::vec3 wireColor(0.0F, 0.0F, 0.0F); // black
        glUniform3fv(uniforms_.color, 1, glm::value_ptr(wireColor));
        glUniform1f(uniforms_.alpha, 0.85f);

        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0F, -1.0F);

        glLineWidth(1.5f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, 18); // same triangles, just outlined

        glDisable(GL_POLYGON_OFFSET_LINE);

        // --- Solid faces pass (red) ---
        glUniform3fv(uniforms_.color, 1, glm::value_ptr(color_)); // red
        glUniform1f(uniforms_.alpha, alpha_);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void FoVPyramidRenderable::cleanup()
    {
        if (vbo_)
            glDeleteBuffers(1, &vbo_);
        if (vao_)
            glDeleteVertexArrays(1, &vao_);
        if (shader_)
            glDeleteProgram(shader_);
        vbo_ = vao_ = shader_ = 0;
    }
    glm::vec3 FoVPyramidRenderable::getCenter() const
    {
        return device_->getGlobalTransform().getPosition().toGlmVec3();
    }
}
