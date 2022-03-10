#pragma once

// clang-format off
#include <glfw3.h>

#include <cstdint>
#include <memory>
#include <mujoco_ext/viewer_impl.hpp>
// clang-format on

namespace mujoco {
namespace ext {

/// Deleter functor used by unique_ptr<GLFWwindow>
struct GLFWwindowDeleter {
    /// Releases the context and window created by GLFW
    auto operator()(GLFWwindow* window_ptr) const -> void {
        if (window_ptr != nullptr) {
            glfwDestroyWindow(window_ptr);
        }
    }
};

class ViewerImplGLFW : public IViewerImpl {
 public:
    /// Unique pointer alias for this type
    using uptr = std::unique_ptr<ViewerImplGLFW>;
    /// Unique pointer alias for the GLFWwindow struct
    using GLFWwindow_uptr = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>;

    /// Creates a GLFW-based viewer of the given width and height
    explicit ViewerImplGLFW(mjModel* model, mjData* data, int32_t width,
                            int32_t height);

    /// Use only explicit constructor (no copy from others)
    ViewerImplGLFW(const ViewerImplGLFW& other) = delete;

    /// Use only explicit constructor (no move from others)
    ViewerImplGLFW(ViewerImplGLFW&& other) = delete;

    /// Don't allow to copy this type
    auto operator=(const ViewerImplGLFW& other) -> ViewerImplGLFW& = delete;

    /// Don't allow to move this type
    auto operator=(ViewerImplGLFW&& other) -> ViewerImplGLFW& = delete;

    /// Release both mjv (mujoco) and glfw resources
    ~ViewerImplGLFW() override;

 protected:
    /// Updates the internal MuJoCo scene (sync with simulation)
    auto _prepareImpl() -> void override;

    /// Renders the current scene using the internal MuJoCo GL-based renderer
    auto _renderImpl() -> void override;

 private:
    mjvOption m_Options{};

    mjvCamera m_Camera{};

    mjvScene m_Scene{};

    mjrContext m_Context{};

    mjrRect m_ViewportRect = {0, 0, 0, 0};

    GLFWwindow_uptr m_GLFWwindow = nullptr;
};

}  // namespace ext
}  // namespace mujoco
