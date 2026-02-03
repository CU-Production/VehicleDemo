#include "TestScene.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <imgui.h>
#include "threepp/cameras/OrthographicCamera.hpp"

using namespace threepp;

namespace {

std::shared_ptr<Mesh> createGround() {
    auto planeGeometry = PlaneGeometry::create(60, 60);
    auto planeMaterial = MeshLambertMaterial::create();
    planeMaterial->color = Color::gray;
    planeMaterial->side = Side::Double;

    auto plane = Mesh::create(planeGeometry, planeMaterial);
    plane->position.y = 0;
    plane->rotateX(math::degToRad(90));
    plane->receiveShadow = true;
    return plane;
}

void addLights(const std::shared_ptr<Scene>& scene) {
    auto hemi = HemisphereLight::create(threepp::Color::white, threepp::Color::gray, 0.9f);
    scene->add(hemi);

    auto dir = DirectionalLight::create(threepp::Color::white, 0.8f);
    dir->position.set(5, 8, 5);
    dir->castShadow = true;
    dir->shadow->mapSize.set(2048, 2048);
    dir->shadow->camera->nearPlane = 1.f;
    dir->shadow->camera->farPlane = 50.f;
    if (auto* shadowCam = dynamic_cast<OrthographicCamera*>(dir->shadow->camera.get())) {
        shadowCam->left = -20.f;
        shadowCam->right = 20.f;
        shadowCam->top = 20.f;
        shadowCam->bottom = -20.f;
        shadowCam->updateProjectionMatrix();
    }
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
    auto groundShape = new JPH::BoxShape(JPH::Vec3(30.f, 0.5f, 30.f));
    JPH::BodyCreationSettings groundSettings(
        groundShape,
        JPH::RVec3(0, -0.5f, 0),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,
        PhysicsLayers::Static);
    bodyInterface.CreateAndAddBody(groundSettings, JPH::EActivation::DontActivate);

    auto kart = VehicleFactory::create(VehicleType::Kart);
    kart.group->position.set(-12, 0, 0);
    testScene.scene->add(kart.group);

    auto sedan = VehicleFactory::create(VehicleType::Sedan);
    sedan.group->position.set(-6, 0, 0);
    testScene.scene->add(sedan.group);

    auto truck = VehicleFactory::create(VehicleType::Truck);
    truck.group->position.set(2, 0, 0);
    testScene.scene->add(truck.group);

    auto tank = VehicleFactory::create(VehicleType::Tank);
    tank.group->position.set(10, 0, 0);
    testScene.scene->add(tank.group);

    auto motorcycle = VehicleFactory::create(VehicleType::Motorcycle);
    motorcycle.group->position.set(16, 0, 0);
    testScene.scene->add(motorcycle.group);

    testScene.vehicles = {kart, sedan, truck, tank, motorcycle};
    testScene.physicsVehicles.emplace_back(std::make_unique<PhysicsVehicle>(*testScene.physics, testScene.vehicles[0], VehicleType::Kart, JPH::RVec3(-12, PhysicsVehicle::spawnHeight(VehicleType::Kart), 0)));
    testScene.physicsVehicles.emplace_back(std::make_unique<PhysicsVehicle>(*testScene.physics, testScene.vehicles[1], VehicleType::Sedan, JPH::RVec3(-6, PhysicsVehicle::spawnHeight(VehicleType::Sedan), 0)));
    testScene.physicsVehicles.emplace_back(std::make_unique<PhysicsVehicle>(*testScene.physics, testScene.vehicles[2], VehicleType::Truck, JPH::RVec3(2, PhysicsVehicle::spawnHeight(VehicleType::Truck), 0)));
    testScene.physicsVehicles.emplace_back(std::make_unique<PhysicsVehicle>(*testScene.physics, testScene.vehicles[3], VehicleType::Tank, JPH::RVec3(10, PhysicsVehicle::spawnHeight(VehicleType::Tank), 0)));
    testScene.physicsVehicles.emplace_back(std::make_unique<PhysicsVehicle>(*testScene.physics, testScene.vehicles[4], VehicleType::Motorcycle, JPH::RVec3(16, PhysicsVehicle::spawnHeight(VehicleType::Motorcycle), 0)));

#ifdef JPH_DEBUG_RENDERER
    testScene.debugRenderer = std::make_unique<JoltDebugRenderer>();
    testScene.scene->add(testScene.debugRenderer->group());
#endif

    return testScene;
}

void TestScene::update(float dt) {
    controller.update(dt);

    int switchTo = controller.consumeSwitchRequest();
    if (switchTo >= 0 && switchTo < static_cast<int>(physicsVehicles.size())) {
        activeVehicle = switchTo;
    }

    VehicleInput input = controller.input();
    for (size_t i = 0; i < physicsVehicles.size(); ++i) {
        if (static_cast<int>(i) == activeVehicle) {
            physicsVehicles[i]->applyInput(input);
        } else {
            physicsVehicles[i]->applyInput({});
        }
    }

    physics->step(dt);

    for (auto& vehicle : physicsVehicles) {
        vehicle->syncVisual();
    }

#ifdef JPH_DEBUG_RENDERER
    if (showDebugDraw && debugRenderer) {
        debugRenderer->BeginFrame();
        debugRenderer->SetCameraPosition(camera->position);

        JPH::BodyManager::DrawSettings settings;
        settings.mDrawShape = true;
        settings.mDrawShapeColor = JPH::BodyManager::EShapeColor::ShapeTypeColor;
        settings.mDrawCenterOfMassTransform = true;
        settings.mDrawBoundingBox = false;
        settings.mDrawWorldTransform = false;
        settings.mDrawVelocity = false;

        physics->system().DrawBodies(settings, debugRenderer.get());
        physics->system().DrawConstraints(debugRenderer.get());
        debugRenderer->EndFrame();
    }
#endif
}

void TestScene::drawUi() {
    ImGui::Begin("Vehicle Debug");
    ImGui::Text("Controls: W/S throttle, A/D steer, Space brake");
    ImGui::Text("Switch vehicle: 1/2/3/4/5");

    if (!physicsVehicles.empty()) {
        ImGui::Separator();
        ImGui::Text("Active vehicle: %d", activeVehicle + 1);
        int maxIndex = static_cast<int>(physicsVehicles.size()) - 1;
        ImGui::SliderInt("Active index", &activeVehicle, 0, maxIndex);

        auto& vehicle = physicsVehicles[activeVehicle];
        auto& settings = vehicle->settings();
        ImGui::SliderFloat("Engine force", &settings.engineForce, 2000.f, 20000.f);
        ImGui::SliderFloat("Max speed", &settings.maxSpeed, 5.f, 60.f);
        ImGui::SliderFloat("Steer torque", &settings.steerTorque, 200.f, 6000.f);
        ImGui::SliderFloat("Brake force", &settings.brakeForce, 200.f, 4000.f);
        ImGui::Text("Speed: %.2f m/s", vehicle->speed());
    }

#ifdef JPH_DEBUG_RENDERER
    ImGui::Separator();
    ImGui::Checkbox("Jolt Debug Draw", &showDebugDraw);
#endif
    ImGui::End();
}

void TestScene::onResize(WindowSize size, GLRenderer& renderer) {
    camera->aspect = size.aspect();
    camera->updateProjectionMatrix();
    renderer.setSize(size);
}
