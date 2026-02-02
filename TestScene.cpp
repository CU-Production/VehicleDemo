#include "TestScene.h"

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
    auto hemi = HemisphereLight::create(Color::white, Color::gray, 0.9f);
    scene->add(hemi);

    auto dir = DirectionalLight::create(Color::white, 0.8f);
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
    return testScene;
}

void TestScene::update(float dt) {
    const float wheelSpin = 2.5f * dt;
    for (const auto& vehicle : vehicles) {
        for (const auto& wheel : vehicle.wheels) {
            wheel->rotation.x -= wheelSpin;
        }
    }
}

void TestScene::onResize(WindowSize size, GLRenderer& renderer) {
    camera->aspect = size.aspect();
    camera->updateProjectionMatrix();
    renderer.setSize(size);
}
