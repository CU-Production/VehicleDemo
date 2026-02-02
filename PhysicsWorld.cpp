#include "PhysicsWorld.h"

#include <algorithm>
#include <thread>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/RegisterTypes.h>

using namespace JPH;

static constexpr uint32_t cMaxBodies = 1024;
static constexpr uint32_t cNumBodyMutexes = 0;
static constexpr uint32_t cMaxBodyPairs = 1024;
static constexpr uint32_t cMaxContactConstraints = 1024;

namespace Layers {
    static constexpr ObjectLayer NON_MOVING = PhysicsLayers::Static;
    static constexpr ObjectLayer MOVING = PhysicsLayers::Dynamic;
    static constexpr ObjectLayer NUM_LAYERS = 2;
}

class PhysicsWorld::BroadPhaseLayerInterfaceImpl final : public BroadPhaseLayerInterface {
public:
    BroadPhaseLayerInterfaceImpl() {
        objectToBroadPhase_[Layers::NON_MOVING] = BroadPhaseLayer(0);
        objectToBroadPhase_[Layers::MOVING] = BroadPhaseLayer(1);
    }

    uint GetNumBroadPhaseLayers() const override {
        return 2;
    }

    BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
        return objectToBroadPhase_[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
        switch ((BroadPhaseLayer::Type)inLayer) {
            case 0: return "NON_MOVING";
            case 1: return "MOVING";
            default: return "UNKNOWN";
        }
    }
#endif

private:
    BroadPhaseLayer objectToBroadPhase_[Layers::NUM_LAYERS];
};

class PhysicsWorld::ObjectVsBroadPhaseLayerFilterImpl final : public ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override {
        if (inLayer1 == Layers::NON_MOVING) {
            return inLayer2 == BroadPhaseLayer(1);
        }
        return true;
    }
};

class PhysicsWorld::ObjectLayerPairFilterImpl final : public ObjectLayerPairFilter {
public:
    bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override {
        if (inObject1 == Layers::NON_MOVING) {
            return inObject2 == Layers::MOVING;
        }
        return true;
    }
};

PhysicsWorld::PhysicsWorld() {
    RegisterDefaultAllocator();

    factory_ = std::make_unique<Factory>();
    Factory::sInstance = factory_.get();
    RegisterTypes();

    tempAllocator_ = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);
    const uint32_t threadCount = std::max(1u, std::thread::hardware_concurrency());
    const uint32_t workerThreads = std::max(1u, threadCount - 1);
    jobSystem_ = std::make_unique<JobSystemThreadPool>(cMaxBodies, cMaxBodies / 4, workerThreads);

    broadPhaseLayerInterface_ = std::make_unique<BroadPhaseLayerInterfaceImpl>();
    objectVsBroadPhaseLayerFilter_ = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    objectLayerPairFilter_ = std::make_unique<ObjectLayerPairFilterImpl>();

    physicsSystem_.Init(
        cMaxBodies,
        cNumBodyMutexes,
        cMaxBodyPairs,
        cMaxContactConstraints,
        *broadPhaseLayerInterface_,
        *objectVsBroadPhaseLayerFilter_,
        *objectLayerPairFilter_);

    physicsSystem_.SetGravity(Vec3(0, -9.81f, 0));
}

PhysicsWorld::~PhysicsWorld() {
    UnregisterTypes();
    Factory::sInstance = nullptr;
    factory_.reset();
}

void PhysicsWorld::step(float dt) {
    physicsSystem_.Update(dt, 1, tempAllocator_.get(), jobSystem_.get());
}

JPH::PhysicsSystem& PhysicsWorld::system() {
    return physicsSystem_;
}

JPH::BodyInterface& PhysicsWorld::bodyInterface() {
    return physicsSystem_.GetBodyInterface();
}
