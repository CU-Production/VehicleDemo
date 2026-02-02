#include "PhysicsVehicle.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

using namespace JPH;

PhysicsVehicle::PhysicsVehicle(PhysicsWorld& world, VehicleModel& model, VehicleType type, const RVec3& position)
    : world_(world), model_(model), type_(type) {

    Vec3 halfExtent(1.0f, 0.5f, 2.0f);
    switch (type_) {
        case VehicleType::Kart:
            halfExtent = Vec3(1.1f, 0.3f, 0.6f);
            settings_.mass = 450.f;
            settings_.engineForce = 4500.f;
            settings_.maxSpeed = 22.f;
            settings_.steerTorque = 1200.f;
            break;
        case VehicleType::Sedan:
            halfExtent = Vec3(1.8f, 0.5f, 0.8f);
            settings_.mass = 1100.f;
            settings_.engineForce = 8000.f;
            settings_.maxSpeed = 26.f;
            settings_.steerTorque = 1800.f;
            break;
        case VehicleType::Truck:
            halfExtent = Vec3(2.6f, 0.6f, 1.0f);
            settings_.mass = 2600.f;
            settings_.engineForce = 12000.f;
            settings_.maxSpeed = 18.f;
            settings_.steerTorque = 1400.f;
            break;
        default:
            break;
    }
    auto shape = new BoxShape(halfExtent);

    BodyCreationSettings settings(
        shape,
        position,
        Quat::sIdentity(),
        EMotionType::Dynamic,
        PhysicsLayers::Dynamic);

    settings.mLinearDamping = settings_.linearDamping;
    settings.mAngularDamping = settings_.angularDamping;
    settings.mMassPropertiesOverride.mMass = settings_.mass;
    settings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;

    BodyInterface& bodyInterface = world_.bodyInterface();
    bodyId_ = bodyInterface.CreateAndAddBody(settings, EActivation::Activate);
}

void PhysicsVehicle::applyInput(const VehicleInput& input) {
    BodyInterface& bodyInterface = world_.bodyInterface();

    Vec3 velocity = bodyInterface.GetLinearVelocity(bodyId_);
    Quat rotation = bodyInterface.GetRotation(bodyId_);
    Vec3 forward = rotation * Vec3::sAxisZ();

    float forwardSpeed = velocity.Dot(forward);
    if (std::abs(forwardSpeed) < settings_.maxSpeed) {
        Vec3 force = forward * (input.throttle * settings_.engineForce);
        bodyInterface.AddForce(bodyId_, force);
    }

    if (input.brake) {
        bodyInterface.AddForce(bodyId_, -velocity * settings_.brakeForce);
    }

    Vec3 torque = Vec3(0, 1, 0) * (input.steer * settings_.steerTorque);
    bodyInterface.AddTorque(bodyId_, torque);
}

void PhysicsVehicle::syncVisual() {
    BodyInterface& bodyInterface = world_.bodyInterface();
    RVec3 position = bodyInterface.GetPosition(bodyId_);
    Quat rotation = bodyInterface.GetRotation(bodyId_);

    model_.group->position.set(position.GetX(), position.GetY(), position.GetZ());
    model_.group->quaternion.set(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW());
}

float PhysicsVehicle::speed() const {
    Vec3 velocity = world_.bodyInterface().GetLinearVelocity(bodyId_);
    return velocity.Length();
}

VehicleSettings& PhysicsVehicle::settings() {
    return settings_;
}

VehicleType PhysicsVehicle::type() const {
    return type_;
}
