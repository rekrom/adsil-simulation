#include "Viewer/AxisRenderable.hpp"
#include <iostream>

/* ---------- GLSL sources ------------------------------------------------ */
static const char *vSrc = R"(
    #version 330 core
    layout(location=0) in vec3 aPos;
    layout(location=1) in vec3 aCol;

    uniform mat4 view;
    uniform mat4 projection;

    out vec3 vCol;

    void main() {
        gl_Position = projection * view * vec4(aPos, 1.0);
        vCol = aCol;
    }
)";

static const char *fSrc = R"(
    #version 330 core
    in vec3 vCol;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vCol, 1.0);
    }
)";
/* ------------------------------------------------------------------------ */

AxisRenderable::~AxisRenderable()
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
}

void AxisRenderable::createShader()
{
    auto compile = [](GLenum type, const char *src) -> GLuint
    {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        int ok;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            char log[512];
            glGetShaderInfoLog(s, 512, nullptr, log);
            std::cerr << "Shader compile error:\n"
                      << log << '\n';
        }
        return s;
    };
    GLuint vs = compile(GL_VERTEX_SHADER, vSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fSrc);

    shader_ = glCreateProgram();
    glAttachShader(shader_, vs);
    glAttachShader(shader_, fs);
    glLinkProgram(shader_);
    glDeleteShader(vs);
    glDeleteShader(fs);
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

    glBindVertexArray(vao_);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
}