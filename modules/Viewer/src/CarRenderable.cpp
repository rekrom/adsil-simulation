#include <viewer/implementations/CarRenderable.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace viewer
{

    CarRenderable::CarRenderable(std::shared_ptr<Car> car)
        : car_(std::move(car))
    {
        setAlpha(0.9);
    }

    CarRenderable::~CarRenderable()
    {
        cleanup();
    }

    std::shared_ptr<Car> CarRenderable::getCar() const
    {
        return car_;
    }

    void CarRenderable::cleanup()
    {
        if (vbo_)
        {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0;
        }
        if (vao_)
        {
            glDeleteVertexArrays(1, &vao_);
            vao_ = 0;
        }
        if (shader_)
        {
            glDeleteProgram(shader_);
            shader_ = 0;
        }

        for (auto &deviceRenderable : txRenderables_)
        {
            deviceRenderable->cleanup();
        }
        for (auto &deviceRenderable : rxRenderables_)
        {
            deviceRenderable->cleanup();
        }
        txRenderables_.clear();
        rxRenderables_.clear();
    }
    void CarRenderable::initGL()
    {
        createShader();
        createBuffers();

        // Create DeviceRenderable for each device in the car

        txRenderables_.clear();
        rxRenderables_.clear();

        for (const auto &device : car_->getTransmitters())
        {
            txRenderables_.push_back(std::make_unique<DeviceRenderable>(device, glm::vec3(0.0f, 0.0f, 1.0f)));
            txRenderables_.back()->initGL();
        }
        for (const auto &device : car_->getReceivers())
        {
            rxRenderables_.push_back(std::make_unique<DeviceRenderable>(device, glm::vec3(1.0f, 0.0f, 0.0f)));
            rxRenderables_.back()->initGL();
        }
    }

    void CarRenderable::createBuffers()
    {
        float r = carColor_.r;
        float g = carColor_.g;
        float b = carColor_.b;
        CarDimension dimension = car_->getDimension();
        float length = dimension.length, width = dimension.width, height = dimension.height;

        float cubeVertices[] = {
            // Front face
            -width / 2, -height / 2, -length / 2, r, g, b,
            -width / 2, -height / 2, +length / 2, r, g, b,
            +width / 2, -height / 2, +length / 2, r, g, b,
            +width / 2, -height / 2, +length / 2, r, g, b,
            +width / 2, -height / 2, -length / 2, r, g, b,
            -width / 2, -height / 2, -length / 2, r, g, b,

            // Back face
            -width / 2, height / 2, -length / 2, r, g, b,
            -width / 2, height / 2, +length / 2, r, g, b,
            +width / 2, height / 2, +length / 2, r, g, b,
            +width / 2, height / 2, +length / 2, r, g, b,
            +width / 2, height / 2, -length / 2, r, g, b,
            -width / 2, height / 2, -length / 2, r, g, b,

            // Left face
            +width / 2, +height / 2, -length / 2, r, g, b,
            +width / 2, -height / 2, -length / 2, r, g, b,
            -width / 2, -height / 2, -length / 2, r, g, b,
            -width / 2, -height / 2, -length / 2, r, g, b,
            -width / 2, +height / 2, -length / 2, r, g, b,
            +width / 2, +height / 2, -length / 2, r, g, b,

            // Right face
            +width / 2, +height / 2, length / 2, r, g, b,
            +width / 2, -height / 2, length / 2, r, g, b,
            -width / 2, -height / 2, length / 2, r, g, b,
            -width / 2, -height / 2, length / 2, r, g, b,
            -width / 2, +height / 2, length / 2, r, g, b,
            +width / 2, +height / 2, length / 2, r, g, b,

            // Top face
            width / 2, -height / 2, -length / 2, r, g, b,
            width / 2, -height / 2, +length / 2, r, g, b,
            width / 2, +height / 2, +length / 2, r, g, b,
            width / 2, +height / 2, +length / 2, r, g, b,
            width / 2, +height / 2, -length / 2, r, g, b,
            width / 2, -height / 2, -length / 2, r, g, b,

            // Bottom face
            -width / 2, -height / 2, -length / 2, r, g, b,
            -width / 2, -height / 2, +length / 2, r, g, b,
            -width / 2, +height / 2, +length / 2, r, g, b,
            -width / 2, +height / 2, +length / 2, r, g, b,
            -width / 2, +height / 2, -length / 2, r, g, b,
            -width / 2, -height / 2, -length / 2, r, g, b};

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void CarRenderable::createShader()
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
        }
    )";

        const char *fSrc = R"(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        uniform bool useUniformColor;
        uniform vec3 uniformColor;
        uniform float alpha;
        void main() 
        {
            vec3 finalColor = useUniformColor ? uniformColor : vColor;
            FragColor = vec4(finalColor, alpha);
        }
    )";

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

    void CarRenderable::render(const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (!car_)
            return;

        // Calculate model matrix from car's transform
        glm::mat4 model = car_->getGlobalTransform().getModelMatrix();

        glUseProgram(shader_);

        glUniformMatrix4fv(glGetUniformLocation(shader_, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        bool useUniform = false;                                // Set true for override
        glm::vec3 highlightColor = glm::vec3(1.0f, 1.0f, 0.0f); // yellow

        glUniform1i(glGetUniformLocation(shader_, "useUniformColor"), static_cast<GLint>(useUniform));
        glUniform3fv(glGetUniformLocation(shader_, "uniformColor"), 1, glm::value_ptr(highlightColor));
        glUniform1f(glGetUniformLocation(shader_, "alpha"), alpha_);

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Render all devices on the car
        renderDevices(view, projection);

        glUseProgram(0);
    }

    void CarRenderable::renderDevices(const glm::mat4 &view, const glm::mat4 &projection)
    {
        for (const auto &deviceRenderable : txRenderables_)
        {
            deviceRenderable->render(view, projection);
        }

        for (const auto &deviceRenderable : rxRenderables_)
        {
            deviceRenderable->render(view, projection);
        }
    }

    glm::vec3 CarRenderable::getCenter() const
    {
        return car_->getPosition().toGlmVec3(); // Or however you access position
    }

} // namespace viewer
