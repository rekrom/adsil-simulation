#include <viewer/implementations/GroundRenderable.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

namespace viewer
{

    GroundRenderable::~GroundRenderable()
    {
        // cleanup();
    }

    void GroundRenderable::initGL()
    {
        createShader();
        createBuffers();
    }

    void GroundRenderable::createShader()
    {
        const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * vec4(aPos, 1.0);
        }
    )";

        const char *fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(0.5, 0.5, 0.5, 1.0); // Light gray
        }
    )";

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        shader_ = glCreateProgram();
        glAttachShader(shader_, vertexShader);
        glAttachShader(shader_, fragmentShader);
        glLinkProgram(shader_);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void GroundRenderable::createBuffers()
    {
        // Creates a simple XZ grid from -10 to 10
        std::vector<float> vertices;
        const int N = 20; // from -10 to 10
        for (int i = -N; i <= N; ++i)
        {
            // Lines parallel to Z axis
            vertices.push_back((float)i);
            vertices.push_back(0.0f);
            vertices.push_back((float)-N);
            vertices.push_back((float)i);
            vertices.push_back(0.0f);
            vertices.push_back((float)N);

            // Lines parallel to X axis
            vertices.push_back((float)-N);
            vertices.push_back(0.0f);
            vertices.push_back((float)i);
            vertices.push_back((float)N);
            vertices.push_back(0.0f);
            vertices.push_back((float)i);
        }

        vertexCount_ = vertices.size() / 3;

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void GroundRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        glUseProgram(shader_);

        GLint viewLoc = glGetUniformLocation(shader_, "view");
        GLint projLoc = glGetUniformLocation(shader_, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertexCount_));
        glBindVertexArray(0);
    }

    void GroundRenderable::cleanup()
    {
        if (vbo_)
            glDeleteBuffers(1, &vbo_);
        if (vao_)
            glDeleteVertexArrays(1, &vao_);
        if (shader_)
            glDeleteProgram(shader_);

        vbo_ = 0;
        vao_ = 0;
        shader_ = 0;
    }

    glm::vec3 GroundRenderable::getCenter() const
    {
        return transformNode_->getGlobalTransform().getPosition().toGlmVec3();
    }
}
