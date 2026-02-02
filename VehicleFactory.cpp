#include "VehicleFactory.h"

using namespace threepp;

namespace {

std::shared_ptr<Mesh> createBox(const Vector3& size, const Vector3& pos, const Color& color) {
    auto geometry = BoxGeometry::create(size.x, size.y, size.z);
    auto material = MeshPhongMaterial::create();
    material->color = color;

    auto box = Mesh::create(geometry, material);
    box->position.copy(pos);
    return box;
}

std::shared_ptr<Mesh> createWheel(float radius, float width, const Color& color) {
    auto geometry = CylinderGeometry::create(radius, radius, width, 16);
    auto material = MeshPhongMaterial::create();
    material->color = color;

    auto wheel = Mesh::create(geometry, material);
    // Align cylinder axis (Y) to vehicle axle (Z)
    wheel->rotateX(math::degToRad(90));
    return wheel;
}

VehicleModel buildKart() {
    auto group = Group::create();

    auto body = createBox({2.2f, 0.4f, 1.2f}, {0, 0.4f, 0}, Color::green);
    auto seat = createBox({0.8f, 0.6f, 0.8f}, {-0.2f, 0.8f, 0}, Color::darkgreen);
    group->add(body);
    group->add(seat);

    std::vector<std::shared_ptr<Mesh>> wheels;
    const float r = 0.35f;
    const float w = 0.25f;
    const float x = 0.9f;
    const float z = 0.6f;

    auto w1 = createWheel(r, w, Color::black);
    w1->position.set(x, r, z);
    auto w2 = createWheel(r, w, Color::black);
    w2->position.set(x, r, -z);
    auto w3 = createWheel(r, w, Color::black);
    w3->position.set(-x, r, z);
    auto w4 = createWheel(r, w, Color::black);
    w4->position.set(-x, r, -z);

    wheels = {w1, w2, w3, w4};
    for (const auto& wheel : wheels) group->add(wheel);

    return {group, wheels};
}

VehicleModel buildSedan() {
    auto group = Group::create();

    auto body = createBox({3.6f, 0.6f, 1.6f}, {0, 0.6f, 0}, Color::blue);
    auto cabin = createBox({1.6f, 0.8f, 1.4f}, {-0.2f, 1.2f, 0}, Color::lightblue);
    group->add(body);
    group->add(cabin);

    std::vector<std::shared_ptr<Mesh>> wheels;
    const float r = 0.45f;
    const float w = 0.3f;
    const float x = 1.3f;
    const float z = 0.8f;

    auto w1 = createWheel(r, w, Color::black);
    w1->position.set(x, r, z);
    auto w2 = createWheel(r, w, Color::black);
    w2->position.set(x, r, -z);
    auto w3 = createWheel(r, w, Color::black);
    w3->position.set(-x, r, z);
    auto w4 = createWheel(r, w, Color::black);
    w4->position.set(-x, r, -z);

    wheels = {w1, w2, w3, w4};
    for (const auto& wheel : wheels) group->add(wheel);

    return {group, wheels};
}

VehicleModel buildTruck() {
    auto group = Group::create();

    auto body = createBox({5.2f, 0.8f, 2.0f}, {0.4f, 0.8f, 0}, Color::red);
    auto cabin = createBox({1.6f, 1.2f, 1.8f}, {-1.8f, 1.4f, 0}, Color::darkred);
    group->add(body);
    group->add(cabin);

    std::vector<std::shared_ptr<Mesh>> wheels;
    const float r = 0.55f;
    const float w = 0.35f;
    const float z = 1.0f;

    auto w1 = createWheel(r, w, Color::black);
    w1->position.set(2.0f, r, z);
    auto w2 = createWheel(r, w, Color::black);
    w2->position.set(2.0f, r, -z);
    auto w3 = createWheel(r, w, Color::black);
    w3->position.set(-0.2f, r, z);
    auto w4 = createWheel(r, w, Color::black);
    w4->position.set(-0.2f, r, -z);
    auto w5 = createWheel(r, w, Color::black);
    w5->position.set(-1.6f, r, z);
    auto w6 = createWheel(r, w, Color::black);
    w6->position.set(-1.6f, r, -z);

    wheels = {w1, w2, w3, w4, w5, w6};
    for (const auto& wheel : wheels) group->add(wheel);

    return {group, wheels};
}

} // namespace

VehicleModel VehicleFactory::create(VehicleType type) {
    switch (type) {
        case VehicleType::Kart:
            return createKart();
        case VehicleType::Sedan:
            return createSedan();
        case VehicleType::Truck:
            return createTruck();
        default:
            return createKart();
    }
}

VehicleModel VehicleFactory::createKart() {
    return buildKart();
}

VehicleModel VehicleFactory::createSedan() {
    return buildSedan();
}

VehicleModel VehicleFactory::createTruck() {
    return buildTruck();
}
