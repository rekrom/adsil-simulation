#pragma once

#include <core/Car.hpp>
#include <viewer/implementations/Renderable.hpp>
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

        std::shared_ptr<Car> getCar() const;

    private:
        std::shared_ptr<Car> car_;

        glm::vec3 carColor_ = glm::vec3(0.2f, 0.6f, 0.9f); // example blue-ish color

        std::vector<std::unique_ptr<DeviceRenderable>> txRenderables_;
        std::vector<std::unique_ptr<DeviceRenderable>> rxRenderables_;
        void renderDevices(const glm::mat4 &view, const glm::mat4 &projection);

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

} // namespace viewer
