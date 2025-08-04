#include <adapter/implementations/PointJsonAdapter.hpp>
#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include <cmath>

using namespace adapter;

// Helper function for floating point comparison
bool floatEqual(float a, float b, float epsilon = 1e-5f)
{
    return std::fabs(a - b) < epsilon;
}

void test_PointJsonAdapter_basic()
{
    math::Point p(1.0F, 2.0F, 3.0F);
    PointJsonAdapter adapter;

    nlohmann::json j = adapter.toJson(p);

    assert(j["x"] == 1.0F);
    assert(j["y"] == 2.0F);
    assert(j["z"] == 3.0F);

    math::Point p2 = adapter.fromJson(j);

    assert(p2.x() == p.x());
    assert(p2.y() == p.y());
    assert(p2.z() == p.z());

    std::cout << "[PASS] PointJsonAdapter basic test\n";
}

void test_VectorJsonAdapter_basic()
{
    math::Vector v(4.0F, 5.0F, 6.0F);
    VectorJsonAdapter adapter;

    nlohmann::json j = adapter.toJson(v);

    assert(j["x"] == 4.0F);
    assert(j["y"] == 5.0F);
    assert(j["z"] == 6.0F);

    math::Vector v2 = adapter.fromJson(j);

    assert(v2.x() == v.x());
    assert(v2.y() == v.y());
    assert(v2.z() == v.z());

    std::cout << "[PASS] VectorJsonAdapter basic test\n";
}

void test_edge_cases_zero_values()
{
    PointJsonAdapter pointAdapter;
    VectorJsonAdapter vectorAdapter;

    // Test zero values
    math::Point zeroPoint(0.0f, 0.0f, 0.0f);
    math::Vector zeroVector(0.0f, 0.0f, 0.0f);

    nlohmann::json pointJson = pointAdapter.toJson(zeroPoint);
    nlohmann::json vectorJson = vectorAdapter.toJson(zeroVector);

    math::Point deserializedPoint = pointAdapter.fromJson(pointJson);
    math::Vector deserializedVector = vectorAdapter.fromJson(vectorJson);

    assert(floatEqual(deserializedPoint.x(), 0.0f));
    assert(floatEqual(deserializedPoint.y(), 0.0f));
    assert(floatEqual(deserializedPoint.z(), 0.0f));

    assert(floatEqual(deserializedVector.x(), 0.0f));
    assert(floatEqual(deserializedVector.y(), 0.0f));
    assert(floatEqual(deserializedVector.z(), 0.0f));

    std::cout << "[PASS] Zero values edge case test\n";
}

void test_edge_cases_negative_values()
{
    PointJsonAdapter pointAdapter;
    VectorJsonAdapter vectorAdapter;

    // Test negative values
    math::Point negPoint(-1.5f, -2.7f, -3.14f);
    math::Vector negVector(-4.0f, -5.5f, -6.28f);

    nlohmann::json pointJson = pointAdapter.toJson(negPoint);
    nlohmann::json vectorJson = vectorAdapter.toJson(negVector);

    math::Point deserializedPoint = pointAdapter.fromJson(pointJson);
    math::Vector deserializedVector = vectorAdapter.fromJson(vectorJson);

    assert(floatEqual(deserializedPoint.x(), -1.5f));
    assert(floatEqual(deserializedPoint.y(), -2.7f));
    assert(floatEqual(deserializedPoint.z(), -3.14f));

    assert(floatEqual(deserializedVector.x(), -4.0f));
    assert(floatEqual(deserializedVector.y(), -5.5f));
    assert(floatEqual(deserializedVector.z(), -6.28f));

    std::cout << "[PASS] Negative values edge case test\n";
}

void test_edge_cases_extreme_values()
{
    PointJsonAdapter pointAdapter;
    VectorJsonAdapter vectorAdapter;

    // Test very large values
    float largeValue = 1e6f;
    math::Point largePoint(largeValue, largeValue, largeValue);
    math::Vector largeVector(largeValue, largeValue, largeValue);

    nlohmann::json pointJson = pointAdapter.toJson(largePoint);
    nlohmann::json vectorJson = vectorAdapter.toJson(largeVector);

    math::Point deserializedPoint = pointAdapter.fromJson(pointJson);
    math::Vector deserializedVector = vectorAdapter.fromJson(vectorJson);

    assert(floatEqual(deserializedPoint.x(), largeValue, 1e1f)); // Larger epsilon for large numbers
    assert(floatEqual(deserializedPoint.y(), largeValue, 1e1f));
    assert(floatEqual(deserializedPoint.z(), largeValue, 1e1f));

    // Test very small values
    float smallValue = 1e-6f;
    math::Point smallPoint(smallValue, smallValue, smallValue);
    math::Vector smallVector(smallValue, smallValue, smallValue);

    pointJson = pointAdapter.toJson(smallPoint);
    vectorJson = vectorAdapter.toJson(smallVector);

    deserializedPoint = pointAdapter.fromJson(pointJson);
    deserializedVector = vectorAdapter.fromJson(vectorJson);

    assert(floatEqual(deserializedPoint.x(), smallValue, 1e-9f));
    assert(floatEqual(deserializedPoint.y(), smallValue, 1e-9f));
    assert(floatEqual(deserializedPoint.z(), smallValue, 1e-9f));

    std::cout << "[PASS] Extreme values edge case test\n";
}

void test_malformed_json_handling()
{
    PointJsonAdapter pointAdapter;
    VectorJsonAdapter vectorAdapter;

    // Test missing fields
    nlohmann::json incompletePointJson = {
        {"x", 1.0f},
        {"y", 2.0f}
        // missing z
    };

    bool exceptionThrown = false;
    try
    {
        math::Point p = pointAdapter.fromJson(incompletePointJson);
    }
    catch (const std::exception &)
    {
        exceptionThrown = true;
    }
    assert(exceptionThrown);

    // Test wrong data types
    nlohmann::json wrongTypeJson = {
        {"x", "not_a_number"},
        {"y", 2.0f},
        {"z", 3.0f}};

    exceptionThrown = false;
    try
    {
        math::Point p = pointAdapter.fromJson(wrongTypeJson);
    }
    catch (const std::exception &)
    {
        exceptionThrown = true;
    }
    assert(exceptionThrown);

    // Test empty JSON
    nlohmann::json emptyJson = {};

    exceptionThrown = false;
    try
    {
        math::Vector v = vectorAdapter.fromJson(emptyJson);
    }
    catch (const std::exception &)
    {
        exceptionThrown = true;
    }
    assert(exceptionThrown);

    std::cout << "[PASS] Malformed JSON handling test\n";
}

void test_precision_preservation()
{
    PointJsonAdapter pointAdapter;
    VectorJsonAdapter vectorAdapter;

    // Test precision with decimal values
    math::Point precisePoint(1.123456789f, 2.987654321f, 3.141592653f);
    math::Vector preciseVector(4.271828182f, 5.555555555f, 6.999999999f);

    nlohmann::json pointJson = pointAdapter.toJson(precisePoint);
    nlohmann::json vectorJson = vectorAdapter.toJson(preciseVector);

    math::Point deserializedPoint = pointAdapter.fromJson(pointJson);
    math::Vector deserializedVector = vectorAdapter.fromJson(vectorJson);

    // Use appropriate epsilon for float precision
    float epsilon = 1e-6f;
    assert(floatEqual(deserializedPoint.x(), precisePoint.x(), epsilon));
    assert(floatEqual(deserializedPoint.y(), precisePoint.y(), epsilon));
    assert(floatEqual(deserializedPoint.z(), precisePoint.z(), epsilon));

    assert(floatEqual(deserializedVector.x(), preciseVector.x(), epsilon));
    assert(floatEqual(deserializedVector.y(), preciseVector.y(), epsilon));
    assert(floatEqual(deserializedVector.z(), preciseVector.z(), epsilon));

    std::cout << "[PASS] Precision preservation test\n";
}

void test_json_structure_validation()
{
    PointJsonAdapter adapter;

    // Test that JSON has correct structure
    math::Point p(1.0f, 2.0f, 3.0f);
    nlohmann::json j = adapter.toJson(p);

    // Verify JSON has exactly the expected keys
    assert(j.size() == 3);
    assert(j.contains("x"));
    assert(j.contains("y"));
    assert(j.contains("z"));

    // Verify values are numbers
    assert(j["x"].is_number());
    assert(j["y"].is_number());
    assert(j["z"].is_number());

    // Verify no extra keys
    for (auto &[key, value] : j.items())
    {
        assert(key == "x" || key == "y" || key == "z");
    }

    std::cout << "[PASS] JSON structure validation test\n";
}

void test_performance_stress()
{
    PointJsonAdapter pointAdapter;
    VectorJsonAdapter vectorAdapter;

    const int iterations = 10000;

    // Stress test with many serialization/deserialization cycles
    for (int i = 0; i < iterations; ++i)
    {
        float val = static_cast<float>(i);
        math::Point p(val, val + 1, val + 2);
        math::Vector v(val + 3, val + 4, val + 5);

        nlohmann::json pJson = pointAdapter.toJson(p);
        nlohmann::json vJson = vectorAdapter.toJson(v);

        math::Point p2 = pointAdapter.fromJson(pJson);
        math::Vector v2 = vectorAdapter.fromJson(vJson);

        // Quick validation every 1000 iterations
        if (i % 1000 == 0)
        {
            assert(floatEqual(p.x(), p2.x()));
            assert(floatEqual(v.x(), v2.x()));
        }
    }

    std::cout << "[PASS] Performance stress test (" << iterations << " iterations)\n";
}

int main()
{
    test_PointJsonAdapter_basic();
    test_VectorJsonAdapter_basic();
    test_edge_cases_zero_values();
    test_edge_cases_negative_values();
    test_edge_cases_extreme_values();
    test_malformed_json_handling();
    test_precision_preservation();
    test_json_structure_validation();
    test_performance_stress();

    std::cout << "[SUCCESS] All enhanced Point/Vector adapter tests passed!\n";
    return 0;
}
