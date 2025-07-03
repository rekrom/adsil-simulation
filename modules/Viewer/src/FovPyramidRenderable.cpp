#include <viewer/implementations/FovPyramidRenderable.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>
#include <core/RotationUtils.hpp>

namespace viewer
{

    FoVPyramidRenderable::FoVPyramidRenderable(std::shared_ptr<Device> device,
                                               float fovHoriDeg,
                                               float fovVertDeg,
                                               float range,
                                               float alpha)
        : device_(std::move(device)),
          fovHoriDeg_(fovHoriDeg),
          fovVertDeg_(fovVertDeg),
          range_(range)
    {
        // Default color: blue for TX, red for RX (can be overridden)
        color_ = glm::vec3(1.0f, 0.2f, 0.2f);
        setAlpha(alpha);
    }

    FoVPyramidRenderable::~FoVPyramidRenderable()
    {
        cleanup();
    }

    void FoVPyramidRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void FoVPyramidRenderable::createShader()
    {
        const char *vSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

        const char *fSrc = R"(
        #version 330 core
        uniform vec3 color;
        out vec4 FragColor;
        uniform float alpha;
        void main() {
            FragColor = vec4(color, alpha);
        }
    )";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vSrc, nullptr);
        glCompileShader(vs);
        GLint success;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(vs, 512, nullptr, infoLog);
            std::cerr << "Vertex shader compile error:\n"
                      << infoLog << std::endl;
        }

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fSrc, nullptr);
        glCompileShader(fs);
        glGetProgramiv(shader_, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(shader_, 512, nullptr, infoLog);
            std::cerr << "Shader program link error:\n"
                      << infoLog << std::endl;
        }

        shader_ = glCreateProgram();
        glAttachShader(shader_, vs);
        glAttachShader(shader_, fs);
        glLinkProgram(shader_);

        glDeleteShader(vs);
        glDeleteShader(fs);
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

        updateVertices(); // call at the end now
    }

    void FoVPyramidRenderable::updateVertices()
    {
        float fovH = glm::radians(fovHoriDeg_);
        float fovV = glm::radians(fovVertDeg_);

        float halfW = range_ * tan(fovH / 2.0f);
        float halfH = range_ * tan(fovV / 2.0f);

        glm::vec3 apex(0.0f, 0.0f, 0.0f);
        glm::vec3 v1(-halfW, halfH, range_);
        glm::vec3 v2(halfW, halfH, range_);
        glm::vec3 v3(halfW, -halfH, range_);
        glm::vec3 v4(-halfW, -halfH, range_);

        std::vector<glm::vec3> triangleVertices = {
            apex, v1, v2, // front face
            apex, v2, v3, // right face
            apex, v3, v4, // back face
            apex, v4, v1, // left face

            // base (as two triangles)
            v1, v2, v3,
            v3, v4, v1};

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * triangleVertices.size(), triangleVertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void FoVPyramidRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!device_)
            return;

        updateVertices();

        glm::mat4 model = device_->getGlobalTransform().getModelMatrix();
        model = glm::scale(model, glm::vec3(0.51f)); // <-- Check if this differs!

        glBindVertexArray(vao_);
        glUseProgram(shader_);

        // --- Wireframe pass (black) ---
        glm::vec3 wireColor(0.0f, 0.0f, 0.0f); // black
        glUniform3fv(glGetUniformLocation(shader_, "color"), 1, glm::value_ptr(wireColor));
        glUniform1f(glGetUniformLocation(shader_, "alpha"), 0.85f);

        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);

        glLineWidth(1.5f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, 18); // same triangles, just outlined
        glLineWidth(1.0f);

        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // --- Solid faces pass (red) ---
        glUniformMatrix4fv(glGetUniformLocation(shader_, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3fv(glGetUniformLocation(shader_, "color"), 1, glm::value_ptr(color_)); // red
        glUniform1f(glGetUniformLocation(shader_, "alpha"), alpha_);

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
