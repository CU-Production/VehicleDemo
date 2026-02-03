#include "JoltDebugRenderer.h"

#ifdef JPH_DEBUG_RENDERER

using namespace threepp;

JoltDebugRenderer::JoltDebugRenderer() {
    group_ = Group::create();
    group_->frustumCulled = false;

    const size_t threadCount = std::max(1u, std::thread::hardware_concurrency());
    threadBuffers_.resize(threadCount * 2);

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
    for (auto& buffer : threadBuffers_) {
        buffer.positions.clear();
        buffer.colors.clear();
    }
    linePositions_.clear();
    lineColors_.clear();
    NextFrame();
}

void JoltDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) {
    static thread_local bool tlsAssigned = false;
    static thread_local size_t tlsIndex = 0;
    if (!tlsAssigned) {
        tlsIndex = nextThreadIndex_.fetch_add(1, std::memory_order_relaxed);
        if (tlsIndex >= threadBuffers_.size()) {
            tlsIndex = threadBuffers_.size() - 1;
        }
        tlsAssigned = true;
    }

    auto& buffer = threadBuffers_[tlsIndex];
    buffer.positions.push_back(static_cast<float>(inFrom.GetX()));
    buffer.positions.push_back(static_cast<float>(inFrom.GetY()));
    buffer.positions.push_back(static_cast<float>(inFrom.GetZ()));
    buffer.positions.push_back(static_cast<float>(inTo.GetX()));
    buffer.positions.push_back(static_cast<float>(inTo.GetY()));
    buffer.positions.push_back(static_cast<float>(inTo.GetZ()));

    float r = inColor.r / 255.f;
    float g = inColor.g / 255.f;
    float b = inColor.b / 255.f;
    buffer.colors.insert(buffer.colors.end(), {r, g, b, r, g, b});
}

void JoltDebugRenderer::SetCameraPosition(const Vector3& position) {
    SetCameraPos(JPH::RVec3(position.x, position.y, position.z));
}

void JoltDebugRenderer::EndFrame() {
    for (auto& buffer : threadBuffers_) {
        linePositions_.insert(linePositions_.end(), buffer.positions.begin(), buffer.positions.end());
        lineColors_.insert(lineColors_.end(), buffer.colors.begin(), buffer.colors.end());
    }

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
