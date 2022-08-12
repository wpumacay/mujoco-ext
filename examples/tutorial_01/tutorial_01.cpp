#include <tutorial_01/tutorial_01.hpp>

Tutorial01::Tutorial01() : Application("Primitives", "tutorial_01.xml") {}

auto main() -> int {
    Tutorial01 sim;
    sim.Initialize();

    while (sim.IsActive()) {
        sim.Step();
        sim.Render();
    }
    return 0;
}
