#pragma once

#include <viewer/interfaces/IRenderable.hpp>
#include <glad/glad.h>
#include <viewer/shaders/ShaderUtils.hpp>
#include <viewer/gl/GLResources.hpp>
#include <core/ResourceLocator.hpp>
#include <vector>
#include <optional>

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

            GLint pointSize = -1;
        };

    public:
        Renderable();
        virtual ~Renderable();

        float getAlpha() const;
        void setAlpha(float alpha);

        glm::vec3 getColor() const;
        void setColor(glm::vec3 color);

        bool isTransparent() const;
        virtual glm::vec3 getCenter() const = 0;

        // Sub-renderables (default = none)
        virtual std::vector<std::shared_ptr<Renderable>> getSubRenderables() const
        {
            return {};
        }

    protected:
        float alpha_{1.0F};

        // RAII-managed OpenGL resources (optional for deferred initialization)
        std::optional<gl::VertexArray> vao_;
        std::optional<gl::Buffer> vbo_;
        std::optional<gl::Buffer> ebo_;
        std::optional<gl::ShaderProgram> shader_;

        StandardUniformLocations uniforms_;
        glm::vec3 color_{0.5F, 0.5F, 0.5F};

    protected:
        virtual void createBuffers(); // optionally overridden
        virtual void createShader();  // optionally overridden
    };

} // namespace viewer
