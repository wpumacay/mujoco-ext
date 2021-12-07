#pragma once

#include <cstdint>
#include <mujoco_ext/common.hpp>

namespace mujoco {
namespace ext {

class IViewerImpl {
 public:
    IViewerImpl() = default;

    IViewerImpl(const IViewerImpl& rhs) = delete;

    IViewerImpl(IViewerImpl&& rhs) = delete;

    auto operator=(const IViewerImpl& rhs) -> IViewerImpl& = delete;

    auto operator=(IViewerImpl&& rhs) -> IViewerImpl& = delete;

    virtual ~IViewerImpl() = default;

    auto render() -> void;

 protected:
    virtual auto _renderImpl() -> void = 0;
};

}  // namespace ext
}  // namespace mujoco
