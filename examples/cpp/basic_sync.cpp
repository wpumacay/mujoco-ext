// clang-format off
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

#include <mujoco_ext/simulation.hpp>
#include <mujoco_ext/viewer.hpp>
// clang-format on

static constexpr int32_t VIEWPORT_WIDTH = 1024;
static constexpr int32_t VIEWPORT_HEIGHT = 768;
static constexpr int32_t MAX_STEPS = 1000;
static constexpr double STEP_SIZE = 1. / 60.;

namespace mjc_ext = mujoco::ext;

std::mutex mtx;                               // NOLINT
mujoco::ext::Simulation::uptr sim = nullptr;  // NOLINT
mujoco::ext::Viewer::uptr viz = nullptr;      // NOLINT

void simulate() {
    while (sim->num_total_steps() < MAX_STEPS) {
        //// std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));  // NOLINT

        mtx.lock();
        sim->step(STEP_SIZE);
        mtx.unlock();
    }
}

auto main(int argc, const char **argv) -> int {
    if (argc != 2) {
        std::cout << "USAGE: basic_demo PATH_TO_XML_MODEL_FILE" << '\n';
        return 1;
    }

    const std::string filename_xml = argv[1];  // NOLINT
    std::cout << "filename: " << filename_xml << '\n';

    sim = mjc_ext::Simulation::CreateFromXmlFile(filename_xml);

    viz = mjc_ext::Viewer::Create(mjc_ext::eViewerType::INTERNAL_GLFW,
                                  &sim->mjc_model(), &sim->mjc_data(),
                                  VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    std::thread sim_thread(simulate);

    while (sim->num_total_steps() < MAX_STEPS) {
        mtx.lock();
        viz->prepare();
        std::cout << "sim-time=" << sim->simulation_time()
                  << ", sim-steps=" << sim->num_simulation_steps()
                  << ", total-sim-steps=" << sim->num_total_simulation_steps()
                  << '\n';
        mtx.unlock();

        viz->render();
    }
    sim_thread.join();

    return 0;
}
