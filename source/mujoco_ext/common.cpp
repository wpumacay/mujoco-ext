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

auto CreateFromFilename(const std::string& filename_xml)
    -> std::pair<mjModel_uptr, mjData_uptr> {
    // @todo(wilbert): create both mjModel and mjData in the correct order
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

    return {std::unique_ptr<mjModel, ModelDeleter>(model_ptr),
            std::unique_ptr<mjData, DataDeleter>(data_ptr)};
}

}  // namespace ext
}  // namespace mujoco
