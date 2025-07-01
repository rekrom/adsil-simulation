#pragma once
#include <viewer/interfaces/Renderable.hpp>
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
        void createBuffers();
        void createShader();

        GLuint vao_{0}, vbo_{0}, shader_{0};
        int vertexCount_{0};
    };

}
