#pragma once

#include "threepp/threepp.hpp"
#include "VehicleFactory.h"
#include <memory>
#include <vector>

struct TestScene {
    std::shared_ptr<threepp::Scene> scene;
    std::shared_ptr<threepp::PerspectiveCamera> camera;
    std::unique_ptr<threepp::OrbitControls> controls;
    std::vector<VehicleModel> vehicles;

    void update(float dt);
    void onResize(threepp::WindowSize size, threepp::GLRenderer& renderer);
};

TestScene createTestScene(threepp::Canvas& canvas);
