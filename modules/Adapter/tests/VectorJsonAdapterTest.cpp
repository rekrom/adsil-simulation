#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <cassert>
#include <iostream>

using namespace adapter;

void test_VectorJsonAdapter()
{
    math::Vector v(1.1F, 2.2F, 3.3F);
    VectorJsonAdapter adapter;

    nlohmann::json j = adapter.toJson(v);
    assert(j["x"] == 1.1F);
    assert(j["y"] == 2.2F);
    assert(j["z"] == 3.3F);

    math::Vector v2 = adapter.fromJson(j);
    assert(v2.x() == v.x());
    assert(v2.y() == v.y());
    assert(v2.z() == v.z());

    std::cout << "[PASS] VectorJsonAdapter basic test\n";
}

int main()
{
    test_VectorJsonAdapter();
    return 0;
}
