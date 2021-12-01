#include <mjrender.h>
#include <mujoco.h>

#include <array>
#include <cassert>
#include <mujoco_ext/common.hpp>

namespace mujoco {
namespace ext {

auto ModelDeleter::operator()(mjModel* model_ptr) const -> void {
    assert(model_ptr != nullptr);  // NOLINT
    mj_deleteModel(model_ptr);
}

auto DataDeleter::operator()(mjData* data_ptr) const -> void {
    assert(data_ptr != nullptr);  // NOLINT
    mj_deleteData(data_ptr);
}

auto SceneDeleter::operator()(mjvScene* scene_ptr) const -> void {
    assert(scene_ptr != nullptr);  // NOLINT
    mjv_freeScene(scene_ptr);
}

auto ContextDeleter::operator()(mjrContext* context_ptr) const -> void {
    assert(context_ptr != nullptr);  // NOLINT
    mjr_freeContext(context_ptr);
}

auto CreateFromFilename(const std::string& filename_xml,
                        eViewerType viewer_type) -> SimResources {
    constexpr int ERROR_BUFFER_SIZE = 1000;
    std::array<char, ERROR_BUFFER_SIZE> error_buffer_msg{};
    mjModel* model_ptr = mj_loadXML(filename_xml.c_str(), nullptr,
                                    error_buffer_msg.data(), ERROR_BUFFER_SIZE);
    if (model_ptr == nullptr) {
        mju_error("Error while loading model:");
        mju_error_s("> filename: %s", filename_xml.c_str());
        mju_error_s("> error: %s", error_buffer_msg.data());

        return {nullptr, nullptr};
    }
    mjData* data_ptr = mj_makeData(model_ptr);

    mjvScene* scene_ptr = nullptr;
    if (viewer_type != eViewerType::NONE) {
        mjv_defaultScene(scene_ptr);
        mjv_makeScene(model_ptr, scene_ptr, NUM_MAX_GEOMETRIES);
    }

    mjrContext* context_ptr = nullptr;
    if (viewer_type == eViewerType::INTERNAL_GLFW ||
        viewer_type == eViewerType::INTERNAL_EGL ||
        viewer_type == eViewerType::INTERNAL_OSMESA) {
        mjr_defaultContext(context_ptr);
        mjr_makeContext(model_ptr, context_ptr, mjFONTSCALE_150);
    }

    return {std::unique_ptr<mjModel, ModelDeleter>(model_ptr),
            std::unique_ptr<mjData, DataDeleter>(data_ptr),
            std::unique_ptr<mjvScene, SceneDeleter>(scene_ptr),
            std::unique_ptr<mjrContext, ContextDeleter>(context_ptr)};
}

}  // namespace ext
}  // namespace mujoco
