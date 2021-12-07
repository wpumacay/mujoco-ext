
#include <cassert>
#include <memory>
#include <mujoco_ext/viewer.hpp>
#include <mujoco_ext/viewer_impl_glfw.hpp>

namespace mujoco {
namespace ext {

Viewer::Viewer(eViewerType type, int32_t viewport_width,
               int32_t viewport_height)
    : m_Type(type),
      m_ViewportWidth(viewport_width),
      m_ViewportHeight(viewport_height) {
    // Instantiate the impl accordingly (the ones that are not plugins)
    if (m_Type == eViewerType::INTERNAL_GLFW) {
        m_ViewerImpl = ViewerImplGLFW::Create(viewport_width, viewport_height);
    } else if (m_Type == eViewerType::INTERNAL_EGL) {
        // m_ViewerImpl = std::make_unique<ViewerImplEGL>(m_ViewportWidth,
        // m_ViewportHeight);
    } else if (m_Type == eViewerType::INTERNAL_OSMESA) {
        // m_ViewerImpl = std::make_unique<ViewerImplOSMESA>(m_ViewportWidth,
        // m_ViewportHeight);
    } else if (m_Type == eViewerType::EXTERNAL_MESHCAT) {
        // m_ViewerImpl = std::make_unique<ViewerImplMeshCat>(m_ViewportWidth,
        // m_ViewportHeight);
    }
}

auto Viewer::Create(eViewerType type, int32_t viewport_width,
                    int32_t viewport_height) -> Viewer::uptr {
    Viewer::uptr viewer;
    switch (type) {
        case eViewerType::NONE: {
            viewer = std::make_unique<Viewer>(viewport_width, viewport_height);
            break;
        }

        case eViewerType::INTERNAL_GLFW:
        case eViewerType::INTERNAL_EGL:
        case eViewerType::INTERNAL_OSMESA:
        case eViewerType::EXTERNAL_MESHCAT: {
            viewer =
                std::make_unique<Viewer>(type, viewport_width, viewport_height);
            break;
        }

            // @todo(wilbert): add cases in which we load via plugins
    }
    return std::move(viewer);
}

auto Viewer::setImpl(std::unique_ptr<IViewerImpl> impl) -> void {
    assert(impl != nullptr);  // NOLINT
    m_ViewerImpl = std::move(impl);
}

auto Viewer::render() -> void {
    if (!m_ViewerImpl) {
        return;
    }
    m_ViewerImpl->render();
}

}  // namespace ext
}  // namespace mujoco
