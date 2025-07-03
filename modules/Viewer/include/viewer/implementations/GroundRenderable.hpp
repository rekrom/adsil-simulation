#pragma once
#include <viewer/implementations/Renderable.hpp>
#include <glad/glad.h>

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

    private:
        int vertexCount_{0};

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

}
