#include "TestScene.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <imgui.h>
#include "threepp/cameras/OrthographicCamera.hpp"

using namespace threepp;

namespace {

std::shared_ptr<Group> createGround() {
    auto group = Group::create();

    auto groundGeometry = PlaneGeometry::create(1400, 1400);
    auto groundMaterial = MeshLambertMaterial::create();
    groundMaterial->color = Color(0x3a5f3a);
    groundMaterial->side = Side::Double;
    auto ground = Mesh::create(groundGeometry, groundMaterial);
    ground->position.y = 0;
    ground->rotateX(math::degToRad(90));
    ground->receiveShadow = true;
    group->add(ground);

    const float trackInner = 120.f;
    const float trackOuter = 170.f;

    auto trackMaterial = MeshLambertMaterial::create();
    trackMaterial->color = Color(0x303030);
    trackMaterial->side = Side::Double;
    auto track = Mesh::create(RingGeometry::create(trackInner, trackOuter, 96), trackMaterial);
    track->position.y = 0.01f;
    track->rotateX(math::degToRad(90));
    track->receiveShadow = true;
    group->add(track);

    auto lineMaterial = MeshLambertMaterial::create();
    lineMaterial->color = Color(0xf1f1f1);
    lineMaterial->side = Side::Double;
    auto centerLine = Mesh::create(RingGeometry::create(144.0f, 146.0f, 128), lineMaterial);
    centerLine->position.y = 0.02f;
    centerLine->rotateX(math::degToRad(90));
    group->add(centerLine);

    auto curbMaterial = MeshLambertMaterial::create();
    curbMaterial->color = Color(0xff4d4d);
    curbMaterial->side = Side::Double;
    auto curbInner = Mesh::create(RingGeometry::create(trackInner - 3.5f, trackInner, 128), curbMaterial);
    curbInner->position.y = 0.015f;
    curbInner->rotateX(math::degToRad(90));
    group->add(curbInner);
    auto curbOuter = Mesh::create(RingGeometry::create(trackOuter, trackOuter + 3.5f, 128), curbMaterial);
    curbOuter->position.y = 0.015f;
    curbOuter->rotateX(math::degToRad(90));
    group->add(curbOuter);

    auto startLine = Mesh::create(PlaneGeometry::create(12, 4.f), lineMaterial);
    startLine->position.set(trackOuter - 4.f, 0.025f, 0);
    startLine->rotateX(math::degToRad(90));
    group->add(startLine);

    auto outerGrassMaterial = MeshLambertMaterial::create();
    outerGrassMaterial->color = Color(0x4fa24f);
    outerGrassMaterial->side = Side::Double;
    auto outerGrass = Mesh::create(RingGeometry::create(trackOuter + 6.f, trackOuter + 28.f, 128), outerGrassMaterial);
    outerGrass->position.y = 0.005f;
    outerGrass->rotateX(math::degToRad(90));
    group->add(outerGrass);

    auto innerGrassMaterial = MeshLambertMaterial::create();
    innerGrassMaterial->color = Color(0x55b955);
    innerGrassMaterial->side = Side::Double;
    auto innerGrass = Mesh::create(RingGeometry::create(0.f, trackInner - 6.f, 128), innerGrassMaterial);
    innerGrass->position.y = 0.005f;
    innerGrass->rotateX(math::degToRad(90));
    group->add(innerGrass);

    auto stripeMaterial = MeshLambertMaterial::create();
    stripeMaterial->color = Color(0xffffff);
    stripeMaterial->side = Side::Double;
    for (int i = 0; i < 18; ++i) {
        float angle = math::degToRad(i * 20.f);
        float r = (trackInner + trackOuter) * 0.5f;
        auto stripe = Mesh::create(PlaneGeometry::create(8.f, 2.f), stripeMaterial);
        stripe->position.set(std::cos(angle) * r, 0.03f, std::sin(angle) * r);
        stripe->rotation.set(math::degToRad(90), -angle, 0);
        group->add(stripe);
    }

    auto railMaterial = MeshLambertMaterial::create();
    railMaterial->color = Color(0xff6b6b);
    auto railInner = Mesh::create(RingGeometry::create(trackInner - 8.f, trackInner - 6.f, 128), railMaterial);
    railInner->position.y = 0.05f;
    railInner->rotateX(math::degToRad(90));
    group->add(railInner);
    auto railOuter = Mesh::create(RingGeometry::create(trackOuter + 6.f, trackOuter + 8.f, 128), railMaterial);
    railOuter->position.y = 0.05f;
    railOuter->rotateX(math::degToRad(90));
    group->add(railOuter);

    auto bannerMaterial = MeshLambertMaterial::create();
    bannerMaterial->color = Color(0x06d6a0);
    auto banner = Mesh::create(BoxGeometry::create(14.f, 1.f, 2.f), bannerMaterial);
    banner->position.set(trackOuter - 10.f, 4.f, 0.f);
    banner->castShadow = true;
    banner->receiveShadow = true;
    group->add(banner);
    auto bannerLegL = Mesh::create(BoxGeometry::create(0.6f, 6.f, 0.6f), bannerMaterial);
    bannerLegL->position.set(trackOuter - 17.f, 2.f, 0.f);
    bannerLegL->castShadow = true;
    bannerLegL->receiveShadow = true;
    group->add(bannerLegL);
    auto bannerLegR = Mesh::create(BoxGeometry::create(0.6f, 6.f, 0.6f), bannerMaterial);
    bannerLegR->position.set(trackOuter - 3.f, 2.f, 0.f);
    bannerLegR->castShadow = true;
    bannerLegR->receiveShadow = true;
    group->add(bannerLegR);

    auto pitMaterial = MeshLambertMaterial::create();
    pitMaterial->color = Color(0x8ecae6);
    for (int i = 0; i < 6; ++i) {
        auto pit = Mesh::create(BoxGeometry::create(10.f, 3.f, 6.f), pitMaterial);
        pit->position.set(trackOuter + 30.f, 1.5f, -30.f + i * 12.f);
        pit->castShadow = true;
        pit->receiveShadow = true;
        group->add(pit);
    }

    auto treeMaterial = MeshLambertMaterial::create();
    treeMaterial->color = Color(0x2d6a4f);
    auto trunkMaterial = MeshLambertMaterial::create();
    trunkMaterial->color = Color(0x7f5539);
    for (int i = 0; i < 10; ++i) {
        float angle = math::degToRad(i * 36.f);
        float r = trackOuter + 60.f;
        auto trunk = Mesh::create(CylinderGeometry::create(0.6f, 0.8f, 6.f, 8), trunkMaterial);
        trunk->position.set(std::cos(angle) * r, 3.f, std::sin(angle) * r);
        trunk->castShadow = true;
        trunk->receiveShadow = true;
        group->add(trunk);
        auto crown = Mesh::create(SphereGeometry::create(3.2f, 12, 12), treeMaterial);
        crown->position.set(std::cos(angle) * r, 7.f, std::sin(angle) * r);
        crown->castShadow = true;
        crown->receiveShadow = true;
        group->add(crown);
    }

    auto standMaterial = MeshLambertMaterial::create();
    standMaterial->color = Color(0xffd166);
    auto stand = Mesh::create(BoxGeometry::create(40.f, 6.f, 14.f), standMaterial);
    stand->position.set(trackOuter + 20.f, 3.f, 0.f);
    stand->castShadow = true;
    stand->receiveShadow = true;
    group->add(stand);

    auto roofMaterial = MeshLambertMaterial::create();
    roofMaterial->color = Color(0x118ab2);
    auto roof = Mesh::create(BoxGeometry::create(42.f, 1.f, 16.f), roofMaterial);
    roof->position.set(trackOuter + 20.f, 7.f, 0.f);
    roof->castShadow = true;
    roof->receiveShadow = true;
    group->add(roof);

    return group;
}

void createGroundBody(PhysicsWorld& physics) {
    JPH::BodyInterface& bodyInterface = physics.bodyInterface();
    auto groundShape = new JPH::BoxShape(JPH::Vec3(300.f, 0.5f, 300.f));
    JPH::BodyCreationSettings groundSettings(
        groundShape,
        JPH::RVec3(0, -0.5f, 0),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,
        PhysicsLayers::Static);
    bodyInterface.CreateAndAddBody(groundSettings, JPH::EActivation::DontActivate);
}

void setupVehicles(TestScene& testScene) {
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
    testScene.activeVehicle = 0;
}

void addLights(const std::shared_ptr<Scene>& scene) {
    auto hemi = HemisphereLight::create(threepp::Color::white, threepp::Color::gray, 0.9f);
    scene->add(hemi);

    auto dir = DirectionalLight::create(threepp::Color::white, 1.0f);
    dir->position.set(20, 25, 20);
    dir->castShadow = true;
    dir->shadow->mapSize.set(4096, 4096);
    dir->shadow->camera->nearPlane = 1.f;
    dir->shadow->camera->farPlane = 120.f;
    if (auto* shadowCam = dynamic_cast<OrthographicCamera*>(dir->shadow->camera.get())) {
        shadowCam->left = -60.f;
        shadowCam->right = 60.f;
        shadowCam->top = 60.f;
        shadowCam->bottom = -60.f;
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
    createGroundBody(*testScene.physics);
    setupVehicles(testScene);

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
    if (controller.consumeResetRequest()) {
        resetSimulation();
        return;
    }
    if (controller.consumeCameraToggleRequest()) {
        toggleCameraMode();
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

    if (cameraMode == CameraMode::ThirdPerson && !vehicles.empty()) {
        const auto& target = vehicles[activeVehicle];
        if (target.group) {
            Vector3 forward = Vector3(0, 0, 1);
            forward.applyQuaternion(target.group->quaternion);
            Vector3 desired = target.group->position + forward * (-thirdPersonDistance);
            desired.y += thirdPersonHeight;
            camera->position.copy(desired);

            Vector3 lookAt = target.group->position;
            lookAt.y += thirdPersonLookAtHeight;
            camera->lookAt(lookAt);
        }
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
    ImGui::Text("Reset: R");
    ImGui::Text("Camera toggle: C");

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
    if (ImGui::Button("Reset Scene")) {
        resetSimulation();
    }
    }

    ImGui::Separator();
    const char* modeLabel = cameraMode == CameraMode::Orbit ? "Orbit" : "Third Person";
    ImGui::Text("Camera mode: %s", modeLabel);
    if (ImGui::Button("Toggle Camera")) {
        toggleCameraMode();
    }
    if (cameraMode == CameraMode::ThirdPerson) {
        ImGui::SliderFloat("TP Distance", &thirdPersonDistance, 3.f, 20.f);
        ImGui::SliderFloat("TP Height", &thirdPersonHeight, 1.f, 8.f);
        ImGui::SliderFloat("TP Look Height", &thirdPersonLookAtHeight, 0.5f, 4.f);
    }

#ifdef JPH_DEBUG_RENDERER
    ImGui::Separator();
    ImGui::Checkbox("Jolt Debug Draw", &showDebugDraw);
#endif
    ImGui::End();
}

void TestScene::resetSimulation() {
    for (auto& vehicle : vehicles) {
        if (vehicle.group) {
            scene->remove(*vehicle.group);
        }
    }
    physicsVehicles.clear();
    vehicles.clear();
    physics.reset();

    physics = std::make_unique<PhysicsWorld>();
    createGroundBody(*physics);
    setupVehicles(*this);
}

void TestScene::toggleCameraMode() {
    if (cameraMode == CameraMode::Orbit) {
        cameraMode = CameraMode::ThirdPerson;
        if (controls) {
            controls->enabled = false;
        }
    } else {
        cameraMode = CameraMode::Orbit;
        if (controls) {
            controls->enabled = true;
            controls->target.set(0, 1, 0);
            controls->update();
        }
    }
}

void TestScene::onResize(WindowSize size, GLRenderer& renderer) {
    camera->aspect = size.aspect();
    camera->updateProjectionMatrix();
    renderer.setSize(size);
}
