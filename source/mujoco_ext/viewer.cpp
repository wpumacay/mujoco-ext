// clang-format off
#include <mujoco_ext/viewer.hpp>
#include <mujoco_ext/viewer_impl_glfw.hpp>
//// #include <mujoco_ext/viewer_impl_egl.hpp>
//// #include <mujoco_ext/viewer_impl_osmesa.hpp>
// clang-format on

namespace mujoco {
namespace ext {

auto Viewer::Create(eViewerType type, mjModel* model, mjData* data,
                    int32_t width, int32_t height) -> Viewer::uptr {
    return std::unique_ptr<Viewer>(
        new Viewer(type, model, data, width, height));
}

Viewer::Viewer(eViewerType type, mjModel* model, mjData* data, int32_t width,
               int32_t height)
    : m_Type(type), m_ViewportWidth(width), m_ViewportHeight(height) {
    if (m_Type == eViewerType::INTERNAL_GLFW) {
        m_ViewerImpl =
            std::make_unique<ViewerImplGLFW>(model, data, width, height);
    } else if (m_Type == eViewerType::INTERNAL_EGL) {
        // @todo(wilbert): add implementation for this backend
    } else if (m_Type == eViewerType::INTERNAL_OSMESA) {
        // @todo(wilbert): add implementation for this backend
    } else if (m_Type == eViewerType::EXTERNAL_MESHCAT) {
        // @todo(wilbert): add implementation for this backend
    }
}

auto Viewer::render() -> void {
    if (m_ViewerImpl) {
        m_ViewerImpl->render();
    }
}

}  // namespace ext
}  // namespace mujoco
