#include <adapter/JsonAdapterRegistry.hpp>
#include <adapter/PointJsonAdapter.hpp>
#include <core/Point.hpp>
#include <cassert>
#include <iostream>

using namespace adapter;

void test_JsonAdapterRegistry_with_Point()
{
    JsonAdapterRegistry registry;

    // Adapter kaydı
    registry.registerAdapter<Point>(std::make_shared<PointJsonAdapter>());

    // Orijinal Point objesi
    Point p1(10.0f, 20.0f, 30.0f);

    // Adapter aracılığıyla JSON'a çevir
    auto *adapter = registry.getAdapter<Point>();
    assert(adapter != nullptr);

    nlohmann::json j = adapter->toJson(p1);
    std::cout << "Serialized JSON:\n"
              << j.dump(2) << "\n";

    // JSON'dan geri objeyi oluştur
    Point p2 = adapter->fromJson(j);

    // Doğrulama
    assert(p2.x() == p1.x());
    assert(p2.y() == p1.y());
    assert(p2.z() == p1.z());

    std::cout << "[PASS] JsonAdapterRegistry with PointJsonAdapter\n";
}

int main()
{
    test_JsonAdapterRegistry_with_Point();
    return 0;
}
