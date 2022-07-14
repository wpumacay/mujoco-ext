#include <simple_pendulum/simple_pendulum.hpp>

auto main() -> int {
    
    SimplePendulum sim;

    while ( true ) {
        sim.Step();
        sim.Render();
    }

    return 0;
}
