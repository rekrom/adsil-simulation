#include "Viewer/DeviceRenderable.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <iostream>

DeviceRenderable::DeviceRenderable(std::shared_ptr<Device> device)
    : device_(std::move(device)) {}

DeviceRenderable::DeviceRenderable(std::shared_ptr<Device> device, glm::vec3 color)
    : device_(std::move(device)), color_(color)
{
}

DeviceRenderable::~DeviceRenderable()
{
    glDeleteVertexArrays(1, &cubeVAO_);
    glDeleteBuffers(1, &cubeVBO_);
    glDeleteVertexArrays(1, &arrowVAO_);
    glDeleteBuffers(1, &arrowVBO_);
    glDeleteProgram(shader_);
}

void DeviceRenderable::initGL()
{
    createShader();
    createBuffers();
}

void DeviceRenderable::createBuffers()
{
    float r = color_.r;
    float g = color_.g;
    float b = color_.b;

    float cubeVertices[] = {
        // FRONT
        -0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        -0.05f,
        r,
        g,
        b,
        -0.05f,
        0.05f,
        -0.05f,
        r,
        g,
        b,
        -0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,

        // BACK
        -0.05f,
        -0.05f,
        0.05f,
        r,
        g,
        b,
        0.05f,
        -0.05f,
        0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        -0.05f,
        0.05f,
        r,
        g,
        b,

        // LEFT
        -0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,
        -0.05f,
        0.05f,
        -0.05f,
        r,
        g,
        b,
        -0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        -0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,

        // RIGHT
        0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        0.05f,
        -0.05f,
        0.05f,
        r,
        g,
        b,
        0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,

        // TOP
        -0.05f,
        0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        0.05f,
        -0.05f,
        r,
        g,
        b,

        // BOTTOM
        -0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,
        0.05f,
        -0.05f,
        0.05f,
        r,
        g,
        b,
        0.05f,
        -0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        -0.05f,
        0.05f,
        r,
        g,
        b,
        -0.05f,
        -0.05f,
        -0.05f,
        r,
        g,
        b,
    };

    glGenVertexArrays(1, &cubeVAO_);
    glGenBuffers(1, &cubeVBO_);

    glBindVertexArray(cubeVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Arrow (unchanged)
    float arrowVerts[] = {
        0.f, 0.f, 0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 0.3f, 1.f, 0.f, 0.f};

    glGenVertexArrays(1, &arrowVAO_);
    glGenBuffers(1, &arrowVBO_);
    glBindVertexArray(arrowVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, arrowVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrowVerts), arrowVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void DeviceRenderable::createShader()
{
    const char *vSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        out vec3 vColor;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            vColor = aColor;
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        })";

    const char *fSrc = R"(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vColor, 1.0);
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
}

void DeviceRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), device_->getOrigin().toGlmVec3());

    // Doğrultu vektörünü Z ekseni ile hizala (modeli döndür)
    glm::vec3 from = glm::vec3(0.f, 0.f, 1.f);                          // modelin varsayılan yönü
    glm::vec3 to = glm::normalize(device_->getDirection().toGlmVec3()); // hedef yön
    glm::quat rot = glm::rotation(from, to);
    model *= glm::toMat4(rot);

    glUseProgram(shader_);

    glUniformMatrix4fv(glGetUniformLocation(shader_, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader_, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 36); // 12 triangles for cube
    glBindVertexArray(0);

    glBindVertexArray(arrowVAO_);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}
