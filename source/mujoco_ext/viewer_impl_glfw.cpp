#include <glfw3.h>
#include <mjrender.h>
#include <mujoco.h>

#include <cstddef>
#include <mujoco_ext/viewer_impl_glfw.hpp>

#include "mujoco_ext/common.hpp"

namespace mujoco {
namespace ext {

auto ViewerImplGLFW::Create(int32_t width, int32_t height)
    -> ViewerImplGLFW::uptr {
    return std::unique_ptr<ViewerImplGLFW>(new ViewerImplGLFW(width, height));
}

ViewerImplGLFW::ViewerImplGLFW(int32_t width, int32_t height) {
    // NOLINTNEXTLINE
    if (!glfwInit()) {
        return;  // @todo(wilbert): call some logging functions here
    }

    auto *window_ptr = glfwCreateWindow(
        width, height, "MuJoCo Renderer (GLFW backend)", nullptr, nullptr);
    if (window_ptr == nullptr) {
        glfwTerminate();
        return;  // @todo(wilbert): call some logging functions here
    }

    glfwMakeContextCurrent(window_ptr);
    glfwSwapInterval(1);

    mjv_defaultOption(&m_Options);
    mjv_defaultCamera(&m_Camera);
    mjv_defaultScene(&m_Scene);
    mjr_defaultContext(&m_Context);

    mjv_makeScene(m_ModelRef, &m_Scene, NUM_MAX_GEOMETRIES);
    mjr_makeContext(m_ModelRef, &m_Context, mjFONTSCALE_150);

    m_GLFWwindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(window_ptr);
}

ViewerImplGLFW::~ViewerImplGLFW() {
    mjv_freeScene(&m_Scene);
    mjr_freeContext(&m_Context);

    m_GLFWwindow = nullptr;
}

auto ViewerImplGLFW::_renderImpl() -> void {
    assert(m_GLFWwindow != nullptr);  // NOLINT
    assert(m_ModelRef != nullptr);    // NOLINT
    assert(m_DataRef != nullptr);     // NOLINT

    mjrRect viewport_rect = {0, 0, 0, 0};
    glfwGetFramebufferSize(m_GLFWwindow.get(), &viewport_rect.width,
                           &viewport_rect.height);

    mjv_updateScene(m_ModelRef, m_DataRef, &m_Options, nullptr, &m_Camera,
                    mjCAT_ALL, &m_Scene);
    mjr_render(viewport_rect, &m_Scene, &m_Context);

    glfwSwapBuffers(m_GLFWwindow.get());
    glfwPollEvents();
}

}  // namespace ext
}  // namespace mujoco
