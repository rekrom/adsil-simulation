#include <adapter/JsonAdapterRegistry.hpp>
#include <adapter/implementations/PointJsonAdapter.hpp>
#include <math/Point.hpp>
#include <cassert>
#include <iostream>

using namespace adapter;

void test_JsonAdapterRegistry_with_Point()
{
    JsonAdapterRegistry registry;

    // Adapter kaydı
    registry.registerAdapter<math::Point>(std::make_shared<PointJsonAdapter>());

    // Orijinal Point objesi
    math::Point p1(10.0F, 20.0F, 30.0F);

    // Adapter aracılığıyla JSON'a çevir
    auto *adapter = registry.getAdapter<math::Point>();
    assert(adapter != nullptr);

    nlohmann::json j = adapter->toJson(p1);

    // JSON'dan geri objeyi oluştur
    math::Point p2 = adapter->fromJson(j);

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
