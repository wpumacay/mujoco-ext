#include <mujoco_ext/viewer_impl.hpp>

namespace mujoco {
namespace ext {

auto IViewerImpl::prepare() -> void { _prepareImpl(); }

auto IViewerImpl::render() -> void { _renderImpl(); }

}  // namespace ext
}  // namespace mujoco
