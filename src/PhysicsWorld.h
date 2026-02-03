#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <memory>

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void step(float dt);

    JPH::PhysicsSystem& system();
    JPH::BodyInterface& bodyInterface();

private:
    class BroadPhaseLayerInterfaceImpl;
    class ObjectVsBroadPhaseLayerFilterImpl;
    class ObjectLayerPairFilterImpl;

    std::unique_ptr<JPH::TempAllocator> tempAllocator_;
    std::unique_ptr<JPH::JobSystem> jobSystem_;
    std::unique_ptr<JPH::Factory> factory_;

    std::unique_ptr<BroadPhaseLayerInterfaceImpl> broadPhaseLayerInterface_;
    std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> objectVsBroadPhaseLayerFilter_;
    std::unique_ptr<ObjectLayerPairFilterImpl> objectLayerPairFilter_;

    JPH::PhysicsSystem physicsSystem_;
};

namespace PhysicsLayers {
    static constexpr JPH::ObjectLayer Static = 0;
    static constexpr JPH::ObjectLayer Dynamic = 1;
}
