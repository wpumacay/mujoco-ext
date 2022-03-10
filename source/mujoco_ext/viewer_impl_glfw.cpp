#include <glfw3.h>
#include <mjrender.h>
#include <mujoco.h>

#include <cassert>
#include <cstddef>
#include <mujoco_ext/viewer_impl_glfw.hpp>

namespace mujoco {
namespace ext {

ViewerImplGLFW::ViewerImplGLFW(mjModel* model, mjData* data, int32_t width,
                               int32_t height)
    : IViewerImpl(model, data, width, height) {
    if (glfwInit() != GLFW_TRUE) {
        return;  // @todo(wilbert): call some logging functions here
    }

    auto* window_ptr = glfwCreateWindow(
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

    mjv_makeScene(mjc_model_cptr(), &m_Scene, NUM_MAX_GEOMETRIES);
    mjr_makeContext(mjc_model_cptr(), &m_Context, mjFONTSCALE_150);

    m_GLFWwindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(window_ptr);
}

ViewerImplGLFW::~ViewerImplGLFW() {
    mjv_freeScene(&m_Scene);
    mjr_freeContext(&m_Context);

    m_GLFWwindow = nullptr;
}

auto ViewerImplGLFW::_prepareImpl() -> void {
    assert(m_GLFWwindow != nullptr);      // NOLINT
    assert(mjc_model_cptr() != nullptr);  // NOLINT
    assert(mjc_data_ptr() != nullptr);    // NOLINT

    m_ViewportRect = {0, 0, 0, 0};
    glfwGetFramebufferSize(m_GLFWwindow.get(), &m_ViewportRect.width,
                           &m_ViewportRect.height);

    mjv_updateScene(mjc_model_cptr(), mjc_data_ptr(), &m_Options, nullptr,
                    &m_Camera, mjCAT_ALL, &m_Scene);

    glfwPollEvents();
}

auto ViewerImplGLFW::_renderImpl() -> void {
    mjr_render(m_ViewportRect, &m_Scene, &m_Context);

    glfwSwapBuffers(m_GLFWwindow.get());
}

}  // namespace ext
}  // namespace mujoco
