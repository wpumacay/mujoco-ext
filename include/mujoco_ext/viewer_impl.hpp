#pragma once

#include <mjmodel.h>

#include <memory>
#include <mujoco_ext/common.hpp>

/**
 * Notes:
 * - First approach is to allow these types to have a non-owning reference to
 *   both mjModel and mjData. Could use shared_ptr, but it makes more sense for
 *   the ownership to be kept by the `Simulation` type, and let this type just
 *   borrow the pointers for their own stuff
 **/

namespace mujoco {
namespace ext {

class IViewerImpl {
 public:
    /// Unique pointer alias for this type
    using uptr = std::unique_ptr<IViewerImpl>;

    /// Use only explicit constructor (no copy from others)
    IViewerImpl(const IViewerImpl& other) = delete;

    /// Use only explicit constructor (no move from others)
    IViewerImpl(IViewerImpl&& other) = delete;

    /// Don't allow to copy this type
    auto operator=(const IViewerImpl& other) -> IViewerImpl& = delete;

    /// Don't allow to move this type
    auto operator=(IViewerImpl&& other) -> IViewerImpl& = delete;

    /// Delegate the resources release to each class implementation
    virtual ~IViewerImpl() = default;

    /// Renders the current scene appropriately
    auto render() -> void;

 protected:
    /// Creates the internal adapter to link the viewer to its backend
    explicit IViewerImpl(mjModel* model, mjData* data, int32_t width,
                         int32_t height)
        : m_Model(model), m_Data(data) {}

    /// Requests to render using the internal implementation for this backend
    virtual auto _renderImpl() -> void = 0;

    /// Getter for mjModel (give access to inherited classes)
    auto mjc_model_ptr() -> mjModel* { return m_Model; }

    /// Getter for mjData (give access to inherited classes)
    auto mjc_data_ptr() -> mjData* { return m_Data; }

    /// Getter for mjModel (give access to inherited classes)
    auto mjc_model_cptr() const -> const mjModel* { return m_Model; }

    /// Getter for mjData (give access to inherited classes)
    auto mjc_data_cptr() const -> const mjData* { return m_Data; }

 private:
    /// Reference to the mjModel used by the simulation
    mjModel* m_Model = nullptr;
    /// Reference to the mjData used by the simulation
    mjData* m_Data = nullptr;
};

}  // namespace ext
}  // namespace mujoco
