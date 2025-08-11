#include <math/RotationUtils.hpp>
#include <math/Constants.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

using namespace math;
using namespace math::constants;
using namespace math::RotationUtils;

void test_deg2rad_conversion()
{
    // Test common angle conversions
    assert(std::abs(deg2rad(0.0F) - 0.0F) < EPSILON_F);
    assert(std::abs(deg2rad(90.0F) - HALF_PI_F) < EPSILON_F);
    assert(std::abs(deg2rad(180.0F) - PI_F) < EPSILON_F);
    assert(std::abs(deg2rad(360.0F) - TWO_PI_F) < EPSILON_F);

    // Test with double precision
    assert(std::abs(deg2rad(45.0) - (PI_D / 4.0)) < EPSILON_D);

    std::cout << "[PASS] Degree to radian conversion test\n";
}

void test_rad2deg_conversion()
{
    // Test common angle conversions
    assert(std::abs(rad2deg(0.0F) - 0.0F) < EPSILON_F);
    assert(std::abs(rad2deg(HALF_PI_F) - 90.0F) < EPSILON_F);
    assert(std::abs(rad2deg(PI_F) - 180.0F) < EPSILON_F);
    assert(std::abs(rad2deg(TWO_PI_F) - 360.0F) < EPSILON_F);

    // Test with double precision
    assert(std::abs(rad2deg(PI_D / 4.0) - 45.0) < EPSILON_D);

    std::cout << "[PASS] Radian to degree conversion test\n";
}

void test_conversion_round_trip()
{
    // Test that deg->rad->deg gives back original value
    float deg_values[] = {0.0F, 30.0F, 45.0F, 90.0F, 180.0F, 270.0F, 360.0F};

    for (float deg : deg_values)
    {
        float converted = rad2deg(deg2rad(deg));
        assert(std::abs(converted - deg) < EPSILON_F);
    }

    // Test that rad->deg->rad gives back original value
    float rad_values[] = {0.0F, PI_F / 6.0F, PI_F / 4.0F, HALF_PI_F, PI_F, 3.0F * PI_F / 2.0F, TWO_PI_F};

    for (float rad : rad_values)
    {
        float converted = deg2rad(rad2deg(rad));
        assert(std::abs(converted - rad) < EPSILON_F);
    }

    std::cout << "[PASS] Conversion round trip test\n";
}

void test_rotateRPY_identity()
{
    // Rotation by zero should return the same vector
    Vector vec(1.0F, 2.0F, 3.0F);
    Vector zero_rotation(0.0F, 0.0F, 0.0F);

    Vector result = rotateRPY(vec, zero_rotation);

    assert(std::abs(result.x() - vec.x()) < EPSILON_F);
    assert(std::abs(result.y() - vec.y()) < EPSILON_F);
    assert(std::abs(result.z() - vec.z()) < EPSILON_F);

    std::cout << "[PASS] RotateRPY identity test\n";
}

void test_rotateRPY_single_axis()
{
    // Test rotation around each axis individually
    Vector unit_x(1.0F, 0.0F, 0.0F);
    Vector unit_y(0.0F, 1.0F, 0.0F);
    Vector unit_z(0.0F, 0.0F, 1.0F);

    // Rotate unit_y by 90 degrees around Z (yaw)
    Vector yaw_90(0.0F, 0.0F, HALF_PI_F);
    Vector rotated_y = rotateRPY(unit_y, yaw_90);

    // Should become approximately (-1, 0, 0)
    assert(std::abs(rotated_y.x() - (-1.0F)) < EPSILON_F);
    assert(std::abs(rotated_y.y() - 0.0F) < EPSILON_F);
    assert(std::abs(rotated_y.z() - 0.0F) < EPSILON_F);

    // Rotate unit_x by 90 degrees around Y (pitch)
    Vector pitch_90(0.0F, HALF_PI_F, 0.0F);
    Vector rotated_x = rotateRPY(unit_x, pitch_90);

    // Should become approximately (0, 0, -1)
    assert(std::abs(rotated_x.x() - 0.0F) < EPSILON_F);
    assert(std::abs(rotated_x.y() - 0.0F) < EPSILON_F);
    assert(std::abs(rotated_x.z() - (-1.0F)) < EPSILON_F);

    // Rotate unit_z by 90 degrees around X (roll)
    Vector roll_90(HALF_PI_F, 0.0F, 0.0F);
    Vector rotated_z = rotateRPY(unit_z, roll_90);

    // Should become approximately (0, -1, 0)
    assert(std::abs(rotated_z.x() - 0.0F) < EPSILON_F);
    assert(std::abs(rotated_z.y() - (-1.0F)) < EPSILON_F);
    assert(std::abs(rotated_z.z() - 0.0F) < EPSILON_F);

    std::cout << "[PASS] RotateRPY single axis test\n";
}

void test_rotateRPY_magnitude_preservation()
{
    // Rotation should preserve vector magnitude
    Vector vec(3.0F, 4.0F, 5.0F);
    Vector rotation(0.5F, 1.0F, 1.5F);

    float original_magnitude = vec.magnitude();
    Vector rotated = rotateRPY(vec, rotation);
    float rotated_magnitude = rotated.magnitude();

    assert(std::abs(original_magnitude - rotated_magnitude) < EPSILON_F);

    std::cout << "[PASS] RotateRPY magnitude preservation test\n";
}

void test_eulerFromDirection_basic()
{
    // Test with basic cardinal directions

    // Forward direction (positive Z)
    Vector forward(0.0F, 0.0F, 1.0F);
    Vector euler_forward = eulerFromDirection(forward);

    // Should have zero yaw and pitch
    assert(std::abs(euler_forward.x()) < EPSILON_F); // roll
    assert(std::abs(euler_forward.y()) < EPSILON_F); // pitch
    assert(std::abs(euler_forward.z()) < EPSILON_F); // yaw

    // Right direction (positive X)
    Vector right(1.0F, 0.0F, 0.0F);
    Vector euler_right = eulerFromDirection(right);

    // Should have 90 degree yaw
    assert(std::abs(euler_right.x()) < EPSILON_F);             // roll
    assert(std::abs(euler_right.y()) < EPSILON_F);             // pitch
    assert(std::abs(euler_right.z() - HALF_PI_F) < EPSILON_F); // yaw

    // Up direction (positive Y)
    Vector up(0.0F, 1.0F, 0.0F);
    Vector euler_up = eulerFromDirection(up);

    // Should have -90 degree pitch
    assert(std::abs(euler_up.x()) < EPSILON_F);                // roll
    assert(std::abs(euler_up.y() - (-HALF_PI_F)) < EPSILON_F); // pitch
    // Yaw is undefined for straight up, could be any value

    std::cout << "[PASS] EulerFromDirection basic test\n";
}

void test_eulerFromDirection_consistency()
{
    // Test that magnitude doesn't affect the result
    Vector dir1(1.0F, 0.0F, 0.0F);
    Vector dir2(2.0F, 0.0F, 0.0F); // Same direction, different magnitude

    Vector euler1 = eulerFromDirection(dir1);
    Vector euler2 = eulerFromDirection(dir2);

    assert(std::abs(euler1.x() - euler2.x()) < EPSILON_F);
    assert(std::abs(euler1.y() - euler2.y()) < EPSILON_F);
    assert(std::abs(euler1.z() - euler2.z()) < EPSILON_F);

    std::cout << "[PASS] EulerFromDirection consistency test\n";
}

void test_eulerToRotationMatrix_identity()
{
    // Zero rotation should give identity matrix
    Vector zero_rotation(0.0F, 0.0F, 0.0F);
    glm::mat4 matrix = eulerToRotationMatrix(zero_rotation);

    // Check diagonal elements are 1
    assert(std::abs(matrix[0][0] - 1.0F) < EPSILON_F);
    assert(std::abs(matrix[1][1] - 1.0F) < EPSILON_F);
    assert(std::abs(matrix[2][2] - 1.0F) < EPSILON_F);
    assert(std::abs(matrix[3][3] - 1.0F) < EPSILON_F);

    // Check off-diagonal elements in upper-left 3x3 are close to 0
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (i != j)
            {
                assert(std::abs(matrix[i][j]) < EPSILON_F);
            }
        }
    }

    std::cout << "[PASS] EulerToRotationMatrix identity test\n";
}

void test_rotateAroundAxis_identity()
{
    Vector v(1.0F, 2.0F, 3.0F);
    Vector axis(0.0F, 1.0F, 0.0F);
    float angle = 0.0F;
    Vector rotated = rotateAroundAxis(v, axis, angle);
    assert(std::abs(rotated.x() - v.x()) < EPSILON_F);
    assert(std::abs(rotated.y() - v.y()) < EPSILON_F);
    assert(std::abs(rotated.z() - v.z()) < EPSILON_F);
    std::cout << "[PASS] rotateAroundAxis identity test\n";
}

void test_rotateAroundAxis_axis_invariant()
{
    // Rotating the axis vector about itself should yield the same vector
    Vector axis(0.0F, 0.0F, 1.0F);
    Vector rotated = rotateAroundAxis(axis, axis, HALF_PI_F);
    assert(std::abs(rotated.x() - axis.x()) < EPSILON_F);
    assert(std::abs(rotated.y() - axis.y()) < EPSILON_F);
    assert(std::abs(rotated.z() - axis.z()) < EPSILON_F);
    std::cout << "[PASS] rotateAroundAxis axis invariant test\n";
}

void test_rotateAroundAxis_quarter_turn_z()
{
    // (1,0,0) rotated 90° about +Z becomes (0,1,0)
    Vector v(1.0F, 0.0F, 0.0F);
    Vector axis(0.0F, 0.0F, 1.0F);
    Vector rotated = rotateAroundAxis(v, axis, HALF_PI_F);
    assert(std::abs(rotated.x() - 0.0F) < EPSILON_F);
    assert(std::abs(rotated.y() - 1.0F) < EPSILON_F);
    assert(std::abs(rotated.z() - 0.0F) < EPSILON_F);
    std::cout << "[PASS] rotateAroundAxis quarter turn Z test\n";
}

void test_rotateAroundAxis_quarter_turn_x()
{
    // (0,1,0) rotated 90° about +X becomes (0,0,1)
    Vector v(0.0F, 1.0F, 0.0F);
    Vector axis(1.0F, 0.0F, 0.0F);
    Vector rotated = rotateAroundAxis(v, axis, HALF_PI_F);
    assert(std::abs(rotated.x() - 0.0F) < EPSILON_F);
    assert(std::abs(rotated.y() - 0.0F) < EPSILON_F);
    assert(std::abs(rotated.z() - 1.0F) < EPSILON_F);
    std::cout << "[PASS] rotateAroundAxis quarter turn X test\n";
}

void test_rotateAroundAxis_full_rotation()
{
    // 360° rotation should return original vector
    Vector v(0.3F, -1.2F, 4.5F);
    Vector axis(2.0F, 3.0F, 4.0F); // intentionally not unit length
    Vector rotated = rotateAroundAxis(v, axis, TWO_PI_F);
    assert(std::abs(rotated.x() - v.x()) < 1e-5F);
    assert(std::abs(rotated.y() - v.y()) < 1e-5F);
    assert(std::abs(rotated.z() - v.z()) < 1e-5F);
    std::cout << "[PASS] rotateAroundAxis full rotation test\n";
}

void test_rotateAroundAxis_magnitude_preserved()
{
    Vector v(2.0F, -3.5F, 1.25F);
    Vector axis(0.0F, 1.0F, 1.0F);
    float angle = 1.2345F;
    float mag_before = v.magnitude();
    Vector rotated = rotateAroundAxis(v, axis, angle);
    float mag_after = rotated.magnitude();
    assert(std::abs(mag_before - mag_after) < 1e-5F);
    std::cout << "[PASS] rotateAroundAxis magnitude preservation test\n";
}

int main()
{
    test_deg2rad_conversion();
    test_rad2deg_conversion();
    test_conversion_round_trip();
    test_rotateRPY_identity();
    test_rotateRPY_single_axis();
    test_rotateRPY_magnitude_preservation();
    test_eulerFromDirection_basic();
    test_eulerFromDirection_consistency();
    test_eulerToRotationMatrix_identity();
    test_rotateAroundAxis_identity();
    test_rotateAroundAxis_axis_invariant();
    test_rotateAroundAxis_quarter_turn_z();
    test_rotateAroundAxis_quarter_turn_x();
    test_rotateAroundAxis_full_rotation();
    test_rotateAroundAxis_magnitude_preserved();

    std::cout << "\n=== All RotationUtils tests passed! ===\n";
    return 0;
}
