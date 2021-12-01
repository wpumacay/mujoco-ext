#pragma once

#include <mujoco.h>

#include <memory>
#include <string>
#include <utility>

namespace mujoco {
namespace ext {

/// Desctructor functor used by unique_ptr<mjModel>
struct ModelDeleter {
    /// Releases the resources used by the given mjModel pointer
    auto operator()(mjModel* model_ptr) const -> void;
};

/// Desctructor functor used by unique_ptr<mjData>
struct DataDeleter {
    /// Releases the resources used by the given mjData pointer
    auto operator()(mjData* data_ptr) const -> void;
};

using mjModel_uptr = std::unique_ptr<mjModel, ModelDeleter>;
using mjData_uptr = std::unique_ptr<mjData, DataDeleter>;

auto CreateFromFilename(const std::string& filename_xml)
    -> std::pair<mjModel_uptr, mjData_uptr>;

}  // namespace ext
}  // namespace mujoco
