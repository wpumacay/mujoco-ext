#pragma once

// clang-format off
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include <mujoco_ext/common.hpp>
// clang-format on

namespace mujoco {
namespace ext {

/// Default fixed step size for the simulation internal step
constexpr double DEFAULT_FIXED_STEP_SIZE = 0.002;

class Simulation {
 public:
    using uptr = std::unique_ptr<Simulation>;

    /// Valid states the simulation can be in
    enum class eState : int32_t {
        IDLE,     /// Idle-state of the simulation (i.e. inactive, no simulation
                  /// steps allowed)
        RUNNING,  /// Running-state of the simulation (i.e. active, simulation
                  /// steps are allowed)
    };

    static auto CreateFromXmlFile(const std::string& xml_filepath)
        -> Simulation::uptr;

    static auto CreateFromXmlString(const std::string& xml_string)
        -> Simulation::uptr;

    /// Resets every MuJoCo-object to their zero-configuration (qpos0, qvel0)
    auto reset() -> void;

    /// Re-allocates the internal mjModel and mjData for a brand-new simulation
    auto reload() -> void;

    /// Steps forward the simulation by the given amount of time
    auto step(double step_size) -> void;

    auto mjc_data() -> mjData& { return *m_Data.get(); }

    auto mjc_data() const -> const mjData& { return *m_Data.get(); }

    auto mjc_model() -> mjModel& { return *m_Model.get(); }

    auto mjc_model() const -> const mjModel& { return *m_Model.get(); }

    auto fixed_timestep() const -> double { return m_FixedTimeStep; }

    auto simulation_time() const -> double { return m_SimulationTime; }

    auto num_simulation_steps() const -> int32_t { return m_NumSimSteps; }

    auto num_total_steps() const -> int32_t { return m_NumTotalSteps; }

    auto num_total_simulation_steps() const -> int32_t {
        return m_NumTotalSimSteps;
    }

    auto state() const -> eState { return m_State; }

 private:
    /// Constructs a simulation object given the required MuJoCo resources
    Simulation(mjModel_uptr model, mjData_uptr data,
               double fixed_timestep = DEFAULT_FIXED_STEP_SIZE)
        : m_Model(std::move(model)),
          m_Data(std::move(data)),
          m_FixedTimeStep(fixed_timestep),
          m_State(eState::RUNNING) {}

 private:
    /// mjModel structure used by MuJoCo to store the whole world model
    mjModel_uptr m_Model = nullptr;
    /// mjData structure used by MuJoCo to store the state of the simulation
    mjData_uptr m_Data = nullptr;
    /// Fixed step size used by the simulation (by default)
    double m_FixedTimeStep = DEFAULT_FIXED_STEP_SIZE;
    /// Current simulation time
    double m_SimulationTime = 0.0;
    /// Number of steps taken by last call internal step-function (mj_step)
    int32_t m_NumSimSteps = 0;
    /// Current number of total calls to our "step" function
    int32_t m_NumTotalSteps = 0;
    /// Current number of total simulation steps taken (calls to mj_step)
    int32_t m_NumTotalSimSteps = 0;
    /// Current state of the simulation
    eState m_State = eState::IDLE;
};

}  // namespace ext
}  // namespace mujoco
