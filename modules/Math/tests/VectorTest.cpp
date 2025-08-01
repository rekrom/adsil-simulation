#include <math/Vector.hpp>
#include <math/Constants.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

using namespace math;
using namespace math::constants;

void test_Vector_construction()
{
    // Default constructor
    Vector v1;
    assert(v1.x() == 0.0F);
    assert(v1.y() == 0.0F);
    assert(v1.z() == 0.0F);

    // Parameterized constructor
    Vector v2(1.0F, 2.0F, 3.0F);
    assert(v2.x() == 1.0F);
    assert(v2.y() == 2.0F);
    assert(v2.z() == 3.0F);

    std::cout << "[PASS] Vector construction test\n";
}

void test_Vector_magnitude()
{
    Vector v1(3.0F, 4.0F, 0.0F);
    assert(std::abs(v1.magnitude() - 5.0F) < EPSILON_F);

    Vector v2(1.0F, 1.0F, 1.0F);
    assert(std::abs(v2.magnitude() - std::sqrt(3.0F)) < EPSILON_F);

    Vector v3(0.0F, 0.0F, 0.0F);
    assert(v3.magnitude() == 0.0F);

    std::cout << "[PASS] Vector magnitude test\n";
}

void test_Vector_normalization()
{
    Vector v1(3.0F, 4.0F, 0.0F);
    Vector normalized = v1.normalized();
    assert(std::abs(normalized.magnitude() - 1.0F) < EPSILON_F);
    assert(std::abs(normalized.x() - 0.6F) < EPSILON_F);
    assert(std::abs(normalized.y() - 0.8F) < EPSILON_F);
    assert(normalized.z() == 0.0F);

    // Zero vector normalization
    Vector v2(0.0F, 0.0F, 0.0F);
    Vector normalized_zero = v2.normalized();
    assert(normalized_zero.x() == 0.0F);
    assert(normalized_zero.y() == 0.0F);
    assert(normalized_zero.z() == 0.0F);

    std::cout << "[PASS] Vector normalization test\n";
}

void test_Vector_dot_product()
{
    Vector v1(1.0F, 2.0F, 3.0F);
    Vector v2(4.0F, 5.0F, 6.0F);

    float dot_result = v1.dot(v2);
    // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    assert(std::abs(dot_result - 32.0F) < EPSILON_F);

    // Orthogonal vectors
    Vector v3(1.0F, 0.0F, 0.0F);
    Vector v4(0.0F, 1.0F, 0.0F);
    assert(std::abs(v3.dot(v4)) < EPSILON_F);

    std::cout << "[PASS] Vector dot product test\n";
}

void test_Vector_cross_product()
{
    Vector v1(1.0F, 0.0F, 0.0F);
    Vector v2(0.0F, 1.0F, 0.0F);
    Vector cross_result = v1.cross(v2);

    assert(std::abs(cross_result.x()) < EPSILON_F);
    assert(std::abs(cross_result.y()) < EPSILON_F);
    assert(std::abs(cross_result.z() - 1.0F) < EPSILON_F);

    // Cross product with itself should be zero
    Vector v3(1.0F, 2.0F, 3.0F);
    Vector self_cross = v3.cross(v3);
    assert(std::abs(self_cross.x()) < EPSILON_F);
    assert(std::abs(self_cross.y()) < EPSILON_F);
    assert(std::abs(self_cross.z()) < EPSILON_F);

    std::cout << "[PASS] Vector cross product test\n";
}

void test_Vector_arithmetic_operations()
{
    Vector v1(1.0F, 2.0F, 3.0F);
    Vector v2(4.0F, 5.0F, 6.0F);

    // Addition
    Vector sum = v1 + v2;
    assert(sum.x() == 5.0F);
    assert(sum.y() == 7.0F);
    assert(sum.z() == 9.0F);

    // Subtraction
    Vector diff = v2 - v1;
    assert(diff.x() == 3.0F);
    assert(diff.y() == 3.0F);
    assert(diff.z() == 3.0F);

    // Scalar multiplication
    Vector scaled = v1 * 2.0F;
    assert(scaled.x() == 2.0F);
    assert(scaled.y() == 4.0F);
    assert(scaled.z() == 6.0F);

    // Compound assignment
    Vector v3 = v1;
    v3 += v2;
    assert(v3.x() == 5.0F);
    assert(v3.y() == 7.0F);
    assert(v3.z() == 9.0F);

    Vector v4 = v2;
    v4 -= v1;
    assert(v4.x() == 3.0F);
    assert(v4.y() == 3.0F);
    assert(v4.z() == 3.0F);

    std::cout << "[PASS] Vector arithmetic operations test\n";
}

void test_Vector_glm_conversion()
{
    Vector v(1.0F, 2.0F, 3.0F);
    glm::vec3 glm_vec = v.toGlmVec3();

    assert(std::abs(glm_vec.x - 1.0F) < EPSILON_F);
    assert(std::abs(glm_vec.y - 2.0F) < EPSILON_F);
    assert(std::abs(glm_vec.z - 3.0F) < EPSILON_F);

    std::cout << "[PASS] Vector GLM conversion test\n";
}

void test_Vector_toString()
{
    Vector v(1.5F, 2.5F, 3.5F);
    std::string str = v.toString();

    // Check that string contains the values (basic check)
    assert(str.find("1.5") != std::string::npos);
    assert(str.find("2.5") != std::string::npos);
    assert(str.find("3.5") != std::string::npos);
    assert(str.find("Vector") != std::string::npos);

    std::cout << "[PASS] Vector toString test\n";
}

int main()
{
    test_Vector_construction();
    test_Vector_magnitude();
    test_Vector_normalization();
    test_Vector_dot_product();
    test_Vector_cross_product();
    test_Vector_arithmetic_operations();
    test_Vector_glm_conversion();
    test_Vector_toString();

    std::cout << "\n=== All Vector tests passed! ===\n";
    return 0;
}
