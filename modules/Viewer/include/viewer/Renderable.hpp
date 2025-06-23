#pragma once
#include <memory>
#include <glm/glm.hpp>

/*  Any drawable object owns two phases:
    • initGL()  : called once, after an OpenGL context exists
    • render()  : called every frame                                           */
class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void initGL() = 0; // create VAO / VBO / shaders
    virtual void render(const glm::mat4 &view, const glm::mat4 &projection) = 0;
};
