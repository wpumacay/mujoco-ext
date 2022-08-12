#pragma once

#ifndef MUJOCOEXT_BUILD_HEADLESS
#include <GLFW/glfw3.h>
#endif

#include <mujoco/mujoco.h>

#include <array>
#include <memory>
#include <string>
#include <utility>

/// Deleter for mjModel (when using unique_ptr)
struct MjcModelDeleter {
    auto operator()(mjModel* ptr) const -> void;
};

/// Deleter for mjData (when using unique_ptr)
struct MjcDataDeleter {
    auto operator()(mjData* ptr) const -> void;
};

/// Deleter for mjvScene (when using unique_ptr)
struct MjvSceneDeleter {
    auto operator()(mjvScene* ptr) const -> void;
};

#ifndef MUJOCOEXT_BUILD_HEADLESS
/// Deleter for mjrContext (when using unique_ptr)
struct MjrContextDeleter {
    auto operator()(mjrContext* ptr) const -> void;
};

/// Deleter for GLFWwindow (when using unique_ptr)
struct GLFWwindowDeleter {
    auto operator()(GLFWwindow* ptr) const -> void;
};
#endif

/// Size of the error buffer used to store logging messages
static constexpr int ERROR_BUFFER_SIZE = 100;
/// Maximum number of geometries available for the simulation
static constexpr int NUM_MAX_GEOMETRIES = 2000;
/// Target FPS of the simulation
static constexpr mjtNum SIMULATION_FPS = static_cast<mjtNum>(60.0);

static constexpr int WINDOW_WIDTH = 1200;
static constexpr int WINDOW_HEIGHT = 900;
static constexpr const char* WINDOW_NAME = "Application";
static constexpr const char* RESOURCES_PATH = MUJOCOEXT_RESOURCES_PATH;

/// State representation of the cursor
struct MouseState {
    /// Whether the left button is being pressed
    bool button_left = false;
    /// Whether the middle button is being pressed
    bool button_middle = false;
    /// Whether the right button is being pressed
    bool button_right = false;
    /// The last position of the cursor (in the x-axis)
    double last_x = 0;
    /// The last position of the cursor (in the y-axis)
    double last_y = 0;
};

/// State representation of the application
struct ApplicationState {
    /// Whether or not the application should be full-screen
    bool fullscreen = false;
    /// Whether Vertical Sync is enabled or not
    bool vsync = true;
    /// Whether or not the application is running
    bool running = true;
    /// Camera used to render the view
    int camera_index = 0;
    /// Whether or not a reset has been requested
    bool dirty_reset = false;
    /// Whether the ui-framework ants to capture the mouse input
    bool wants_to_capture_mouse = false;
};

class Application {
 public:
    /// Creates an application object
    explicit Application(const char* app_name, const char* app_model);

    /// Destroys the resources allocated by this application
    virtual ~Application();

    /// Not copy constructable
    Application(const Application& rhs) = delete;

    /// Not move constructable
    Application(Application&& rhs) = delete;

    /// No copy operations allowed
    auto operator=(const Application& rhs) -> Application& = delete;

    /// No move operations allowed
    auto operator=(Application&& rhs) -> Application& = delete;

    /// Startup function to be called by the user
    auto Initialize() -> void;

    /// Advances the simulation by a single step
    auto Step() -> void;

    /// Update the rendered scene and visualizer
    auto Render() -> void;

    /// Resets the current simulation to its initial configuration
    auto Reset() -> void;

    /// Returns whether the application is still active or should close
    auto IsActive() const -> bool;

    /// Returns whether or not the current simulation is running without a
    /// visualizer
    auto IsHeadless() const -> bool { return m_IsHeadless; }

    /// Returns the current state of the mouse
    auto GetMouseState() -> MouseState& { return m_MouseState; }

    /// Returns the current state of the mouse (read-only)
    auto GetMouseState() const -> const MouseState& { return m_MouseState; }

    /// Returns the current state of the application
    auto GetApplicationState() -> ApplicationState& {
        return m_ApplicationState;
    }

    /// Returns the current state of the application (read-only)
    auto GetApplicationState() const -> const ApplicationState& {
        return m_ApplicationState;
    }

    /// Returns a mutable reference to the mjModel of this simulation
    auto model() -> mjModel& { return *m_Model; }

    /// Returns an unmutable reference to the mjModel of this simulation
    auto model() const -> const mjModel& { return *m_Model; }

    /// Returns a mutable reference to the mjData of this simulation
    auto data() -> mjData& { return *m_Data; }

    /// Returns an unmutable reference to the mjData of this simulation
    auto data() const -> const mjData& { return *m_Data; }

    /// Returns a mutable reference to the mjvScene of this simulation
    auto scene() -> mjvScene& { return *m_Scene; }

    /// Returns an unmutable reference to the mjvScene of this simulation
    auto scene() const -> const mjvScene& { return *m_Scene; }

    /// Returns a mutable reference to the mjvCamera of this simulation
    auto camera() -> mjvCamera& { return m_Camera; }

    /// Returns an unmutable reference to the mjvCamera of this simulation
    auto camera() const -> const mjvCamera& { return m_Camera; }

 protected:
    /// Render the base UI, exposing some simulation|render options
    auto _RenderUiCore() -> void;

 protected:
    /// Implementation specific initialization step
    virtual auto _InitializeInternal() -> void{};

    /// Implementation-specific simulation step
    virtual auto _SimStepInternal() -> void{};

    /// Implementation-specific rendering step
    virtual auto _RenderInternal() -> void{};

    /// Implementation-specific ui-rendering step
    virtual auto _RenderUiInternal() -> void{/* implement your own UI here*/};

 protected:
    /// Camera used to render the visualization
    mjvCamera m_Camera{};
    /// Options related to the visualiziation/scene
    mjvOption m_Option{};
    /// Buffer used to store error messages from MuJoCo
    std::array<char, ERROR_BUFFER_SIZE> m_ErrorBuffer{};
    /// Name of the application
    std::string m_Appname{};
    /// Name of the model file used for this simulation
    std::string m_Appmodel{};
    /// Path to the main model used for this simulation
    std::string m_Modelpath{};

    /// Model struct containing the simulation structure
    std::unique_ptr<mjModel, MjcModelDeleter> m_Model = nullptr;
    /// Data struct containing simulation information
    std::unique_ptr<mjData, MjcDataDeleter> m_Data = nullptr;
    /// Scene struct containing visualization information
    std::unique_ptr<mjvScene, MjvSceneDeleter> m_Scene = nullptr;
#ifndef MUJOCOEXT_BUILD_HEADLESS
    /// Context struct containing rendering information
    std::unique_ptr<mjrContext, MjrContextDeleter> m_Context = nullptr;
    /// GLFW window created for the visualizer
    std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_Window = nullptr;
    /// Current state of the cursor
    MouseState m_MouseState{};
    /// Current state of the application
    ApplicationState m_ApplicationState{};
    /// Whether or not we're running in headless mode
    bool m_IsHeadless = false;  // might be headless if can't initialize GLFW
#else
    /// Whether or not we're running in headless mode
    bool m_IsHeadless = true;  // always will be in headless
#endif
};
