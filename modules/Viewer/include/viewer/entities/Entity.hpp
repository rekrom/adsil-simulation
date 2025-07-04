#pragma once
#include <viewer/interfaces/IEntity.hpp>

class Entity : public IEntity
{
public:
    virtual ~Entity() = default;

    void setVisible(bool visible) override { visible_ = visible; }
    bool isVisible() const override { return visible_; }

protected:
    bool visible_ = true;
};
