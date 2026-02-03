#include "VehicleController.h"
#include "PhysicsVehicle.h"

#include <algorithm>

using namespace threepp;

VehicleController::VehicleController() = default;

void VehicleController::update(float dt) {
    float targetThrottle = 0.f;
    if (accel_) targetThrottle += 1.f;
    if (reverse_) targetThrottle -= 1.f;

    float targetSteer = 0.f;
    if (left_) targetSteer -= 1.f;
    if (right_) targetSteer += 1.f;

    if (!accel_ && !reverse_) {
        throttle_ = 0.f;
    } else {
        float speed = std::clamp(dt * 6.f, 0.f, 1.f);
        throttle_ += (targetThrottle - throttle_) * speed;
    }

    if (!left_ && !right_) {
        steer_ = 0.f;
    } else {
        float speed = std::clamp(dt * 6.f, 0.f, 1.f);
        steer_ += (targetSteer - steer_) * speed;
    }
}

VehicleInput VehicleController::input() const {
    VehicleInput input;
    input.throttle = throttle_;
    input.steer = steer_;
    input.brake = brake_;
    input.handbrake = brake_;
    return input;
}

int VehicleController::consumeSwitchRequest() {
    int request = switchRequest_;
    switchRequest_ = -1;
    return request;
}

bool VehicleController::consumeResetRequest() {
    bool request = resetRequest_;
    resetRequest_ = false;
    return request;
}

void VehicleController::onKeyPressed(KeyEvent evt) {
    switch (evt.key) {
        case Key::W:
        case Key::UP:
            accel_ = true;
            break;
        case Key::S:
        case Key::DOWN:
            reverse_ = true;
            break;
        case Key::A:
        case Key::LEFT:
            left_ = true;
            break;
        case Key::D:
        case Key::RIGHT:
            right_ = true;
            break;
        case Key::SPACE:
            brake_ = true;
            break;
        case Key::NUM_1:
            switchRequest_ = 0;
            break;
        case Key::NUM_2:
            switchRequest_ = 1;
            break;
        case Key::NUM_3:
            switchRequest_ = 2;
            break;
        case Key::NUM_4:
            switchRequest_ = 3;
            break;
        case Key::NUM_5:
            switchRequest_ = 4;
            break;
        case Key::KP_1:
            switchRequest_ = 0;
            break;
        case Key::KP_2:
            switchRequest_ = 1;
            break;
        case Key::KP_3:
            switchRequest_ = 2;
            break;
        case Key::KP_4:
            switchRequest_ = 3;
            break;
        case Key::KP_5:
            switchRequest_ = 4;
            break;
        case Key::R:
            resetRequest_ = true;
            break;
        default:
            break;
    }
}

void VehicleController::onKeyReleased(KeyEvent evt) {
    switch (evt.key) {
        case Key::W:
        case Key::UP:
            accel_ = false;
            break;
        case Key::S:
        case Key::DOWN:
            reverse_ = false;
            break;
        case Key::A:
        case Key::LEFT:
            left_ = false;
            break;
        case Key::D:
        case Key::RIGHT:
            right_ = false;
            break;
        case Key::SPACE:
            brake_ = false;
            break;
        default:
            break;
    }
}
