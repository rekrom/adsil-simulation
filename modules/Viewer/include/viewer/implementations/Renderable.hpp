#pragma once

#include <viewer/interfaces/IRenderable.hpp>
#include <glad/glad.h>

namespace viewer
{

    class Renderable : public IRenderable
    {
    public:
        Renderable();
        virtual ~Renderable();

        float getAlpha() const;
        void setAlpha(float alpha);
        bool isTransparent() const;
        virtual glm::vec3 getCenter() const = 0;

    protected:
        float alpha_;
        GLuint vao_ = 0;
        GLuint vbo_ = 0;
        GLuint ebo_ = 0;
        GLuint shader_ = 0;

    protected:
        virtual void createBuffers(); // optionally overridden
        virtual void createShader();  // optionally overridden
    };

} // namespace viewer
