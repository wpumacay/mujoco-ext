#pragma once

#include <GLFW/glfw3.h>
#include <mujoco/mujoco.h>

#include <array>
#include <memory>
#include <string>
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

struct GLFWwindowDeleter {
    auto operator()(GLFWwindow* ptr) const -> void;
};

static constexpr int ERROR_BUFFER_SIZE = 100;
static constexpr int NUM_MAX_GEOMETRIES = 2000;
static constexpr mjtNum SIMULATION_FPS = static_cast<mjtNum>(60.0);

static constexpr const char* JOINT_NAME = "hinge";
static constexpr const char* ACTUATOR_NAME = "torque";
static constexpr const char* RESOURCES_PATH = MUJOCOEXT_RESOURCES_PATH;

// GLFW related constants
static constexpr int WINDOW_WIDTH = 1200;
static constexpr int WINDOW_HEIGHT = 900;
static constexpr const char* WINDOW_NAME = "Simple Pendulum Application";

struct MouseState {
    bool button_left = false;
    bool button_middle = false;
    bool button_right = false;
    double last_x = 0;
    double last_y = 0;
};

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

    /// Returns whether the application is still active or should close
    auto IsActive() const -> bool;

    /// Returns the current state of the mouse
    auto GetMouseState() -> MouseState& { return m_MouseState; }

    /// Returns the current state of the mouse (read-only)
    auto GetMouseState() const -> const MouseState& { return m_MouseState; }

    /// Returns a mutable reference to the mjModel of this simulation
    auto model() -> mjModel& { return *m_MjcModel; }

    /// Returns an unmutable reference to the mjModel of this simulation
    auto model() const -> const mjModel& { return *m_MjcModel; }

    /// Returns a mutable reference to the mjData of this simulation
    auto data() -> mjData& { return *m_MjcData; }

    /// Returns an unmutable reference to the mjData of this simulation
    auto data() const -> const mjData& { return *m_MjcData; }

 private:
    mjvCamera m_MjcCamera{};
    mjvOption m_MjcOption{};

    int m_JointHingeId{-1};
    int m_ActuatorHingeId{-1};

    std::array<char, ERROR_BUFFER_SIZE> m_MjcErrorBuffer{};

    std::string m_MjcFilepath;

    std::unique_ptr<mjvScene, mjvSceneDeleter> m_MjcScene = nullptr;
    std::unique_ptr<mjrContext, mjrContextDeleter> m_MjcContext = nullptr;

    std::unique_ptr<mjModel, mjModelDeleter> m_MjcModel = nullptr;
    std::unique_ptr<mjData, mjDataDeleter> m_MjcData = nullptr;

    MouseState m_MouseState{};

#ifdef MUJOCOEXT_BUILD_HEADLESS
    /// Whether or not we're running in headless mode
    bool m_IsHeadless = true;  // always will be headless
#else
    /// GLFW window created for the visualizer
    std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_Window = nullptr;
    /// Whether or not we're running in headless mode
    bool m_IsHeadless = false;  // might be headless if can't initialize GLFW
#endif
};
