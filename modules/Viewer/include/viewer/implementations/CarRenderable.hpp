#pragma once

#include <viewer/interfaces/Renderable.hpp>
#include <core/Car.hpp>
#include <viewer/implementations/DeviceRenderable.hpp>

#include <memory>
#include <vector>
#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>

namespace viewer
{

    class CarRenderable : public Renderable
    {
    public:
        explicit CarRenderable(std::shared_ptr<Car> car);
        ~CarRenderable() override;

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

    private:
        std::shared_ptr<Car> car_;

        GLuint cubeVAO_{0}, cubeVBO_{0};
        GLuint shader_{0};

        glm::vec3 carColor_ = glm::vec3(0.2f, 0.6f, 0.9f); // example blue-ish color

        std::vector<std::unique_ptr<DeviceRenderable>> txRenderables_;
        std::vector<std::unique_ptr<DeviceRenderable>> rxRenderables_;

        void createBuffers();
        void createShader();

        void renderDevices(const glm::mat4 &view, const glm::mat4 &projection);
    };

} // namespace viewer
