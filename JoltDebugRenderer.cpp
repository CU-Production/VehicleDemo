#include "JoltDebugRenderer.h"

#ifdef JPH_DEBUG_RENDERER

using namespace threepp;

JoltDebugRenderer::JoltDebugRenderer() {
    group_ = Group::create();
    group_->frustumCulled = false;

    lineGeometry_ = BufferGeometry::create();
    lineMaterial_ = LineBasicMaterial::create();
    lineMaterial_->vertexColors = true;

    maxVertices_ = 4096;
    positionAttr_ = FloatBufferAttribute::create(std::vector<float>(maxVertices_ * 3, 0.f), 3);
    positionAttr_->setUsage(DrawUsage::Dynamic);
    colorAttr_ = FloatBufferAttribute::create(std::vector<float>(maxVertices_ * 3, 0.f), 3);
    colorAttr_->setUsage(DrawUsage::Dynamic);
    lineGeometry_->setAttribute("position", positionAttr_);
    lineGeometry_->setAttribute("color", colorAttr_);
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
    const size_t vertexCount = linePositions_.size() / 3;
    if (vertexCount > maxVertices_) {
        maxVertices_ = std::max(vertexCount, maxVertices_ * 2);
        positionAttr_ = FloatBufferAttribute::create(std::vector<float>(maxVertices_ * 3, 0.f), 3);
        positionAttr_->setUsage(DrawUsage::Dynamic);
        colorAttr_ = FloatBufferAttribute::create(std::vector<float>(maxVertices_ * 3, 0.f), 3);
        colorAttr_->setUsage(DrawUsage::Dynamic);
        lineGeometry_->setAttribute("position", positionAttr_);
        lineGeometry_->setAttribute("color", colorAttr_);
    }

    auto& posArray = positionAttr_->array();
    auto& colArray = colorAttr_->array();
    std::copy(linePositions_.begin(), linePositions_.end(), posArray.begin());
    std::copy(lineColors_.begin(), lineColors_.end(), colArray.begin());
    positionAttr_->needsUpdate();
    colorAttr_->needsUpdate();

    lineGeometry_->setDrawRange(0, static_cast<int>(vertexCount));
}

#endif
