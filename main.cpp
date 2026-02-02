#include "threepp/threepp.hpp"
#include "TestScene.h"

using namespace threepp;

int main() {

    Canvas canvas("Vehicle Demo");
    GLRenderer renderer{canvas.size()};
    auto testScene = createTestScene(canvas);
    canvas.onWindowResize([&](WindowSize size) {
        testScene.onResize(size, renderer);
    });

    Clock clock;
    canvas.animate([&]() {

        float dt = clock.getDelta();
        testScene.update(dt);
        renderer.render(*testScene.scene, *testScene.camera);
    });
}
