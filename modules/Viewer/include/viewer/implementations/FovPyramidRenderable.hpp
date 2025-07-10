#pragma once
#include <viewer/implementations/Renderable.hpp>
#include <geometry/implementations/Device.hpp>
#include <glad/glad.h>
#include <memory>
#include <glm/glm.hpp>

namespace viewer
{
    class FoVPyramidRenderable : public Renderable
    {
    public:
        FoVPyramidRenderable(std::shared_ptr<Device> device,
                             glm::vec3 color = glm::vec3{0.8F, 0.1F, 0.1F},
                             float alpha = 0.25F);

        ~FoVPyramidRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

    private:
        void updateVertices();

        std::shared_ptr<Device> device_;

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };
}
