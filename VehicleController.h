#pragma once

#include "threepp/input/KeyListener.hpp"

struct VehicleInput;

class VehicleController : public threepp::KeyListener {
public:
    VehicleController();

    void update(float dt);
    VehicleInput input() const;

    int consumeSwitchRequest();

    void onKeyPressed(threepp::KeyEvent evt) override;
    void onKeyReleased(threepp::KeyEvent evt) override;

private:
    float throttle_ = 0.f;
    float steer_ = 0.f;
    bool accel_ = false;
    bool reverse_ = false;
    bool left_ = false;
    bool right_ = false;
    bool brake_ = false;
    int switchRequest_ = -1;
};
