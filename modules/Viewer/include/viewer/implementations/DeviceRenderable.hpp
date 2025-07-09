#pragma once

#include <viewer/implementations/Renderable.hpp>

#include <viewer/implementations/FovPyramidRenderable.hpp>
#include <geometry/implementations/Device.hpp>
#include <glad/glad.h>
#include <memory>
#include <glm/gtx/quaternion.hpp> // rotation için gerekli

namespace viewer
{
    class DeviceRenderable : public Renderable
    {
    public:
        explicit DeviceRenderable(std::shared_ptr<Device> device);
        explicit DeviceRenderable(std::shared_ptr<Device> device, glm::vec3 color);

        ~DeviceRenderable();

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;
        void enableFoV(bool enable);

    private:
        std::shared_ptr<Device> device_;

        GLuint arrowVAO_{0}, arrowVBO_{0};
        glm::vec3 color_ = glm::vec3(1.0F); // varsayılan beyaz renk

        bool showFoV_ = true;
        std::unique_ptr<FoVPyramidRenderable> fovRenderable_;

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

}
