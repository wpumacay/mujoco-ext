
#include <mujoco_ext/simulation.hpp>

namespace mujoco {
namespace ext {

auto Simulation::CreateFromXmlFile(const std::string& xml_filepath)
    -> Simulation::uptr {
    auto sim_resources = CreateFromFilename(xml_filepath);
    auto* simulation_ptr = new Simulation(std::move(sim_resources.first),
                                          std::move(sim_resources.second));
    return Simulation::uptr(simulation_ptr);
}

auto Simulation::CreateFromXmlString(const std::string& xml_string)
    -> Simulation::uptr {
    auto sim_resources = CreateFromString(xml_string);
    auto* simulation_ptr = new Simulation(std::move(sim_resources.first),
                                          std::move(sim_resources.second));
    return Simulation::uptr(simulation_ptr);
}

auto Simulation::step(double step_size) -> void {
    auto sim_step_time =
        static_cast<mjtNum>((step_size <= 0) ? m_FixedTimeStep : step_size);
    auto sim_start_time = m_Data->time;

    m_NumSimSteps = 0;  // reset sim-step counter
    while ((m_Data->time - sim_start_time) < sim_step_time) {
        mj_step(m_Model.get(), m_Data.get());
        // Simulation-related counters go up on each iteration
        m_SimulationTime += m_FixedTimeStep;
        m_NumSimSteps++;
    }
    m_NumTotalSimSteps += m_NumSimSteps;
    m_NumTotalSteps++;
}

}  // namespace ext
}  // namespace mujoco
