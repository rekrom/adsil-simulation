#include <adapter/implementations/PointJsonAdapter.hpp>
#include <cassert>
#include <iostream>

using namespace adapter;

void test_PointJsonAdapter()
{
    Point p(1.0F, 2.0F, 3.0F);
    PointJsonAdapter adapter;

    nlohmann::json j = adapter.toJson(p);

    assert(j["x"] == 1.0F);
    assert(j["y"] == 2.0F);
    assert(j["z"] == 3.0F);

    Point p2 = adapter.fromJson(j);

    assert(p2.x() == p.x());
    assert(p2.y() == p.y());
    assert(p2.z() == p.z());

    std::cout << "[PASS] PointJsonAdapter basic test\n";
}

int main()
{
    test_PointJsonAdapter();
    return 0;
}
