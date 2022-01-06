#pragma once

#include <mujoco.h>

#include <memory>
#include <string>
#include <utility>

namespace mujoco {
namespace ext {

/// Preallocated number of geometries (max-number of geometries)
constexpr int NUM_MAX_GEOMETRIES = 2000;

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

/// All available types of visualizers
enum class eViewerType {
    NONE,
    INTERNAL_GLFW,
    INTERNAL_EGL,
    INTERNAL_OSMESA,
    EXTERNAL_MESHCAT
};

using SimResources = std::pair<mjModel_uptr, mjData_uptr>;

/// Creates the required simulation resources for the given mjcf
auto CreateFromFilename(const std::string& filename_xml) -> SimResources;

/// Creates the required simulation resources from a given mjcf string
auto CreateFromString(const std::string& string_xml) -> SimResources;

}  // namespace ext
}  // namespace mujoco
