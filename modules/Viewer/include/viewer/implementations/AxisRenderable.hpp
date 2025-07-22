#pragma once
#include <core/Logger.hpp>
#include <viewer/implementations/Renderable.hpp>
#include <glad/glad.h>

namespace viewer
{

    class AxisRenderable : public Renderable
    {
    public:
        AxisRenderable() = default;
        ~AxisRenderable();

        void initGL() override; // creates buffers & shader
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;
        glm::vec3 getCenter() const override;

    private:
    protected:
        void createShader() override;
        void createBuffers() override;
    };

}
