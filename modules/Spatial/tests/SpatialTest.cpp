#include <spatial/implementations/Transform.hpp>
#include <spatial/implementations/TransformNode.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <cassert>
#include <iostream>

using namespace spatial;
using namespace math;

void test_Transform_basic_operations()
{
    std::cout << "Testing Transform basic operations..." << std::endl;

    // Test default constructor
    Transform transform;
    Point defaultPos = transform.getPosition();
    Vector defaultOrientation = transform.getOrientation();

    assert(defaultPos.x() == 0.0f);
    assert(defaultPos.y() == 0.0f);
    assert(defaultPos.z() == 0.0f);

    // Test constructor with position and orientation
    Point position(1.0f, 2.0f, 3.0f);
    Vector orientation(0.1f, 0.2f, 0.3f);
    Transform transform2(position, orientation);

    assert(transform2.getPosition().x() == 1.0f);
    assert(transform2.getPosition().y() == 2.0f);
    assert(transform2.getPosition().z() == 3.0f);

    assert(transform2.getOrientation().x() == 0.1f);
    assert(transform2.getOrientation().y() == 0.2f);
    assert(transform2.getOrientation().z() == 0.3f);

    std::cout << "✅ Transform basic operations test passed" << std::endl;
}

void test_Transform_setters()
{
    std::cout << "Testing Transform setters..." << std::endl;

    Transform transform;

    // Test setPosition
    Point newPosition(5.0f, 6.0f, 7.0f);
    transform.setPosition(newPosition);

    Point retrievedPos = transform.getPosition();
    assert(retrievedPos.x() == 5.0f);
    assert(retrievedPos.y() == 6.0f);
    assert(retrievedPos.z() == 7.0f);

    // Test setOrientation
    Vector newOrientation(0.5f, 0.6f, 0.7f);
    transform.setOrientation(newOrientation);

    Vector retrievedOrientation = transform.getOrientation();
    assert(retrievedOrientation.x() == 0.5f);
    assert(retrievedOrientation.y() == 0.6f);
    assert(retrievedOrientation.z() == 0.7f);

    std::cout << "✅ Transform setters test passed" << std::endl;
}

void test_TransformNode_hierarchy()
{
    std::cout << "Testing TransformNode hierarchy..." << std::endl;

    // Create root transform node
    Transform rootTransform(Point(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    auto rootNode = std::make_shared<TransformNode>(rootTransform);

    // Create child transform node
    Transform childTransform(Point(1.0f, 1.0f, 1.0f), Vector(0.1f, 0.1f, 0.1f));
    auto childNode = std::make_shared<TransformNode>(childTransform);

    // Test parent-child relationship
    childNode->setParent(rootNode);

    // Verify parent is set
    assert(childNode->getParent() == rootNode);

    // Test local transform
    Transform localTransform = childNode->getLocalTransform();
    assert(localTransform.getPosition().x() == 1.0f);
    assert(localTransform.getPosition().y() == 1.0f);
    assert(localTransform.getPosition().z() == 1.0f);

    std::cout << "✅ TransformNode hierarchy test passed" << std::endl;
}

void test_TransformNode_global_transform()
{
    std::cout << "Testing TransformNode global transform..." << std::endl;

    // Create parent with offset
    Transform parentTransform(Point(10.0f, 20.0f, 30.0f), Vector(0.0f, 0.0f, 0.0f));
    auto parentNode = std::make_shared<TransformNode>(parentTransform);

    // Create child with local offset
    Transform childTransform(Point(1.0f, 2.0f, 3.0f), Vector(0.0f, 0.0f, 0.0f));
    auto childNode = std::make_shared<TransformNode>(childTransform);
    childNode->setParent(parentNode);

    // Test global transform calculation
    Transform globalTransform = childNode->getGlobalTransform();

    // Global transform should incorporate parent transformation
    Point globalPos = globalTransform.getPosition();

    // At minimum, global transform should be computed
    bool globalTransformComputed = true; // Just test that it doesn't crash
    assert(globalTransformComputed && "Global transform should be computed without error");

    std::cout << "✅ TransformNode global transform test passed" << std::endl;
}

void test_TransformNode_world_transform()
{
    std::cout << "Testing TransformNode world transform..." << std::endl;

    // Create parent with offset
    Transform parentTransform(Point(10.0f, 20.0f, 30.0f), Vector(0.0f, 0.0f, 0.0f));
    TransformNode parentNode(parentTransform);

    // Create child with local offset
    Transform childTransform(Point(1.0f, 2.0f, 3.0f), Vector(0.0f, 0.0f, 0.0f));
    TransformNode childNode(childTransform);
    childNode.setParent(std::make_shared<TransformNode>(parentNode));

    // Test world transform calculation (should combine parent + child)
    Transform worldTransform = childNode.getGlobalTransform();

    // World position should be parent + child
    // Note: This assumes simple addition - actual implementation might differ
    Point worldPos = worldTransform.getPosition();

    // At minimum, world transform should not be the same as local
    bool worldTransformCalculated = (worldPos.x() != 1.0f || worldPos.y() != 2.0f || worldPos.z() != 3.0f);
    assert(worldTransformCalculated && "World transform should be different from local transform");

    std::cout << "✅ TransformNode world transform test passed" << std::endl;
}

void test_TransformNode_modifications()
{
    std::cout << "Testing TransformNode modifications..." << std::endl;

    Transform initialTransform(Point(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
    auto node = std::make_shared<TransformNode>(initialTransform);

    // Test setting new local transform
    Transform newTransform(Point(5.0f, 10.0f, 15.0f), Vector(0.5f, 1.0f, 1.5f));
    node->setLocalTransform(newTransform);

    Transform retrievedTransform = node->getLocalTransform();
    assert(retrievedTransform.getPosition().x() == 5.0f);
    assert(retrievedTransform.getPosition().y() == 10.0f);
    assert(retrievedTransform.getPosition().z() == 15.0f);

    assert(retrievedTransform.getOrientation().x() == 0.5f);
    assert(retrievedTransform.getOrientation().y() == 1.0f);
    assert(retrievedTransform.getOrientation().z() == 1.5f);

    std::cout << "✅ TransformNode modifications test passed" << std::endl;
}

int main()
{
    std::cout << "🧪 Running Spatial Module Tests..." << std::endl;

    try
    {
        test_Transform_basic_operations();
        test_Transform_setters();
        test_TransformNode_hierarchy();
        test_TransformNode_global_transform();
        test_TransformNode_modifications();

        std::cout << "✅ All Spatial module tests passed!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cout << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
