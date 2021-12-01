#include <mujoco.h>

#include <mujoco_ext/simulation.hpp>

namespace mujoco {
namespace ext {

Simulation::Simulation(mjModel_uptr model, mjData_uptr data,
                       double fixed_timestep)
    : m_Model(std::move(model)),
      m_Data(std::move(data)),
      m_FixedTimeStep(fixed_timestep) {}

Simulation::~Simulation() {
    m_Data = nullptr;
    m_Model = nullptr;
}

auto Simulation::step(double step_size) -> void {
    auto sim_step_time =
        static_cast<mjtNum>((step_size <= 0) ? m_FixedTimeStep : step_size);
    auto sim_start_time = m_Data->time;

    while ((m_Data->time - sim_start_time) < sim_step_time) {
        mj_step(m_Model.get(), m_Data.get());
        m_SimulationTime += m_FixedTimeStep;
    }
}

}  // namespace ext
}  // namespace mujoco
