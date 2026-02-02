#include "JoltDebugRenderer.h"

#ifdef JPH_DEBUG_RENDERER

using namespace threepp;

JoltDebugRenderer::JoltDebugRenderer() {
    group_ = Group::create();
    group_->frustumCulled = false;
}

void JoltDebugRenderer::BeginFrame() {
    group_->clear();
    NextFrame();
}

void JoltDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) {
    auto geometry = BufferGeometry::create();
    std::vector<float> positions = {
        static_cast<float>(inFrom.GetX()), static_cast<float>(inFrom.GetY()), static_cast<float>(inFrom.GetZ()),
        static_cast<float>(inTo.GetX()), static_cast<float>(inTo.GetY()), static_cast<float>(inTo.GetZ())
    };

    geometry->setAttribute("position", FloatBufferAttribute::create(positions, 3));

    auto material = LineBasicMaterial::create();
    material->color.setRGB(inColor.r / 255.f, inColor.g / 255.f, inColor.b / 255.f);

    auto line = LineSegments::create(geometry, material);
    line->frustumCulled = false;
    group_->add(line);
}

void JoltDebugRenderer::SetCameraPosition(const Vector3& position) {
    SetCameraPos(JPH::RVec3(position.x, position.y, position.z));
}

#endif
