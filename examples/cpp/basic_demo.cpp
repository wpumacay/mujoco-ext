#include <iostream>
#include <mujoco_ext/common.hpp>
#include <string>

auto main(int argc, const char** argv) -> int {
    if (argc != 2) {
        std::cout << "USAGE: basic_demo PATH_TO_XML_MODEL_FILE" << '\n';
        return 1;
    }

    const std::string filename_xml = argv[1];  // NOLINT
    std::cout << "filename: " << filename_xml << '\n';

    // @todo(wilbert): 1st option for the API
    // auto [sim, viewer] = mujoco::ext::CreateSimulation(ARGUMENTS_HERE);
    // viewer->registerKeyCallback(LAMBDA_BIND_SIM); // ESC -> sim->stop()

    // @todo(wilbert): 2nd option for the API
    // auto sim = mujoco::ext::CreateSimulation(SIMULATION_ARGUMENTS);
    // auto viewer = mujoco::ext::CreateViewer(sim.get(), VIEWER_ARGUMENTS);
    // viewer->registerKeyCallback(LAMBDA_BIND_SIM); // ESC -> sim->stop()

    // while(sim->running()) {}
    //      sim->step(STEP_SIZE);
    //      viewer->render();
    // }

    // @todo(wilbert): CreateFromXmlFile should return [sim, viewer]
    auto simulation_resources = mujoco::ext::CreateFromFilename(filename_xml);

    // @todo(wilbert): CreateFromXmlString should return [sim, viewer]

    // @todo(wilbert): CreateFromWorld(world) returns [sim, viewer], where world
    // is a programmatically created representation of the world (no mjcf-xml)

    auto model = std::move(simulation_resources.model);
    auto data = std::move(simulation_resources.data);

    return 0;
}
