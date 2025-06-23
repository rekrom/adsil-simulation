#include <adapter/VectorJsonAdapter.hpp>
#include <cassert>
#include <iostream>

using namespace adapter;

void test_VectorJsonAdapter()
{
    Vector v(1.1f, 2.2f, 3.3f);
    VectorJsonAdapter adapter;

    nlohmann::json j = adapter.toJson(v);
    assert(j["x"] == 1.1f);
    assert(j["y"] == 2.2f);
    assert(j["z"] == 3.3f);

    Vector v2 = adapter.fromJson(j);
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
