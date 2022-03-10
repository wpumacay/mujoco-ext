#pragma once

// clang-format off
#include <cstdint>
#include <memory>
#include <mujoco_ext/common.hpp>
#include <mujoco_ext/viewer_impl.hpp>
// clang-format on

namespace mujoco {
namespace ext {

/// Abstraction of a viewer, that potentially supports different backends
class Viewer {
 public:
    static constexpr int32_t DEFAULT_VIEWPORT_WIDTH = 1024;
    static constexpr int32_t DEFAULT_VIEWPORT_HEIGHT = 768;

    /// Unique pointer alias for this type
    using uptr = std::unique_ptr<Viewer>;

    /// Factory function that creates a viewer with given parameters
    static auto Create(eViewerType type, mjModel* model, mjData* data,
                       int32_t width = DEFAULT_VIEWPORT_WIDTH,
                       int32_t height = DEFAULT_VIEWPORT_HEIGHT)
        -> Viewer::uptr;

    /// Prepares|Syncs the simulation and the viewer internal states
    auto prepare() -> void;

    /// Renders the current state of the simulation
    auto render() -> void;

    /// Returns the width of the viewport
    auto width() const -> int32_t { return m_ViewportWidth; }

    /// Returns the height of the viewport
    auto height() const -> int32_t { return m_ViewportHeight; }

    /// Returns which kind of viewer we are using
    auto type() const -> eViewerType { return m_Type; }

 private:
    /// Creates a viewer object of a given type
    explicit Viewer(eViewerType type, mjModel* model, mjData* data,
                    int32_t width, int32_t height);

 private:
    /// The type of viewer used internally
    eViewerType m_Type = eViewerType::NONE;
    /// The width of the viewport
    int32_t m_ViewportWidth = DEFAULT_VIEWPORT_WIDTH;
    /// The height of the viewport
    int32_t m_ViewportHeight = DEFAULT_VIEWPORT_HEIGHT;
    /// Implementation used by this viewer (we dispatch to this one)
    IViewerImpl::uptr m_ViewerImpl = nullptr;
    /// Reference to the mjModel used by the simulation
    mjModel* m_Model = nullptr;
    /// Reference to the mjData used by the simulation
    mjData* m_Data = nullptr;
};

}  // namespace ext
}  // namespace mujoco
