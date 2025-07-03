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
                             float fovHoriDeg,
                             float fovVertDeg,
                             float range,
                             float alpha);

        ~FoVPyramidRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

    private:
        void updateVertices();

        std::shared_ptr<Device> device_;
        float fovHoriDeg_;
        float fovVertDeg_;
        float range_;

        glm::vec3 color_; // determined from device type

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };
}
