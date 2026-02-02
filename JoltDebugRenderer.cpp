#include "JoltDebugRenderer.h"

#ifdef JPH_DEBUG_RENDERER

using namespace threepp;

JoltDebugRenderer::JoltDebugRenderer() {
    group_ = Group::create();
    group_->frustumCulled = false;

    lineGeometry_ = BufferGeometry::create();
    lineMaterial_ = LineBasicMaterial::create();
    lineMaterial_->vertexColors = true;
    lineSegments_ = LineSegments::create(lineGeometry_, lineMaterial_);
    lineSegments_->frustumCulled = false;
    group_->add(lineSegments_);
}

void JoltDebugRenderer::BeginFrame() {
    linePositions_.clear();
    lineColors_.clear();
    NextFrame();
}

void JoltDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) {
    linePositions_.push_back(static_cast<float>(inFrom.GetX()));
    linePositions_.push_back(static_cast<float>(inFrom.GetY()));
    linePositions_.push_back(static_cast<float>(inFrom.GetZ()));
    linePositions_.push_back(static_cast<float>(inTo.GetX()));
    linePositions_.push_back(static_cast<float>(inTo.GetY()));
    linePositions_.push_back(static_cast<float>(inTo.GetZ()));

    float r = inColor.r / 255.f;
    float g = inColor.g / 255.f;
    float b = inColor.b / 255.f;
    lineColors_.insert(lineColors_.end(), {r, g, b, r, g, b});
}

void JoltDebugRenderer::SetCameraPosition(const Vector3& position) {
    SetCameraPos(JPH::RVec3(position.x, position.y, position.z));
}

void JoltDebugRenderer::EndFrame() {
    if (linePositions_.empty()) {
        lineSegments_->visible = false;
        return;
    }

    lineSegments_->visible = true;
    lineGeometry_->setAttribute("position", FloatBufferAttribute::create(linePositions_, 3));
    lineGeometry_->setAttribute("color", FloatBufferAttribute::create(lineColors_, 3));
    lineGeometry_->setDrawRange(0, static_cast<int>(linePositions_.size() / 3));
}

#endif
