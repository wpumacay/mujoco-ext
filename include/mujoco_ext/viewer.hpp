#pragma once

#include <cstdint>
#include <memory>
#include <mujoco_ext/common.hpp>
#include <mujoco_ext/viewer_impl.hpp>

namespace mujoco {
namespace ext {

/// Abstraction of a viewer, that potentially supports different backends
class Viewer {
 public:
    static constexpr int32_t DEFAULT_VIEWPORT_WIDTH = 1024;
    static constexpr int32_t DEFAULT_VIEWPORT_HEIGHT = 768;

    using uptr = std::unique_ptr<Viewer>;

    static auto Create(eViewerType type, int32_t viewport_width,
                       int32_t viewport_height) -> Viewer::uptr;

    /// Creates a viewer given its type and viewport
    explicit Viewer(eViewerType type, int32_t viewport_width,
                    int32_t viewport_height);

    /// Creates an empty viewer with some given viewport dimensions
    explicit Viewer(int32_t viewport_width, int32_t viewport_height)
        : m_ViewportWidth(viewport_width), m_ViewportHeight(viewport_height) {}

    /// Sets the impl("adapter") of the viewer
    auto setImpl(std::unique_ptr<IViewerImpl> impl) -> void;

    /// Renders the current state of the simulation
    auto render() -> void;

    /// Returns the width of the viewport
    auto width() const -> int32_t { return m_ViewportWidth; }

    /// Returns the height of the viewport
    auto height() const -> int32_t { return m_ViewportHeight; }

    /// Returns which kind of viewer we are using
    auto type() const -> eViewerType { return m_Type; }

 private:
    /// The type of viewer used internally
    eViewerType m_Type = eViewerType::NONE;
    /// The width of the viewport
    int32_t m_ViewportWidth = DEFAULT_VIEWPORT_WIDTH;
    /// The height of the viewport
    int32_t m_ViewportHeight = DEFAULT_VIEWPORT_HEIGHT;
    /// Implementation used by this viewer (we dispatch to this one)
    std::unique_ptr<IViewerImpl> m_ViewerImpl = nullptr;
};

}  // namespace ext
}  // namespace mujoco
