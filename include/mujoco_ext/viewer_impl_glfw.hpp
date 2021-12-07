#pragma once

#include <glfw3.h>

#include <cassert>
#include <cstdint>
#include <memory>
#include <mujoco_ext/viewer_impl.hpp>

namespace mujoco {
namespace ext {

/// Deleter functor used by unique_ptr<GLFWwindow>
struct GLFWwindowDeleter {
    /// Releases the context and window created by GLFW
    auto operator()(GLFWwindow* window_ptr) const -> void {
        assert(window_ptr != nullptr);  // NOLINT
        glfwDestroyWindow(window_ptr);
    }
};

class ViewerImplGLFW : public IViewerImpl {
 public:
    using uptr = std::unique_ptr<ViewerImplGLFW>;
    using GLFWwindow_uptr = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>;

    static auto Create(int32_t width, int32_t height) -> ViewerImplGLFW::uptr;

    ViewerImplGLFW(const ViewerImplGLFW& rhs) = delete;

    ViewerImplGLFW(ViewerImplGLFW&& rhs) = delete;

    auto operator=(const ViewerImplGLFW& rhs) -> ViewerImplGLFW& = delete;

    auto operator=(ViewerImplGLFW&& rhs) -> ViewerImplGLFW& = delete;

    ~ViewerImplGLFW() override;

 protected:
    auto _renderImpl() -> void override;

 private:
    ViewerImplGLFW(int32_t width, int32_t height);

 private:
    mjvOption m_Options{};

    mjvCamera m_Camera{};

    mjvScene m_Scene{};

    mjrContext m_Context{};

    GLFWwindow_uptr m_GLFWwindow = nullptr;

    mjModel* m_ModelRef = nullptr;

    mjData* m_DataRef = nullptr;
};

}  // namespace ext
}  // namespace mujoco
