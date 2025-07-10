#pragma once

#include <viewer/interfaces/IRenderable.hpp>
#include <glad/glad.h>
#include <viewer/shaders/ShaderUtils.hpp>
#include <core/ResourceLocator.hpp>

namespace viewer
{

    class Renderable : public IRenderable
    {
        struct StandardUniformLocations
        {
            GLint model = -1;
            GLint view = -1;
            GLint projection = -1;
            GLint alpha = -1;

            GLint color = -1;
            GLint useUniformColor = -1;
            GLint uniformColor = -1;
        };

    public:
        Renderable();
        virtual ~Renderable();

        bool getVisible() const;
        void setVisible(bool isVisible);

        float getAlpha() const;
        void setAlpha(float alpha);

        glm::vec3 getColor() const;
        void setColor(glm::vec3 color);

        bool isTransparent() const;              // todo: deprectaed
        virtual glm::vec3 getCenter() const = 0; // todo: deprecated

    protected:
        float alpha_{1.0F};
        GLuint vao_ = 0;
        GLuint vbo_ = 0;
        GLuint ebo_ = 0;
        GLuint shader_ = 0;

        StandardUniformLocations uniforms_;
        glm::vec3 color_{0.5F, 0.5F, 0.5F};
        bool isVisible_{true};

    protected:
        virtual void createBuffers(); // optionally overridden
        virtual void createShader();  // optionally overridden
    };

} // namespace viewer
