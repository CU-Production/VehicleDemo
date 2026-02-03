#include "threepp/threepp.hpp"
#include "ImguiContextCompat.hpp"
#include "TestScene.h"

using namespace threepp;

int main() {

    Canvas canvas("Vehicle Demo");
    GLRenderer renderer{canvas.size()};
    renderer.shadowMap().enabled = true;
    renderer.shadowMap().type = ShadowMap::PFCSoft;
    auto testScene = createTestScene(canvas);
    ImguiFunctionalContextCompat ui{canvas, [&]() {
        testScene.drawUi();
    }};
    canvas.addKeyListener(testScene.controller);
    canvas.onWindowResize([&](WindowSize size) {
        testScene.onResize(size, renderer);
    });

    Clock clock;
    canvas.animate([&]() {

        float dt = clock.getDelta();
        testScene.update(dt);

        renderer.render(*testScene.scene, *testScene.camera);
        ui.render();
    });
}
