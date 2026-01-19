#pragma once
#include <core/Logger.hpp>

#include <viewer/renderables/Renderable.hpp>

#include <viewer/renderables/FovPyramidRenderable.hpp>
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
        std::vector<std::shared_ptr<Renderable>> getSubRenderables() const override;

        void enableFoV(bool enable);

        [[nodiscard]] glm::vec3 getFovPyramidColor() const;
        void setFovPyramidColor(glm::vec3 color);

    private:
        std::shared_ptr<Device> device_;

        // RAII-managed additional OpenGL resources for arrow
        std::optional<gl::VertexArray> arrowVAO_;
        std::optional<gl::Buffer> arrowVBO_;

        bool showFoV_ = true;
        std::shared_ptr<FoVPyramidRenderable> fovRenderable_;

    protected:
        void createShader() override;
        void createBuffers() override;
        glm::vec3 getCenter() const override;
    };

}
