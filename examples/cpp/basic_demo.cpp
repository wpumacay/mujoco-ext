#include <cstdint>
#include <iostream>
#include <mujoco_ext/simulation.hpp>
#include <mujoco_ext/viewer.hpp>
#include <string>

static constexpr int32_t VIEWPORT_WIDTH = 1024;
static constexpr int32_t VIEWPORT_HEIGHT = 768;
static constexpr int32_t MAX_STEPS = 1000;
static constexpr double STEP_SIZE = 1. / 60.;

namespace mjc_ext = mujoco::ext;

auto main(int argc, const char **argv) -> int {
    if (argc != 2) {
        std::cout << "USAGE: basic_demo PATH_TO_XML_MODEL_FILE" << '\n';
        return 1;
    }

    const std::string filename_xml = argv[1];  // NOLINT
    std::cout << "filename: " << filename_xml << '\n';

    auto sim = mjc_ext::Simulation::CreateFromXmlFile(filename_xml);

    auto viz = mjc_ext::Viewer::Create(mjc_ext::eViewerType::INTERNAL_GLFW,
                                       &sim->mjc_model(), &sim->mjc_data(),
                                       VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    while (sim->num_total_steps() < MAX_STEPS) {
        sim->step(STEP_SIZE);
        viz->render();
        std::cout << "sim-time=" << sim->simulation_time()
                  << ", sim-steps=" << sim->num_simulation_steps()
                  << ", total-sim-steps=" << sim->num_total_simulation_steps()
                  << '\n';
    }

    return 0;
}
