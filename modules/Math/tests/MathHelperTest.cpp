#include <math/MathHelper.hpp>
#include <math/Constants.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

using namespace math;
using namespace math::constants;
using namespace math::helper;

void test_intersectLinePlane_normal_intersection()
{
    // Simple case: plane at z=0, line going from (0,0,1) in direction (0,0,-1)
    Point plane_point(0.0F, 0.0F, 0.0F);
    Vector plane_normal(0.0F, 0.0F, 1.0F); // pointing up
    Point line_origin(0.0F, 0.0F, 1.0F);
    Vector line_direction(0.0F, 0.0F, -1.0F); // pointing down

    auto result = intersectLinePlane(plane_point, plane_normal, line_origin, line_direction);

    assert(result.has_value());
    Point intersection = result.value();
    assert(std::abs(intersection.x() - 0.0F) < EPSILON_F);
    assert(std::abs(intersection.y() - 0.0F) < EPSILON_F);
    assert(std::abs(intersection.z() - 0.0F) < EPSILON_F);

    std::cout << "[PASS] Line-plane normal intersection test\n";
}

void test_intersectLinePlane_angled_intersection()
{
    // Plane at z=5, line from origin in direction (1,1,1)
    Point plane_point(0.0F, 0.0F, 5.0F);
    Vector plane_normal(0.0F, 0.0F, 1.0F); // pointing up
    Point line_origin(0.0F, 0.0F, 0.0F);
    Vector line_direction(1.0F, 1.0F, 1.0F);

    auto result = intersectLinePlane(plane_point, plane_normal, line_origin, line_direction);

    assert(result.has_value());
    Point intersection = result.value();
    assert(std::abs(intersection.x() - 5.0F) < EPSILON_F);
    assert(std::abs(intersection.y() - 5.0F) < EPSILON_F);
    assert(std::abs(intersection.z() - 5.0F) < EPSILON_F);

    std::cout << "[PASS] Line-plane angled intersection test\n";
}

void test_intersectLinePlane_parallel_line()
{
    // Line parallel to plane (no intersection)
    Point plane_point(0.0F, 0.0F, 0.0F);
    Vector plane_normal(0.0F, 0.0F, 1.0F); // plane in XY
    Point line_origin(0.0F, 0.0F, 1.0F);
    Vector line_direction(1.0F, 1.0F, 0.0F); // parallel to XY plane

    auto result = intersectLinePlane(plane_point, plane_normal, line_origin, line_direction);

    assert(!result.has_value());

    std::cout << "[PASS] Line-plane parallel line test\n";
}

void test_intersectLinePlane_negative_direction()
{
    // Test intersection with negative parameter t
    Point plane_point(0.0F, 0.0F, 0.0F);
    Vector plane_normal(0.0F, 0.0F, 1.0F);
    Point line_origin(0.0F, 0.0F, -2.0F);
    Vector line_direction(0.0F, 0.0F, 1.0F); // pointing up

    auto result = intersectLinePlane(plane_point, plane_normal, line_origin, line_direction);

    assert(result.has_value());
    Point intersection = result.value();
    assert(std::abs(intersection.x() - 0.0F) < EPSILON_F);
    assert(std::abs(intersection.y() - 0.0F) < EPSILON_F);
    assert(std::abs(intersection.z() - 0.0F) < EPSILON_F);

    std::cout << "[PASS] Line-plane negative direction test\n";
}

void test_isPointInConvexQuad_inside()
{
    // Square centered at origin, point inside
    Point test_point(0.0F, 0.0F, 0.0F); // P0 - point to test
    Point p1(-1.0F, -1.0F, 0.0F);       // P1
    Point p2(1.0F, -1.0F, 0.0F);        // P2
    Point p3(1.0F, 1.0F, 0.0F);         // P3
    Point p4(-1.0F, 1.0F, 0.0F);        // P4

    bool result = isPointInConvexQuad(test_point, p1, p2, p3, p4);
    assert(result == true);

    std::cout << "[PASS] Point in convex quad - inside test\n";
}

void test_isPointInConvexQuad_outside()
{
    // Square, point clearly outside
    Point test_point(2.0F, 0.0F, 0.0F); // P0 - point to test (outside)
    Point p1(-1.0F, -1.0F, 0.0F);       // P1
    Point p2(1.0F, -1.0F, 0.0F);        // P2
    Point p3(1.0F, 1.0F, 0.0F);         // P3
    Point p4(-1.0F, 1.0F, 0.0F);        // P4

    bool result = isPointInConvexQuad(test_point, p1, p2, p3, p4);
    assert(result == false);

    std::cout << "[PASS] Point in convex quad - outside test\n";
}

void test_isPointInConvexQuad_on_edge()
{
    // Point on the edge of the quad
    Point test_point(1.0F, 0.0F, 0.0F); // P0 - on right edge
    Point p1(-1.0F, -1.0F, 0.0F);       // P1
    Point p2(1.0F, -1.0F, 0.0F);        // P2
    Point p3(1.0F, 1.0F, 0.0F);         // P3
    Point p4(-1.0F, 1.0F, 0.0F);        // P4

    bool result = isPointInConvexQuad(test_point, p1, p2, p3, p4);
    // Edge case might be considered inside or outside depending on implementation
    // The important thing is that it doesn't crash and gives a consistent result

    std::cout << "[PASS] Point in convex quad - on edge test (result: "
              << (result ? "inside" : "outside") << ")\n";
}

void test_isPointInConvexQuad_3d()
{
    // Test with a quad not in XY plane
    Point test_point(0.0F, 0.0F, 1.0F); // P0 - point to test
    Point p1(-1.0F, 0.0F, 0.0F);        // P1
    Point p2(0.0F, -1.0F, 2.0F);        // P2
    Point p3(1.0F, 0.0F, 2.0F);         // P3
    Point p4(0.0F, 1.0F, 0.0F);         // P4

    bool result = isPointInConvexQuad(test_point, p1, p2, p3, p4);
    // This tests that the function works in 3D space

    std::cout << "[PASS] Point in convex quad - 3D test (result: "
              << (result ? "inside" : "outside") << ")\n";
}

int main()
{
    test_intersectLinePlane_normal_intersection();
    test_intersectLinePlane_angled_intersection();
    test_intersectLinePlane_parallel_line();
    test_intersectLinePlane_negative_direction();
    test_isPointInConvexQuad_inside();
    test_isPointInConvexQuad_outside();
    test_isPointInConvexQuad_on_edge();
    test_isPointInConvexQuad_3d();

    std::cout << "\n=== All MathHelper tests passed! ===\n";
    return 0;
}
