#include <math/PointCloud.hpp>
#include <math/Point.hpp>
#include <cassert>
#include <iostream>
#include <vector>

using namespace math;

void test_PointCloud_construction()
{
    // Default constructor
    PointCloud cloud1;
    assert(cloud1.empty());
    assert(cloud1.size() == 0);

    // Constructor with points
    std::vector<Point> points = {
        Point(1.0F, 2.0F, 3.0F),
        Point(4.0F, 5.0F, 6.0F),
        Point(7.0F, 8.0F, 9.0F)};

    PointCloud cloud2(points);
    assert(!cloud2.empty());
    assert(cloud2.size() == 3);

    std::cout << "[PASS] PointCloud construction test\n";
}

void test_PointCloud_addPoint()
{
    PointCloud cloud;

    Point p1(1.0F, 2.0F, 3.0F);
    cloud.addPoint(p1);

    assert(cloud.size() == 1);
    assert(!cloud.empty());

    const std::vector<Point> &points = cloud.getPoints();
    assert(points[0].x() == 1.0F);
    assert(points[0].y() == 2.0F);
    assert(points[0].z() == 3.0F);

    // Add another point
    Point p2(4.0F, 5.0F, 6.0F);
    cloud.addPoint(p2);

    assert(cloud.size() == 2);
    assert(points[1].x() == 4.0F);
    assert(points[1].y() == 5.0F);
    assert(points[1].z() == 6.0F);

    std::cout << "[PASS] PointCloud addPoint test\n";
}

void test_PointCloud_addPoints()
{
    PointCloud cloud;

    // Add initial point
    cloud.addPoint(Point(0.0F, 0.0F, 0.0F));
    assert(cloud.size() == 1);

    // Add multiple points
    std::vector<Point> new_points = {
        Point(1.0F, 1.0F, 1.0F),
        Point(2.0F, 2.0F, 2.0F),
        Point(3.0F, 3.0F, 3.0F)};

    cloud.addPoints(new_points);
    assert(cloud.size() == 4);

    const std::vector<Point> &all_points = cloud.getPoints();
    assert(all_points[0].x() == 0.0F); // Original point
    assert(all_points[1].x() == 1.0F); // First new point
    assert(all_points[2].x() == 2.0F); // Second new point
    assert(all_points[3].x() == 3.0F); // Third new point

    std::cout << "[PASS] PointCloud addPoints test\n";
}

void test_PointCloud_clear()
{
    PointCloud cloud;

    // Add some points
    cloud.addPoint(Point(1.0F, 2.0F, 3.0F));
    cloud.addPoint(Point(4.0F, 5.0F, 6.0F));
    assert(cloud.size() == 2);
    assert(!cloud.empty());

    // Clear the cloud
    cloud.clear();
    assert(cloud.size() == 0);
    assert(cloud.empty());

    std::cout << "[PASS] PointCloud clear test\n";
}

void test_PointCloud_merge_operator()
{
    // Create first cloud
    PointCloud cloud1;
    cloud1.addPoint(Point(1.0F, 2.0F, 3.0F));
    cloud1.addPoint(Point(4.0F, 5.0F, 6.0F));

    // Create second cloud
    PointCloud cloud2;
    cloud2.addPoint(Point(7.0F, 8.0F, 9.0F));
    cloud2.addPoint(Point(10.0F, 11.0F, 12.0F));

    // Merge clouds
    PointCloud merged = cloud1 + cloud2;

    assert(merged.size() == 4);
    assert(cloud1.size() == 2); // Original clouds unchanged
    assert(cloud2.size() == 2);

    const std::vector<Point> &merged_points = merged.getPoints();

    // Check that all points are present
    assert(merged_points[0].x() == 1.0F);  // From cloud1
    assert(merged_points[1].x() == 4.0F);  // From cloud1
    assert(merged_points[2].x() == 7.0F);  // From cloud2
    assert(merged_points[3].x() == 10.0F); // From cloud2

    std::cout << "[PASS] PointCloud merge operator test\n";
}

void test_PointCloud_empty_operations()
{
    PointCloud empty_cloud;

    // Test operations on empty cloud
    assert(empty_cloud.empty());
    assert(empty_cloud.size() == 0);
    assert(empty_cloud.getPoints().empty());

    // Clear empty cloud (should not crash)
    empty_cloud.clear();
    assert(empty_cloud.empty());

    // Merge with empty cloud
    PointCloud cloud_with_data;
    cloud_with_data.addPoint(Point(1.0F, 2.0F, 3.0F));

    PointCloud merged1 = empty_cloud + cloud_with_data;
    PointCloud merged2 = cloud_with_data + empty_cloud;

    assert(merged1.size() == 1);
    assert(merged2.size() == 1);
    assert(merged1.getPoints()[0].x() == 1.0F);
    assert(merged2.getPoints()[0].x() == 1.0F);

    std::cout << "[PASS] PointCloud empty operations test\n";
}

void test_PointCloud_getPoints_const()
{
    PointCloud cloud;
    cloud.addPoint(Point(1.0F, 2.0F, 3.0F));
    cloud.addPoint(Point(4.0F, 5.0F, 6.0F));

    const PointCloud &const_cloud = cloud;
    const std::vector<Point> &points = const_cloud.getPoints();

    assert(points.size() == 2);
    assert(points[0].x() == 1.0F);
    assert(points[1].x() == 4.0F);

    std::cout << "[PASS] PointCloud getPoints const test\n";
}

void test_PointCloud_toString()
{
    PointCloud empty_cloud;
    std::string empty_str = empty_cloud.toString();
    assert(empty_str.find("0 points") != std::string::npos);
    assert(empty_str.find("PointCloud") != std::string::npos);

    PointCloud cloud_with_points;
    cloud_with_points.addPoint(Point(1.0F, 2.0F, 3.0F));
    cloud_with_points.addPoint(Point(4.0F, 5.0F, 6.0F));
    cloud_with_points.addPoint(Point(7.0F, 8.0F, 9.0F));

    std::string points_str = cloud_with_points.toString();
    assert(points_str.find("3 points") != std::string::npos);
    assert(points_str.find("PointCloud") != std::string::npos);

    std::cout << "[PASS] PointCloud toString test\n";
}

void test_PointCloud_large_dataset()
{
    // Test with a larger number of points to ensure performance is reasonable
    PointCloud cloud;

    const size_t num_points = 1000;
    for (size_t i = 0; i < num_points; ++i)
    {
        cloud.addPoint(Point(static_cast<float>(i),
                             static_cast<float>(i * 2),
                             static_cast<float>(i * 3)));
    }

    assert(cloud.size() == num_points);
    assert(!cloud.empty());

    const std::vector<Point> &points = cloud.getPoints();
    assert(points[0].x() == 0.0F);
    assert(points[999].x() == 999.0F);
    assert(points[999].y() == 1998.0F);
    assert(points[999].z() == 2997.0F);

    // Test merging large clouds
    PointCloud cloud2;
    for (size_t i = 0; i < 500; ++i)
    {
        cloud2.addPoint(Point(static_cast<float>(i + 1000),
                              static_cast<float>((i + 1000) * 2),
                              static_cast<float>((i + 1000) * 3)));
    }

    PointCloud merged = cloud + cloud2;
    assert(merged.size() == 1500);

    std::cout << "[PASS] PointCloud large dataset test\n";
}

int main()
{
    test_PointCloud_construction();
    test_PointCloud_addPoint();
    test_PointCloud_addPoints();
    test_PointCloud_clear();
    test_PointCloud_merge_operator();
    test_PointCloud_empty_operations();
    test_PointCloud_getPoints_const();
    test_PointCloud_toString();
    test_PointCloud_large_dataset();

    std::cout << "\n=== All PointCloud tests passed! ===\n";
    return 0;
}
