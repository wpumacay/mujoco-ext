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

    auto simulation_data = mujoco::ext::CreateFromFilename(filename_xml);

    auto model = std::move(simulation_data.first);
    auto data = std::move(simulation_data.second);

    return 0;
}
