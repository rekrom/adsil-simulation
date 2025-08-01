#pragma once

namespace math
{
    // Mathematical constants used throughout the project
    namespace constants
    {
        // Zero constants
        constexpr float ZERO_F = 0.0F;
        constexpr double ZERO_D = 0.0;

        // Division constants
        constexpr float HALF_DIVISOR_F = 2.0F; // Used for half-angle calculations and geometric computations
        constexpr double HALF_DIVISOR_D = 2.0;

        // Unity constants
        constexpr float ONE_F = 1.0F;
        constexpr double ONE_D = 1.0;

        // Common fractions
        constexpr float QUARTER = 0.25F;
        constexpr float HALF = 0.5F;
        constexpr float THREE_QUARTERS = 0.75F;

        // Tolerance values for floating point comparisons
        constexpr float EPSILON_F = 1e-6F;
        constexpr double EPSILON_D = 1e-12;

        // Mathematical constants
        constexpr float PI_F = 3.14159265358979323846F;
        constexpr double PI_D = 3.14159265358979323846;
        constexpr float TWO_PI_F = 2.0F * PI_F;
        constexpr double TWO_PI_D = 2.0 * PI_D;
        constexpr float HALF_PI_F = PI_F / 2.0F;
        constexpr double HALF_PI_D = PI_D / 2.0;

        // Conversion constants
        constexpr float DEG_TO_RAD_F = PI_F / 180.0F;
        constexpr double DEG_TO_RAD_D = PI_D / 180.0;
        constexpr float RAD_TO_DEG_F = 180.0F / PI_F;
        constexpr double RAD_TO_DEG_D = 180.0 / PI_D;
    }
}
