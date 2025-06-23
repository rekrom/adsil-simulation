#pragma once
#include <viewer/Renderable.hpp>
#include <glad/glad.h>

class AxisRenderable : public Renderable
{
public:
    AxisRenderable() = default;
    ~AxisRenderable();

    void initGL() override; // creates buffers & shader
    void render(const glm::mat4 &view, const glm::mat4 &projection) override;

private:
    void createBuffers();
    void createShader();

    GLuint vao_{0}, vbo_{0}, shader_{0};
};
