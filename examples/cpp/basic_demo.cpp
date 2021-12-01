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

    auto simulation_resources = mujoco::ext::CreateFromFilename(filename_xml);

    auto model = std::move(simulation_resources.model);
    auto data = std::move(simulation_resources.data);
    auto scene = std::move(simulation_resources.scene);
    auto context = std::move(simulation_resources.context);

    return 0;
}
