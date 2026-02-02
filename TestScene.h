#pragma once

#include "threepp/threepp.hpp"
#include "PhysicsVehicle.h"
#include "VehicleController.h"
#include "VehicleFactory.h"
#include <memory>
#include <vector>

struct TestScene {
    std::shared_ptr<threepp::Scene> scene;
    std::shared_ptr<threepp::PerspectiveCamera> camera;
    std::unique_ptr<threepp::OrbitControls> controls;
    std::vector<VehicleModel> vehicles;
    std::unique_ptr<PhysicsWorld> physics;
    std::vector<std::unique_ptr<PhysicsVehicle>> physicsVehicles;
    VehicleController controller;
    int activeVehicle = 0;

    void update(float dt);
    void drawUi();
    void onResize(threepp::WindowSize size, threepp::GLRenderer& renderer);
};

TestScene createTestScene(threepp::Canvas& canvas);
