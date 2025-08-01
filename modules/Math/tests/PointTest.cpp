#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <math/Constants.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

using namespace math;
using namespace math::constants;

void test_Point_construction()
{
    // Default constructor
    Point p1;
    assert(p1.x() == 0.0F);
    assert(p1.y() == 0.0F);
    assert(p1.z() == 0.0F);

    // Parameterized constructor
    Point p2(1.0F, 2.0F, 3.0F);
    assert(p2.x() == 1.0F);
    assert(p2.y() == 2.0F);
    assert(p2.z() == 3.0F);

    std::cout << "[PASS] Point construction test\n";
}

void test_Point_distance()
{
    Point p1(0.0F, 0.0F, 0.0F);
    Point p2(3.0F, 4.0F, 0.0F);

    float distance = p1.distanceTo(p2);
    assert(std::abs(distance - 5.0F) < EPSILON_F);

    // Distance to self should be zero
    assert(p1.distanceTo(p1) == 0.0F);

    // 3D distance test
    Point p3(1.0F, 1.0F, 1.0F);
    Point p4(2.0F, 2.0F, 2.0F);
    float distance_3d = p3.distanceTo(p4);
    assert(std::abs(distance_3d - std::sqrt(3.0F)) < EPSILON_F);

    std::cout << "[PASS] Point distance test\n";
}

void test_Point_toVectorFrom()
{
    Point origin(0.0F, 0.0F, 0.0F);
    Point target(3.0F, 4.0F, 5.0F);

    Vector v = target.toVectorFrom(origin);
    assert(v.x() == 3.0F);
    assert(v.y() == 4.0F);
    assert(v.z() == 5.0F);

    // Non-zero origin
    Point origin2(1.0F, 1.0F, 1.0F);
    Point target2(4.0F, 5.0F, 6.0F);
    Vector v2 = target2.toVectorFrom(origin2);
    assert(v2.x() == 3.0F);
    assert(v2.y() == 4.0F);
    assert(v2.z() == 5.0F);

    std::cout << "[PASS] Point toVectorFrom test\n";
}

void test_Point_vector_operations()
{
    Point p(1.0F, 2.0F, 3.0F);
    Vector v(4.0F, 5.0F, 6.0F);

    // Point + Vector
    Point p1 = p + v;
    assert(p1.x() == 5.0F);
    assert(p1.y() == 7.0F);
    assert(p1.z() == 9.0F);

    // Point - Vector
    Point p2 = p - v;
    assert(p2.x() == -3.0F);
    assert(p2.y() == -3.0F);
    assert(p2.z() == -3.0F);

    std::cout << "[PASS] Point vector operations test\n";
}

void test_Point_point_operations()
{
    Point p1(1.0F, 2.0F, 3.0F);
    Point p2(4.0F, 5.0F, 6.0F);

    // Point + Point
    Point sum = p1 + p2;
    assert(sum.x() == 5.0F);
    assert(sum.y() == 7.0F);
    assert(sum.z() == 9.0F);

    // Point - Point
    Point diff = p2 - p1;
    assert(diff.x() == 3.0F);
    assert(diff.y() == 3.0F);
    assert(diff.z() == 3.0F);

    std::cout << "[PASS] Point point operations test\n";
}

void test_Point_glm_conversion()
{
    Point p(1.0F, 2.0F, 3.0F);
    glm::vec3 glm_vec = p.toGlmVec3();

    assert(std::abs(glm_vec.x - 1.0F) < EPSILON_F);
    assert(std::abs(glm_vec.y - 2.0F) < EPSILON_F);
    assert(std::abs(glm_vec.z - 3.0F) < EPSILON_F);

    std::cout << "[PASS] Point GLM conversion test\n";
}

void test_Point_toString()
{
    Point p(1.5F, 2.5F, 3.5F);
    std::string str = p.toString();

    // Check that string contains the values (basic check)
    assert(str.find("1.5") != std::string::npos);
    assert(str.find("2.5") != std::string::npos);
    assert(str.find("3.5") != std::string::npos);
    assert(str.find("Point") != std::string::npos);

    std::cout << "[PASS] Point toString test\n";
}

void test_Point_Vector_consistency()
{
    // Test that Point-Vector operations are consistent
    Point p1(1.0F, 2.0F, 3.0F);
    Point p2(4.0F, 6.0F, 8.0F);

    Vector v = p2.toVectorFrom(p1);
    Point reconstructed = p1 + v;

    assert(std::abs(reconstructed.x() - p2.x()) < EPSILON_F);
    assert(std::abs(reconstructed.y() - p2.y()) < EPSILON_F);
    assert(std::abs(reconstructed.z() - p2.z()) < EPSILON_F);

    // Test distance consistency
    float distance1 = p1.distanceTo(p2);
    float distance2 = v.magnitude();
    assert(std::abs(distance1 - distance2) < EPSILON_F);

    std::cout << "[PASS] Point-Vector consistency test\n";
}

int main()
{
    test_Point_construction();
    test_Point_distance();
    test_Point_toVectorFrom();
    test_Point_vector_operations();
    test_Point_point_operations();
    test_Point_glm_conversion();
    test_Point_toString();
    test_Point_Vector_consistency();

    std::cout << "\n=== All Point tests passed! ===\n";
    return 0;
}
