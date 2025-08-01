#pragma once

#include <optional>
#include <cmath>
#include <math/math.hpp>
#include <core/Logger.hpp>

namespace math
{
    namespace helper
    {

        inline std::optional<Point> intersectLinePlane(
            const Point &plane_point,
            const Vector &plane_normal,
            const Point &line_origin,
            const Vector &line_direction)
        {
            constexpr float EPSILON = 1e-6f;

            Vector normalized_normal = plane_normal.normalized();
            float denom = normalized_normal.dot(line_direction);

            if (std::abs(denom) < EPSILON)
            {
                // Line is parallel to the plane
                LOGGER_INFO("Line is parallel to the plane, no intersection");
                return std::nullopt;
            }

            Vector diff = line_origin.toVectorFrom(plane_point);
            float t = -normalized_normal.dot(diff) / denom;
            Point intersection = line_origin + line_direction * t;

            return intersection;
        }

        inline bool isPointInConvexQuad(const Point &P0, const Point &P1, const Point &P2,
                                        const Point &P3, const Point &P4)
        {
            // Compute consistent plane normal using two edges
            Vector normal = P2.toVectorFrom(P1).cross(P4.toVectorFrom(P1));

            auto edgeTest = [&](const Point &A, const Point &B) -> double
            {
                Vector edge = B.toVectorFrom(A);
                Vector toP0 = P0.toVectorFrom(A);
                return edge.cross(toP0).dot(normal);
            };

            double d1 = edgeTest(P1, P2);
            double d2 = edgeTest(P2, P3);
            double d3 = edgeTest(P3, P4);
            double d4 = edgeTest(P4, P1);
            // Check if all signs are the same
            bool allPositive = (d1 > 0 && d2 > 0 && d3 > 0 && d4 > 0);
            bool allNegative = (d1 < 0 && d2 < 0 && d3 < 0 && d4 < 0);
            return allPositive || allNegative;
        }
    }
}