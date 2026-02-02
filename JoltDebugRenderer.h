#pragma once

#include "threepp/threepp.hpp"

#ifdef JPH_DEBUG_RENDERER
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

class JoltDebugRenderer final : public JPH::DebugRendererSimple {
public:
    JoltDebugRenderer();

    void BeginFrame();

    void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    void DrawText3D(JPH::RVec3Arg, const std::string_view&, JPH::ColorArg, float) override {}

    void SetCameraPosition(const threepp::Vector3& position);

    std::shared_ptr<threepp::Group> group() const { return group_; }

private:
    std::shared_ptr<threepp::Group> group_;
};
#endif
