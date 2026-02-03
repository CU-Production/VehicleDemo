#include "VehicleFactory.h"

using namespace threepp;

namespace {

std::shared_ptr<Mesh> createBox(const Vector3& size, const Vector3& pos, const Color& color) {
    auto geometry = BoxGeometry::create(size.x, size.y, size.z);
    auto material = MeshPhongMaterial::create();
    material->color = color;

    auto box = Mesh::create(geometry, material);
    box->position.copy(pos);
    box->castShadow = true;
    box->receiveShadow = true;
    return box;
}

std::shared_ptr<Mesh> createWheel(float radius, float width, const Color& color) {
    auto geometry = CylinderGeometry::create(radius, radius, width, 16);
    auto material = MeshPhongMaterial::create();
    material->color = color;

    auto wheel = Mesh::create(geometry, material);
    wheel->castShadow = true;
    wheel->receiveShadow = true;
    return wheel;
}

VehicleModel buildKart() {
    auto group = Group::create();

    auto body = createBox({1.2f, 0.4f, 2.2f}, {0, 0.0f, 0}, Color::green);
    auto seat = createBox({0.8f, 0.6f, 0.8f}, {0, 0.5f, -0.2f}, Color::darkgreen);
    auto bumperFront = createBox({1.3f, 0.2f, 0.2f}, {0, 0.1f, 1.2f}, Color(0x2a2a2a));
    auto bumperRear = createBox({1.3f, 0.2f, 0.2f}, {0, 0.1f, -1.2f}, Color(0x2a2a2a));
    auto engine = createBox({0.6f, 0.3f, 0.5f}, {0, 0.35f, 0.6f}, Color(0x444444));
    group->add(body);
    group->add(seat);
    group->add(bumperFront);
    group->add(bumperRear);
    group->add(engine);

    std::vector<std::shared_ptr<Mesh>> wheels;
    const float r = 0.35f;
    const float w = 0.25f;
    const float x = 0.7f;
    const float z = 0.9f;

    auto w1 = createWheel(r, w, Color::black);
    w1->position.set(x, r, z);
    auto w2 = createWheel(r, w, Color::black);
    w2->position.set(-x, r, z);
    auto w3 = createWheel(r, w, Color::black);
    w3->position.set(x, r, -z);
    auto w4 = createWheel(r, w, Color::black);
    w4->position.set(-x, r, -z);

    wheels = {w1, w2, w3, w4};
    std::vector<std::shared_ptr<Mesh>> steeringWheels = {w1, w2};
    for (const auto& wheel : wheels) group->add(wheel);

    return {group, wheels, steeringWheels};
}

VehicleModel buildSedan() {
    auto group = Group::create();

    auto body = createBox({1.6f, 0.6f, 3.6f}, {0, 0.0f, 0}, Color::blue);
    auto cabin = createBox({1.4f, 0.8f, 1.6f}, {0, 0.7f, -0.2f}, Color::lightblue);
    auto hood = createBox({1.5f, 0.3f, 0.9f}, {0, 0.35f, 1.1f}, Color(0x2f5aa6));
    auto trunk = createBox({1.5f, 0.25f, 0.9f}, {0, 0.3f, -1.2f}, Color(0x2f5aa6));
    auto bumperFront = createBox({1.6f, 0.25f, 0.15f}, {0, 0.15f, 1.85f}, Color(0x222222));
    auto bumperRear = createBox({1.6f, 0.25f, 0.15f}, {0, 0.15f, -1.85f}, Color(0x222222));
    group->add(body);
    group->add(cabin);
    group->add(hood);
    group->add(trunk);
    group->add(bumperFront);
    group->add(bumperRear);

    std::vector<std::shared_ptr<Mesh>> wheels;
    const float r = 0.45f;
    const float w = 0.3f;
    const float x = 0.9f;
    const float z = 1.3f;

    auto w1 = createWheel(r, w, Color::black);
    w1->position.set(x, r, z);
    auto w2 = createWheel(r, w, Color::black);
    w2->position.set(-x, r, z);
    auto w3 = createWheel(r, w, Color::black);
    w3->position.set(x, r, -z);
    auto w4 = createWheel(r, w, Color::black);
    w4->position.set(-x, r, -z);

    wheels = {w1, w2, w3, w4};
    std::vector<std::shared_ptr<Mesh>> steeringWheels = {w1, w2};
    for (const auto& wheel : wheels) group->add(wheel);

    return {group, wheels, steeringWheels};
}

VehicleModel buildTruck() {
    auto group = Group::create();

    auto body = createBox({2.0f, 0.8f, 5.2f}, {0, 0.0f, 0.4f}, Color::red);
    auto cabin = createBox({1.8f, 1.2f, 1.6f}, {0, 1.0f, 1.8f}, Color::darkred);
    auto grille = createBox({1.6f, 0.6f, 0.2f}, {0, 0.6f, 2.8f}, Color(0x222222));
    auto bumper = createBox({2.1f, 0.3f, 0.2f}, {0, 0.2f, 2.9f}, Color(0x333333));
    auto bed = createBox({2.0f, 0.6f, 2.4f}, {0, 0.5f, -1.4f}, Color(0x8b1a1a));
    group->add(body);
    group->add(cabin);
    group->add(grille);
    group->add(bumper);
    group->add(bed);

    std::vector<std::shared_ptr<Mesh>> wheels;
    const float r = 0.55f;
    const float w = 0.35f;
    const float x = 1.0f;
    const float z1 = 2.0f;
    const float z2 = -0.2f;
    const float z3 = -1.6f;

    auto w1 = createWheel(r, w, Color::black);
    w1->position.set(x, r, z1);
    auto w2 = createWheel(r, w, Color::black);
    w2->position.set(-x, r, z1);
    auto w3 = createWheel(r, w, Color::black);
    w3->position.set(x, r, z2);
    auto w4 = createWheel(r, w, Color::black);
    w4->position.set(-x, r, z2);
    auto w5 = createWheel(r, w, Color::black);
    w5->position.set(x, r, z3);
    auto w6 = createWheel(r, w, Color::black);
    w6->position.set(-x, r, z3);

    wheels = {w1, w2, w3, w4, w5, w6};
    std::vector<std::shared_ptr<Mesh>> steeringWheels = {w1, w2};
    for (const auto& wheel : wheels) group->add(wheel);

    return {group, wheels, steeringWheels};
}

VehicleModel buildTank() {
    auto group = Group::create();

    auto body = createBox({3.4f, 1.0f, 6.4f}, {0, 0.0f, 0}, Color::darkgray);
    auto turret = createBox({2.8f, 0.8f, 4.0f}, {0, 0.9f, 0}, Color::gray);
    auto hatch = createBox({0.8f, 0.2f, 1.0f}, {0, 1.35f, -0.4f}, Color(0x555555));
    auto barrel = createBox({0.3f, 0.3f, 3.0f}, {0, 1.05f, 3.0f}, Color(0x444444));
    auto skirtLeft = createBox({0.2f, 0.6f, 6.0f}, {1.9f, 0.2f, 0}, Color(0x2a2a2a));
    auto skirtRight = createBox({0.2f, 0.6f, 6.0f}, {-1.9f, 0.2f, 0}, Color(0x2a2a2a));
    group->add(body);
    group->add(turret);
    group->add(hatch);
    group->add(barrel);
    group->add(skirtLeft);
    group->add(skirtRight);

    std::vector<std::shared_ptr<Mesh>> wheels;
    const float r = 0.3f;
    const float w = 0.1f;
    const float x = 1.7f;
    const float zPositions[] = {2.95f, 2.1f, 1.4f, 0.7f, 0.0f, -0.7f, -1.4f, -2.1f, -2.75f};

    for (float z : zPositions) {
        auto wl = createWheel(r, w, Color::black);
        wl->position.set(x, r, z);
        wheels.push_back(wl);
        group->add(wl);
    }
    for (float z : zPositions) {
        auto wr = createWheel(r, w, Color::black);
        wr->position.set(-x, r, z);
        wheels.push_back(wr);
        group->add(wr);
    }

    std::vector<std::shared_ptr<Mesh>> steeringWheels;
    return {group, wheels, steeringWheels};
}

VehicleModel buildMotorcycle() {
    auto group = Group::create();

    auto body = createBox({0.5f, 0.6f, 1.6f}, {0, 0.0f, 0}, Color::darkblue);
    auto seat = createBox({0.4f, 0.4f, 0.6f}, {0, 0.5f, -0.2f}, Color::blue);
    auto tank = createBox({0.45f, 0.35f, 0.5f}, {0, 0.55f, 0.4f}, Color(0x1f3a93));
    auto fork = createBox({0.1f, 0.6f, 0.2f}, {0, 0.35f, 0.9f}, Color(0x3a3a3a));
    auto handle = createBox({0.6f, 0.1f, 0.1f}, {0, 0.9f, 0.7f}, Color(0x3a3a3a));
    group->add(body);
    group->add(seat);
    group->add(tank);
    group->add(fork);
    group->add(handle);

    std::vector<std::shared_ptr<Mesh>> wheels;
    const float r = 0.31f;
    const float w = 0.05f;
    const float zFront = 0.75f;
    const float zBack = -0.75f;

    auto wFront = createWheel(r, w, Color::black);
    wFront->position.set(0.f, r, zFront);
    auto wBack = createWheel(r, w, Color::black);
    wBack->position.set(0.f, r, zBack);

    wheels = {wFront, wBack};
    std::vector<std::shared_ptr<Mesh>> steeringWheels = {wFront};
    for (const auto& wheel : wheels) group->add(wheel);

    return {group, wheels, steeringWheels};
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
        case VehicleType::Tank:
            return createTank();
        case VehicleType::Motorcycle:
            return createMotorcycle();
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

VehicleModel VehicleFactory::createTank() {
    return buildTank();
}

VehicleModel VehicleFactory::createMotorcycle() {
    return buildMotorcycle();
}
