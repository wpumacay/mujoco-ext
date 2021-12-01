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

/// Destructor functor used by unique_ptr<mjvScene>
struct SceneDeleter {
    /// Releases the resources for the abstract visual MuJoCo scene
    auto operator()(mjvScene* scene_ptr) const -> void;
};

/// Destructor functor used by unique_ptr<mjrContext>
struct ContextDeleter {
    /// Releases the resources allocated by the OpenGL renderer from MuJoCo
    auto operator()(mjrContext* context_ptr) const -> void;
};

using mjModel_uptr = std::unique_ptr<mjModel, ModelDeleter>;
using mjData_uptr = std::unique_ptr<mjData, DataDeleter>;
using mjvScene_uptr = std::unique_ptr<mjvScene, SceneDeleter>;
using mjrContext_uptr = std::unique_ptr<mjrContext, ContextDeleter>;

/// All available types of visualizers
enum class eViewerType {
    NONE,
    INTERNAL_GLFW,
    INTERNAL_EGL,
    INTERNAL_OSMESA,
    EXTERNAL_MESHCAT
};

/// Resources used by the simulation once instantiated
struct SimResources {
    /// Owned mjModel structure
    mjModel_uptr model = nullptr;
    /// Owned mjData structure
    mjData_uptr data = nullptr;
    /// Owned mjvScene structure (if using a viewer)
    mjvScene_uptr scene = nullptr;
    /// Owned mjrContext structure (if using the mujoco-renderer)
    mjrContext_uptr context = nullptr;
};

/// Creates the required simulation resources for the given mjcf
auto CreateFromFilename(const std::string& filename_xml,
                        eViewerType viewer_type = eViewerType::NONE)
    -> SimResources;

}  // namespace ext
}  // namespace mujoco
