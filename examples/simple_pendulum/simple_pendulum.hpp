#pragma once

#include <mujoco/mujoco.h>

#include <array>
#include <string>
#include <memory>
#include <utility>

struct mjModelDeleter {
    auto operator()(mjModel* ptr) const -> void;
};

struct mjDataDeleter {
    auto operator()(mjData* ptr) const -> void;
};

struct mjvSceneDeleter {
    auto operator()(mjvScene* ptr) const -> void;
};

struct mjrContextDeleter {
    auto operator()(mjrContext* ptr) const -> void;
};


static constexpr int ERROR_BUFFER_SIZE = 100;
static constexpr int NUM_MAX_GEOMETRIES = 2000;
static constexpr mjtNum SIMULATION_FPS = static_cast<mjtNum>(60.0);

static constexpr const char* JOINT_NAME = "hinge";
static constexpr const char* ACTUATOR_NAME = "torque";
static constexpr const char* RESOURCES_PATH = MUJOCOEXT_RESOURCES_PATH;

class SimplePendulum {
public:

    SimplePendulum();

    /// Advances the simulation by a single step
    auto Step() -> void;

    /// Update the rendered scene and visualizer
    auto Render() -> void;

    /// Resets the current simulation to its initial configuration
    auto Reset() -> void;

    /// Returns the current angle of the pendulum
    auto GetTheta() const -> double;

    /// Returns a mutable reference to the mjModel of this simulation
    auto model() -> mjModel& { return *m_MjcModel.get(); }
    
    /// Returns an unmutable reference to the mjModel of this simulation
    auto model() const -> const mjModel& { return *m_MjcModel.get(); }
    
    /// Returns a mutable reference to the mjData of this simulation
    auto data() -> mjData& { return *m_MjcData.get(); }
    
    /// Returns an unmutable reference to the mjData of this simulation
    auto data() const -> const mjData& { return *m_MjcData.get(); }

private:

    mjvCamera m_MjcCamera;
    mjvOption m_MjcOption;

    std::array<char, ERROR_BUFFER_SIZE> m_MjcErrorBuffer;

    std::string m_MjcFilepath;

    std::unique_ptr<mjvScene, mjvSceneDeleter> m_MjcScene = nullptr;
    std::unique_ptr<mjrContext, mjrContextDeleter> m_MjcContext = nullptr;

    std::unique_ptr<mjModel, mjModelDeleter> m_MjcModel = nullptr;
    std::unique_ptr<mjData, mjDataDeleter> m_MjcData = nullptr;
};
