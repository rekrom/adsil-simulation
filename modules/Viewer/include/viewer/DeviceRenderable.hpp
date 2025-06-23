#pragma once

#include <viewer/Renderable.hpp>
#include <device/Device.hpp>
#include <glad/glad.h>
#include <memory>
#include <glm/gtx/quaternion.hpp> // rotation için gerekli

class DeviceRenderable : public Renderable
{
public:
    explicit DeviceRenderable(std::shared_ptr<Device> device);
    explicit DeviceRenderable(std::shared_ptr<Device> device, glm::vec3 color);

    ~DeviceRenderable();

    void initGL() override;
    void render(const glm::mat4 &view, const glm::mat4 &projection) override;

private:
    void createBuffers();
    void createShader();

    std::shared_ptr<Device> device_;
    GLuint cubeVAO_{0}, cubeVBO_{0};
    GLuint arrowVAO_{0}, arrowVBO_{0};
    GLuint shader_{0};
    glm::vec3 color_ = glm::vec3(1.0f); // varsayılan beyaz renk
};
