#include "PhysicsVehicle.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <Jolt/Physics/Vehicle/VehicleCollisionTester.h>
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>

using namespace JPH;

PhysicsVehicle::PhysicsVehicle(PhysicsWorld& world, VehicleModel& model, VehicleType type, const RVec3& position)
    : world_(world), model_(model), type_(type) {

    Vec3 halfExtent(1.0f, 0.5f, 2.0f);
    float wheelRadius = 0.4f;
    float wheelWidth = 0.3f;
    switch (type_) {
        case VehicleType::Kart:
            // Match visual body size: 1.2 x 0.4 x 2.2
            halfExtent = Vec3(0.6f, 0.2f, 1.1f);
            wheelRadius = 0.35f;
            wheelWidth = 0.25f;
            settings_.mass = 450.f;
            settings_.engineForce = 4500.f;
            settings_.maxSpeed = 22.f;
            settings_.steerTorque = 1200.f;
            break;
        case VehicleType::Sedan:
            // Match visual body size: 1.6 x 0.6 x 3.6
            halfExtent = Vec3(0.8f, 0.3f, 1.8f);
            wheelRadius = 0.45f;
            wheelWidth = 0.3f;
            settings_.mass = 1100.f;
            settings_.engineForce = 8000.f;
            settings_.maxSpeed = 26.f;
            settings_.steerTorque = 1800.f;
            break;
        case VehicleType::Truck:
            // Match visual body size: 2.0 x 0.8 x 5.2
            halfExtent = Vec3(1.0f, 0.4f, 2.6f);
            wheelRadius = 0.55f;
            wheelWidth = 0.35f;
            settings_.mass = 2600.f;
            settings_.engineForce = 12000.f;
            settings_.maxSpeed = 18.f;
            settings_.steerTorque = 1400.f;
            break;
        default:
            break;
    }
    // Match Jolt demo: lower COM for stability.
    const float comOffset = -0.9f * halfExtent.GetY();
    auto shape = OffsetCenterOfMassShapeSettings(Vec3(0, comOffset, 0), new BoxShape(halfExtent)).Create().Get();

    BodyCreationSettings bodySettings(
        shape,
        position,
        Quat::sIdentity(),
        EMotionType::Dynamic,
        PhysicsLayers::Dynamic);

    bodySettings.mLinearDamping = settings_.linearDamping;
    bodySettings.mAngularDamping = settings_.angularDamping;
    bodySettings.mMassPropertiesOverride.mMass = settings_.mass;
    bodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;

    BodyInterface& bodyInterface = world_.bodyInterface();
    body_ = bodyInterface.CreateBody(bodySettings);
    bodyId_ = body_->GetID();
    bodyInterface.AddBody(bodyId_, EActivation::Activate);

    VehicleConstraintSettings vehicleSettings;
    vehicleSettings.mMaxPitchRollAngle = JPH_PI / 3.f;

    vehicleSettings.mWheels.reserve(model_.wheels.size());
    wheelRights_.reserve(model_.wheels.size());

    Vec3 suspensionDir(0, -1, 0);
    Vec3 steeringAxis(0, 1, 0);
    Vec3 wheelUp(0, 1, 0);
    Vec3 wheelForward(0, 0, 1);

    for (const auto& wheel : model_.wheels) {
        auto* w = new WheelSettingsWV;
        // Keep wheel center relative to COM so that tire bottom sits near ground.
        w->mPosition = Vec3(wheel->position.x, comOffset, wheel->position.z);
        w->mSuspensionDirection = suspensionDir;
        w->mSteeringAxis = steeringAxis;
        w->mWheelUp = wheelUp;
        w->mWheelForward = wheelForward;
        // Match Jolt demo suspension range.
        w->mSuspensionMinLength = 0.3f;
        w->mSuspensionMaxLength = 0.5f;
        w->mSuspensionSpring.mFrequency = 1.5f;
        w->mSuspensionSpring.mDamping = 0.5f;
        w->mRadius = wheelRadius;
        w->mWidth = wheelWidth;
        w->mMaxSteerAngle = (wheel->position.z > 0) ? (JPH_PI / 6.f) : 0.f;
        w->mMaxBrakeTorque = settings_.brakeForce;
        w->mMaxHandBrakeTorque = (wheel->position.z > 0) ? 0.f : (settings_.brakeForce * 2.0f);

        vehicleSettings.mWheels.push_back(w);

        Vec3 wheelRight = Vec3::sAxisY();
        wheelRights_.push_back(wheelRight);
    }

    auto* controllerSettings = new WheeledVehicleControllerSettings;
    vehicleSettings.mController = controllerSettings;

    controllerSettings->mDifferentials.clear();
    if (vehicleSettings.mWheels.size() >= 2) {
        controllerSettings->mDifferentials.resize(1);
        controllerSettings->mDifferentials[0].mLeftWheel = 0;
        controllerSettings->mDifferentials[0].mRightWheel = 1;
    }
    if (vehicleSettings.mWheels.size() >= 4) {
        controllerSettings->mDifferentials.resize(2);
        controllerSettings->mDifferentials[1].mLeftWheel = 2;
        controllerSettings->mDifferentials[1].mRightWheel = 3;
        controllerSettings->mDifferentials[0].mEngineTorqueRatio = 0.5f;
        controllerSettings->mDifferentials[1].mEngineTorqueRatio = 0.5f;
    }
    if (vehicleSettings.mWheels.size() >= 6) {
        controllerSettings->mDifferentials.resize(3);
        controllerSettings->mDifferentials[2].mLeftWheel = 4;
        controllerSettings->mDifferentials[2].mRightWheel = 5;
        controllerSettings->mDifferentials[0].mEngineTorqueRatio = 1.f / 3.f;
        controllerSettings->mDifferentials[1].mEngineTorqueRatio = 1.f / 3.f;
        controllerSettings->mDifferentials[2].mEngineTorqueRatio = 1.f / 3.f;
    }

    vehicleConstraint_ = new VehicleConstraint(*body_, vehicleSettings);
    collisionTester_ = new VehicleCollisionTesterCastCylinder(PhysicsLayers::Dynamic);
    vehicleConstraint_->SetVehicleCollisionTester(collisionTester_);

    world_.system().AddConstraint(vehicleConstraint_);
    world_.system().AddStepListener(vehicleConstraint_);
    controller_ = static_cast<WheeledVehicleController*>(vehicleConstraint_->GetController());
}

PhysicsVehicle::~PhysicsVehicle() {
    if (vehicleConstraint_) {
        world_.system().RemoveStepListener(vehicleConstraint_);
        world_.system().RemoveConstraint(vehicleConstraint_);
        vehicleConstraint_ = nullptr;
    }
    if (body_) {
        BodyInterface& bodyInterface = world_.bodyInterface();
        bodyInterface.RemoveBody(bodyId_);
        bodyInterface.DestroyBody(bodyId_);
        body_ = nullptr;
    }
}

void PhysicsVehicle::applyInput(const VehicleInput& input) {
    if (!controller_) return;

    BodyInterface& bodyInterface = world_.bodyInterface();
    bodyInterface.ActivateBody(bodyId_);

    Vec3 velocity = bodyInterface.GetLinearVelocity(bodyId_);
    Quat rotation = bodyInterface.GetRotation(bodyId_);
    Vec3 forward = rotation * Vec3::sAxisZ();
    float forwardSpeed = velocity.Dot(forward);

    float throttle = input.throttle;
    if (std::abs(forwardSpeed) > settings_.maxSpeed) {
        throttle = 0.f;
    }

    float brake = input.brake ? 1.f : 0.f;
    if (throttle * forwardSpeed < -0.1f) {
        brake = std::max(brake, 1.f);
        throttle = 0.f;
    } else if (std::abs(throttle) < 0.01f && std::abs(forwardSpeed) > 0.1f) {
        brake = std::max(brake, 0.3f);
    }

    controller_->GetEngine().mMaxTorque = settings_.engineForce;
    controller_->SetDriverInput(throttle, input.steer, brake, input.handbrake ? 1.f : 0.f);
}

void PhysicsVehicle::syncVisual() {
    BodyInterface& bodyInterface = world_.bodyInterface();
    RVec3 position = bodyInterface.GetPosition(bodyId_);
    Quat rotation = bodyInterface.GetRotation(bodyId_);

    model_.group->position.set(position.GetX(), position.GetY(), position.GetZ());
    model_.group->quaternion.set(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW());

    if (!vehicleConstraint_) return;
    const Vec3 wheelUp = Vec3::sAxisX();
    for (size_t i = 0; i < model_.wheels.size(); ++i) {
        Mat44 transform = vehicleConstraint_->GetWheelLocalTransform(static_cast<uint>(i), wheelRights_[i], wheelUp);
        Vec3 t = transform.GetTranslation();
        Quat q = transform.GetQuaternion();
        auto& wheel = model_.wheels[i];
        wheel->position.set(t.GetX(), t.GetY(), t.GetZ());
        wheel->quaternion.set(q.GetX(), q.GetY(), q.GetZ(), q.GetW());
    }
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

float PhysicsVehicle::spawnHeight(VehicleType type) {
    float halfY = 0.3f;
    float wheelRadius = 0.4f;
    switch (type) {
        case VehicleType::Kart:
            halfY = 0.2f;
            wheelRadius = 0.35f;
            break;
        case VehicleType::Sedan:
            halfY = 0.3f;
            wheelRadius = 0.45f;
            break;
        case VehicleType::Truck:
            halfY = 0.4f;
            wheelRadius = 0.55f;
            break;
        default:
            break;
    }
    const float comOffset = -0.9f * halfY;
    const float suspensionRest = 0.4f;
    return wheelRadius - comOffset + suspensionRest;
}

