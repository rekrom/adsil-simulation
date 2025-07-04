#pragma once

#include "imgui.h"
#include <viewer/interfaces/Camera.hpp>

#include <viewer/common/RenderingTypes.hpp> // instead of defining the enum

namespace viewer::imgui
{
    class ViewerInspectorPanel
    {
    public:
        void draw(Camera &camera, RenderingMode &mode, int &fps);

    private:
        bool showCameraSettings_ = true;
        bool showRenderingSettings_ = true;
        bool showFPSSettings_ = true;

        void drawCameraSection(Camera &camera);
        void drawRenderingModeSection(RenderingMode &mode);
        void drawFPS(int &fps);
    };
}
