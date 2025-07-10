#pragma once

#include <core/Car.hpp>
#include <viewer/implementations/Renderable.hpp>
#include <viewer/implementations/DeviceRenderable.hpp>

#include <memory>
#include <vector>
#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>
#include <tiny_obj_loader.h>
#include <filesystem>

namespace viewer
{
    class CarRenderable : public Renderable
    {
    public:
        explicit CarRenderable(std::shared_ptr<Car> car, glm::vec3 carColor);
        ~CarRenderable() override;

        void initGL() override;
        void render(const glm::mat4 &view, const glm::mat4 &projection) override;
        void cleanup() override;

        std::shared_ptr<Car> getCar() const;
        void rebuildMesh(); // safe public interface to trigger buffer rebuild
        void setVisible(bool visible);
        bool isVisible() const;

    public:
        size_t vertexCount_ = 0;

    private:
        std::shared_ptr<Car> car_;

        bool visible_{true};

        std::vector<std::unique_ptr<DeviceRenderable>> txRenderables_;
        std::vector<std::unique_ptr<DeviceRenderable>> rxRenderables_;
        void renderDevices(const glm::mat4 &view, const glm::mat4 &projection);

        void createBuffers2();

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

} // namespace viewer
