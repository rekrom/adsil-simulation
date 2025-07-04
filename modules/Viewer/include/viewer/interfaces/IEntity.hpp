#pragma once

#include <glm/glm.hpp>
#include <string>

class IEntity
{
public:
    virtual ~IEntity() = default;

    virtual void initGL() = 0;
    virtual void render(const glm::mat4 &view, const glm::mat4 &projection) = 0;
    virtual void cleanup() = 0;

    virtual bool isVisible() const = 0;
    virtual void setVisible(bool visible) = 0;

    virtual void update(float deltaTime) {} // default no-op

    virtual glm::vec3 getColor() const { return glm::vec3(1.0f); } // default white

    virtual bool isTransparent() const = 0;

    virtual glm::vec3 getCenter() const = 0;

    // Optional: for debug, scene selection, etc.
    virtual std::string getName() const = 0;
};