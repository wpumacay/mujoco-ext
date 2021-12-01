#pragma once

#include <mujoco_ext/common.hpp>

namespace mujoco {
namespace ext {

/// Default fixed step size for the simulation internal step
constexpr double DEFAULT_FIXED_STEP_SIZE = 0.002;

class Simulation {
 public:
    Simulation(mjModel_uptr model, mjData_uptr data, double fixed_timestep);

    Simulation(const Simulation& rhs) = delete;

    Simulation(Simulation&& rhs) = default;

    auto operator=(const Simulation& rhs) -> Simulation& = delete;

    auto operator=(Simulation&& rhs) -> Simulation& = default;

    ~Simulation();

    auto step(double step_size) -> void;

    auto mjc_data() -> mjData& { return *m_Data.get(); }

    auto mjc_data() const -> const mjData& { return *m_Data.get(); }

    auto mjc_model() -> mjModel& { return *m_Model.get(); }

    auto mjc_model() const -> const mjModel& { return *m_Model.get(); }

    auto fixed_timestep() const -> double { return m_FixedTimeStep; }

    auto simulation_time() const -> double { return m_SimulationTime; }

 private:
    /// mjModel structure for a simulation in MuJoCo
    mjModel_uptr m_Model = nullptr;
    /// mjData structure for a simulation in MuJoCo
    mjData_uptr m_Data = nullptr;
    /// Fixed step size used by the simulation (by default)
    double m_FixedTimeStep = DEFAULT_FIXED_STEP_SIZE;
    /// Current simulation time
    double m_SimulationTime = 0.0;
};

}  // namespace ext
}  // namespace mujoco
