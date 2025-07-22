#pragma once
#include <memory>

namespace simulation
{
    class Frame;

    class IFrameObserver
    {
    public:
        virtual ~IFrameObserver() = default;
        virtual void onFrameChanged(const std::shared_ptr<Frame> &frame) = 0;
    };
}