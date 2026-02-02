#include "TestScene.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <imgui.h>

using namespace threepp;

namespace {

std::shared_ptr<Mesh> createGround() {
    auto planeGeometry = PlaneGeometry::create(60, 60);
    auto planeMaterial = MeshLambertMaterial::create();
    planeMaterial->color = Color::gray;
    planeMaterial->side = Side::Double;

    auto plane = Mesh::create(planeGeometry, planeMaterial);
    plane->position.y = -1;
    plane->rotateX(math::degToRad(90));
    return plane;
}

void addLights(const std::shared_ptr<Scene>& scene) {
    auto hemi = HemisphereLight::create(threepp::Color::white, threepp::Color::gray, 0.9f);
    scene->add(hemi);

    auto dir = DirectionalLight::create(threepp::Color::white, 0.8f);
    dir->position.set(5, 8, 5);
    scene->add(dir);
}

} // namespace

TestScene createTestScene(Canvas& canvas) {
    TestScene testScene;
    testScene.scene = Scene::create();
    testScene.camera = PerspectiveCamera::create(60, canvas.aspect(), 0.1f, 200);
    testScene.camera->position.set(10, 8, 12);

    testScene.controls = std::make_unique<OrbitControls>(*testScene.camera, canvas);
    testScene.controls->target.set(0, 1, 0);
    testScene.controls->update();

    addLights(testScene.scene);

    auto ground = createGround();
    testScene.scene->add(ground);

    testScene.physics = std::make_unique<PhysicsWorld>();

    JPH::BodyInterface& bodyInterface = testScene.physics->bodyInterface();
    auto groundShape = new JPH::BoxShape(JPH::Vec3(30.f, 1.f, 30.f));
    JPH::BodyCreationSettings groundSettings(
        groundShape,
        JPH::RVec3(0, -1, 0),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,
        PhysicsLayers::Static);
    bodyInterface.CreateAndAddBody(groundSettings, JPH::EActivation::DontActivate);

    auto kart = VehicleFactory::create(VehicleType::Kart);
    kart.group->position.set(-6, 0, 0);
    testScene.scene->add(kart.group);

    auto sedan = VehicleFactory::create(VehicleType::Sedan);
    sedan.group->position.set(0, 0, 0);
    testScene.scene->add(sedan.group);

    auto truck = VehicleFactory::create(VehicleType::Truck);
    truck.group->position.set(7, 0, 0);
    testScene.scene->add(truck.group);

    testScene.vehicles = {kart, sedan, truck};
    testScene.physicsVehicles.emplace_back(std::make_unique<PhysicsVehicle>(*testScene.physics, testScene.vehicles[0], VehicleType::Kart, JPH::RVec3(-6, 0.5f, 0)));
    testScene.physicsVehicles.emplace_back(std::make_unique<PhysicsVehicle>(*testScene.physics, testScene.vehicles[1], VehicleType::Sedan, JPH::RVec3(0, 0.5f, 0)));
    testScene.physicsVehicles.emplace_back(std::make_unique<PhysicsVehicle>(*testScene.physics, testScene.vehicles[2], VehicleType::Truck, JPH::RVec3(7, 0.5f, 0)));

    return testScene;
}

void TestScene::update(float dt) {
    controller.update(dt);

    int switchTo = controller.consumeSwitchRequest();
    if (switchTo >= 0 && switchTo < static_cast<int>(physicsVehicles.size())) {
        activeVehicle = switchTo;
    }

    if (!physicsVehicles.empty()) {
        physicsVehicles[activeVehicle]->applyInput(controller.input());
    }

    physics->step(dt);

    for (auto& vehicle : physicsVehicles) {
        vehicle->syncVisual();
    }

    for (size_t i = 0; i < physicsVehicles.size(); ++i) {
        float speed = physicsVehicles[i]->speed();
        float wheelSpin = speed * dt;
        for (const auto& wheel : vehicles[i].wheels) {
            wheel->rotation.z -= wheelSpin;
        }
    }
}

void TestScene::drawUi() {
    ImGui::Begin("Vehicle Debug");
    ImGui::Text("Controls: W/S throttle, A/D steer, Space brake");
    ImGui::Text("Switch vehicle: 1/2/3");

    if (!physicsVehicles.empty()) {
        ImGui::Separator();
        ImGui::Text("Active vehicle: %d", activeVehicle + 1);

        auto& vehicle = physicsVehicles[activeVehicle];
        auto& settings = vehicle->settings();
        ImGui::SliderFloat("Engine force", &settings.engineForce, 2000.f, 20000.f);
        ImGui::SliderFloat("Max speed", &settings.maxSpeed, 5.f, 60.f);
        ImGui::SliderFloat("Steer torque", &settings.steerTorque, 200.f, 6000.f);
        ImGui::SliderFloat("Brake force", &settings.brakeForce, 200.f, 4000.f);
        ImGui::Text("Speed: %.2f m/s", vehicle->speed());
    }
    ImGui::End();
}

void TestScene::onResize(WindowSize size, GLRenderer& renderer) {
    camera->aspect = size.aspect();
    camera->updateProjectionMatrix();
    renderer.setSize(size);
}
