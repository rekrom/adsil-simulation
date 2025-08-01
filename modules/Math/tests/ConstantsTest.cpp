#include <math/Constants.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

using namespace math::constants;

void test_zero_constants()
{
    assert(ZERO_F == 0.0F);
    assert(ZERO_D == 0.0);

    std::cout << "[PASS] Zero constants test\n";
}

void test_unity_constants()
{
    assert(ONE_F == 1.0F);
    assert(ONE_D == 1.0);

    std::cout << "[PASS] Unity constants test\n";
}

void test_division_constants()
{
    assert(HALF_DIVISOR_F == 2.0F);
    assert(HALF_DIVISOR_D == 2.0);

    std::cout << "[PASS] Division constants test\n";
}

void test_fraction_constants()
{
    assert(QUARTER == 0.25F);
    assert(HALF == 0.5F);
    assert(THREE_QUARTERS == 0.75F);

    std::cout << "[PASS] Fraction constants test\n";
}

void test_epsilon_constants()
{
    assert(EPSILON_F == 1e-6F);
    assert(EPSILON_D == 1e-12);

    // Test that epsilon values are small positive numbers
    assert(EPSILON_F > 0.0F);
    assert(EPSILON_D > 0.0);
    assert(EPSILON_F < 1.0F);
    assert(EPSILON_D < 1.0);

    std::cout << "[PASS] Epsilon constants test\n";
}

void test_pi_constants()
{
    // Test PI values
    assert(std::abs(PI_F - 3.14159265358979323846F) < EPSILON_F);
    assert(std::abs(PI_D - 3.14159265358979323846) < EPSILON_D);

    // Test TWO_PI values
    assert(std::abs(TWO_PI_F - (2.0F * PI_F)) < EPSILON_F);
    assert(std::abs(TWO_PI_D - (2.0 * PI_D)) < EPSILON_D);

    // Test HALF_PI values
    assert(std::abs(HALF_PI_F - (PI_F / 2.0F)) < EPSILON_F);
    assert(std::abs(HALF_PI_D - (PI_D / 2.0)) < EPSILON_D);

    std::cout << "[PASS] PI constants test\n";
}

void test_conversion_constants()
{
    // Test degree to radian conversion
    assert(std::abs(DEG_TO_RAD_F - (PI_F / 180.0F)) < EPSILON_F);
    assert(std::abs(DEG_TO_RAD_D - (PI_D / 180.0)) < EPSILON_D);

    // Test radian to degree conversion
    assert(std::abs(RAD_TO_DEG_F - (180.0F / PI_F)) < EPSILON_F);
    assert(std::abs(RAD_TO_DEG_D - (180.0 / PI_D)) < EPSILON_D);

    // Test that conversion constants are inverses of each other
    assert(std::abs((DEG_TO_RAD_F * RAD_TO_DEG_F) - 1.0F) < EPSILON_F);
    assert(std::abs((DEG_TO_RAD_D * RAD_TO_DEG_D) - 1.0) < EPSILON_D);

    std::cout << "[PASS] Conversion constants test\n";
}

void test_conversion_functionality()
{
    // Test common angle conversions

    // 180 degrees should be PI radians
    float deg_180_to_rad = 180.0F * DEG_TO_RAD_F;
    assert(std::abs(deg_180_to_rad - PI_F) < EPSILON_F);

    // 90 degrees should be PI/2 radians
    float deg_90_to_rad = 90.0F * DEG_TO_RAD_F;
    assert(std::abs(deg_90_to_rad - HALF_PI_F) < EPSILON_F);

    // PI radians should be 180 degrees
    float pi_to_deg = PI_F * RAD_TO_DEG_F;
    assert(std::abs(pi_to_deg - 180.0F) < EPSILON_F);

    // PI/2 radians should be 90 degrees
    float half_pi_to_deg = HALF_PI_F * RAD_TO_DEG_F;
    assert(std::abs(half_pi_to_deg - 90.0F) < EPSILON_F);

    std::cout << "[PASS] Conversion functionality test\n";
}

void test_mathematical_relationships()
{
    // Test that our constants satisfy basic mathematical relationships

    // Test that QUARTER + QUARTER = HALF
    assert(std::abs((QUARTER + QUARTER) - HALF) < EPSILON_F);

    // Test that HALF + HALF = ONE_F
    assert(std::abs((HALF + HALF) - ONE_F) < EPSILON_F);

    // Test that THREE_QUARTERS = HALF + QUARTER
    assert(std::abs(THREE_QUARTERS - (HALF + QUARTER)) < EPSILON_F);

    // Test that TWO_PI = 2 * PI
    assert(std::abs(TWO_PI_F - (2.0F * PI_F)) < EPSILON_F);

    // Test that HALF_PI * 2 = PI
    assert(std::abs((HALF_PI_F * 2.0F) - PI_F) < EPSILON_F);

    std::cout << "[PASS] Mathematical relationships test\n";
}

int main()
{
    test_zero_constants();
    test_unity_constants();
    test_division_constants();
    test_fraction_constants();
    test_epsilon_constants();
    test_pi_constants();
    test_conversion_constants();
    test_conversion_functionality();
    test_mathematical_relationships();

    std::cout << "\n=== All Constants tests passed! ===\n";
    return 0;
}
