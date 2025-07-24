#pragma once
#include <core/Logger.hpp>
#include <viewer/renderables/Renderable.hpp>
#include <glad/glad.h>
#include <spatial/implementations/HasTransform.hpp>

namespace viewer
{

    class GroundRenderable : public Renderable
    {
    public:
        GroundRenderable() = default;
        ~GroundRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;
        glm::vec3 getCenter() const override;

    private:
        std::shared_ptr<spatial::HasTransform> transformNode_;
        size_t vertexCount_{0};

    protected:
        void createShader() override;
        void createBuffers() override;
    };

}
