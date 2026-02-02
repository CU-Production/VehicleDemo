#pragma once

#include "PhysicsWorld.h"
#include "VehicleFactory.h"

struct VehicleInput {
    float throttle = 0.f;
    float steer = 0.f;
    bool brake = false;
};

struct VehicleSettings {
    float mass = 900.f;
    float engineForce = 8000.f;
    float maxSpeed = 25.f;
    float steerTorque = 1800.f;
    float brakeForce = 1200.f;
    float linearDamping = 0.2f;
    float angularDamping = 0.6f;
};

class PhysicsVehicle {
public:
    PhysicsVehicle(PhysicsWorld& world, VehicleModel& model, VehicleType type, const JPH::RVec3& position);

    void applyInput(const VehicleInput& input);
    void syncVisual();

    float speed() const;
    VehicleSettings& settings();
    VehicleType type() const;

private:
    PhysicsWorld& world_;
    VehicleModel& model_;
    VehicleType type_;
    VehicleSettings settings_;
    JPH::BodyID bodyId_;
};
