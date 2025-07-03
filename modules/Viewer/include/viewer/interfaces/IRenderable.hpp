#pragma once
#include <memory>
#include <glm/glm.hpp>

namespace viewer
{
    /*  Any drawable object owns two phases:
        • initGL()  : called once, after an OpenGL context exists
        • render()  : called every frame                                           */
    class IRenderable
    {
    public:
        virtual ~IRenderable() = default;
        virtual void initGL() = 0; // create VAO / VBO / shaders
        virtual void render(const glm::mat4 &view, const glm::mat4 &projection) = 0;
        virtual void cleanup() = 0;
    };

}
