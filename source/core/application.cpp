#include <core/application.hpp>
#include <iostream>

#include "GLFW/glfw3.h"

// clang-format off
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
// clang-format on

auto MjcModelDeleter::operator()(mjModel* ptr) const -> void {
    if (ptr != nullptr) {
        mj_deleteModel(ptr);
    }
}

auto MjcDataDeleter::operator()(mjData* ptr) const -> void {
    if (ptr != nullptr) {
        mj_deleteData(ptr);
    }
}

auto MjvSceneDeleter::operator()(mjvScene* ptr) const -> void {
    if (ptr != nullptr) {
        mjv_freeScene(ptr);
    }
}

#ifndef MUJOCOEXT_BUILD_HEADLESS
auto MjrContextDeleter::operator()(mjrContext* ptr) const -> void {
    if (ptr != nullptr) {
        mjr_freeContext(ptr);
    }
}

auto GLFWwindowDeleter::operator()(GLFWwindow* ptr) const -> void {
    if (ptr != nullptr) {
        glfwDestroyWindow(ptr);
        glfwTerminate();
    }
}
#endif

Application::Application(const char* app_name, const char* app_model)
    : m_Appname(app_name), m_Appmodel(app_model) {
    m_Modelpath = std::string(RESOURCES_PATH) + app_model;
    auto* mjc_model =
        mj_loadXML(m_Modelpath.c_str(), nullptr, m_ErrorBuffer.data(),
                   static_cast<int>(m_ErrorBuffer.size()));

    if (mjc_model == nullptr) {
        std::cout << "Application >> there was an error loading model ["
                  << m_Modelpath << "]" << std::endl;
        mju_error_s("Error: %s", m_ErrorBuffer.data());
        return;
    }

    auto* mjc_data = mj_makeData(mjc_model);

    m_Model = std::unique_ptr<mjModel, MjcModelDeleter>(mjc_model);
    m_Data = std::unique_ptr<mjData, MjcDataDeleter>(mjc_data);
    m_Scene = std::unique_ptr<mjvScene, MjvSceneDeleter>(new mjvScene());

    mjv_defaultCamera(&m_Camera);
    mjv_defaultOption(&m_Option);
    mjv_defaultScene(m_Scene.get());
    mjv_makeScene(mjc_model, m_Scene.get(), NUM_MAX_GEOMETRIES);

#ifndef MUJOCOEXT_BUILD_HEADLESS
    if (glfwInit() == GLFW_FALSE) {
        mju_error("Couldn't initialize GLFW");
        m_IsHeadless = true;
        return;
    }

    auto* glfw_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                         m_Appname.c_str(), nullptr, nullptr);
    if (glfw_window == nullptr) {
        mju_error("Couldn't create GLFW window");
        m_IsHeadless = true;
    }
    glfwMakeContextCurrent(glfw_window);
    glfwSwapInterval(GLFW_TRUE);
    // Keep ownership of this glfw window for later usage
    m_Window = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(glfw_window);

    // Initialize MuJoCo rendering context (we can do this step now that we
    // have a valid GLFW window and GL context properly setup)
    m_Context =
        std::unique_ptr<mjrContext, MjrContextDeleter>(new mjrContext());
    mjr_defaultContext(m_Context.get());
    mjr_makeContext(mjc_model, m_Context.get(), mjFONTSCALE_150);

    glfwSetWindowUserPointer(glfw_window, this);

    glfwSetKeyCallback(glfw_window, [](GLFWwindow* window_ptr, int key,
                                       int scancode, int action, int mode) {
        auto* application =
            static_cast<Application*>(glfwGetWindowUserPointer(window_ptr));
        auto& mjc_model = application->model();
        auto& mjc_data = application->data();
        if (action == GLFW_PRESS && key == GLFW_KEY_BACKSPACE) {
            mj_resetData(&mjc_model, &mjc_data);
            mj_forward(&mjc_model, &mjc_data);
        }
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window_ptr, GLFW_TRUE);
        }
    });
    glfwSetMouseButtonCallback(
        glfw_window,
        [](GLFWwindow* window_ptr, int button, int action, int mode) {
            auto* application =
                static_cast<Application*>(glfwGetWindowUserPointer(window_ptr));
            // update buttons' state
            auto& mouse_state = application->GetMouseState();
            const auto& app_state = application->GetApplicationState();
            if (!app_state.wants_to_capture_mouse) {
                mouse_state.button_left =
                    (glfwGetMouseButton(window_ptr, GLFW_MOUSE_BUTTON_LEFT) ==
                     GLFW_PRESS);
                mouse_state.button_middle =
                    (glfwGetMouseButton(window_ptr, GLFW_MOUSE_BUTTON_MIDDLE) ==
                     GLFW_PRESS);
                mouse_state.button_right =
                    (glfwGetMouseButton(window_ptr, GLFW_MOUSE_BUTTON_RIGHT) ==
                     GLFW_PRESS);

                // update cursor's state
                glfwGetCursorPos(window_ptr, &mouse_state.last_x,
                                 &mouse_state.last_y);
            }
        });
    glfwSetCursorPosCallback(
        glfw_window, [](GLFWwindow* window_ptr, double xpos, double ypos) {
            auto* application =
                static_cast<Application*>(glfwGetWindowUserPointer(window_ptr));
            auto& mjc_model = application->model();
            auto& mjc_scene = application->scene();
            auto& mjc_camera = application->camera();
            auto& mouse_state = application->GetMouseState();
            // no buttons down: nothing to do
            if (!mouse_state.button_left && !mouse_state.button_middle &&
                !mouse_state.button_right) {
                return;
            }

            double delta_x = xpos - mouse_state.last_x;
            double delta_y = ypos - mouse_state.last_y;
            mouse_state.last_x = xpos;
            mouse_state.last_y = ypos;

            int width{};
            int height{};
            glfwGetWindowSize(window_ptr, &width, &height);

            bool mod_shift =
                (glfwGetKey(window_ptr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                 glfwGetKey(window_ptr, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

            // determine action based on mouse button
            mjtMouse action{};
            if (mouse_state.button_right) {
                action = mod_shift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V;
            } else if (mouse_state.button_left) {
                action = mod_shift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V;
            } else {
                action = mjMOUSE_ZOOM;
            }

            // move camera
            mjv_moveCamera(&mjc_model, action, delta_x / height,
                           delta_y / height, &mjc_scene, &mjc_camera);
        });
    glfwSetScrollCallback(
        glfw_window, [](GLFWwindow* window_ptr, double xOff, double yOff) {
            auto* application =
                static_cast<Application*>(glfwGetWindowUserPointer(window_ptr));
            auto& mjc_model = application->model();
            auto& mjc_scene = application->scene();
            auto& mjc_camera = application->camera();

            // emulate vertical mouse motion = 5% of window height
            constexpr double MOTION_SCALER = -0.05;
            mjv_moveCamera(&mjc_model, mjMOUSE_ZOOM, 0, MOTION_SCALER * yOff,
                           &mjc_scene, &mjc_camera);
        });

    // --------------------------------
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imgui_io = ImGui::GetIO();
    (void)imgui_io;

    // --------------------------------
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // --------------------------------
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_Window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
#endif
}

Application::~Application() {
#ifndef MUJOCOEXT_BUILD_HEADLESS
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif
}

auto Application::Initialize() -> void {
    /// Call custom user-defined initialization routine
    _InitializeInternal();
}

auto Application::Step() -> void {
    if (!m_ApplicationState.running) {
        return;
    }

    if (m_ApplicationState.dirty_reset) {
        m_ApplicationState.dirty_reset = false;
        Reset();
        return;
    }

    mjtNum sim_start = m_Data->time;
    while (m_Data->time - sim_start < 1.0 / SIMULATION_FPS) {
        // Apply controller and set control commands
        _SimStepInternal();
        // Take a step in the simulation
        mj_step(m_Model.get(), m_Data.get());
    }
}

auto Application::Render() -> void {
#ifndef MUJOCOEXT_BUILD_HEADLESS
    // Process pending GUI events, call GLFW callbacks
    glfwPollEvents();
#endif
    // Update the abstract visualization scene (this is independent of wheter
    // or not we have a proper rendering context. We could sent draw calls even
    // remotely using RPC or similar protocol)
    mjv_updateScene(m_Model.get(), m_Data.get(), &m_Option, nullptr, &m_Camera,
                    mjCAT_ALL, m_Scene.get());

#ifndef MUJOCOEXT_BUILD_HEADLESS
    // Call user's custom render steps
    _RenderInternal();

    // --------------------------------
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // Reset ui-capture flag
    m_ApplicationState.wants_to_capture_mouse = false;
    // Render the core stuff related to the simulation
    _RenderUiCore();
    // Call user's custom ui-render steps
    _RenderUiInternal();
    // --------------------------------

    // Draw all data generated by ImGui calls
    ImGui::Render();
    // Get the state of the ui-capture flag
    ImGuiIO& imgui_io = ImGui::GetIO();
    m_ApplicationState.wants_to_capture_mouse = imgui_io.WantCaptureMouse;

    // Prepare for the actual rendering
    mjrRect viewport = {0, 0, 0, 0};
    glfwGetFramebufferSize(m_Window.get(), &viewport.width, &viewport.height);
    // Render the current scene
    mjr_render(viewport, m_Scene.get(), m_Context.get());
    // Render all ui-elements
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Swap buffers (blocking call due to v-sync)
    glfwSwapBuffers(m_Window.get());
#endif
}

auto Application::_RenderUiCore() -> void {
    auto& app_state = m_ApplicationState;
    // --------------------------------
    // Show the core state of the application
    ImGui::Begin("Application");
    if (ImGui::CollapsingHeader("Simulation")) {
        ImGui::Checkbox("Running", &app_state.running);
        if (ImGui::Button("Reset")) {
            app_state.dirty_reset = true;
        }
    }
    if (ImGui::CollapsingHeader("Rendering")) {
        // Check vsync property
        bool old_vsync = app_state.vsync;
        ImGui::Checkbox("Vsync", &app_state.vsync);
        if (old_vsync != app_state.vsync) {
            glfwSwapInterval(app_state.vsync ? GLFW_TRUE : GLFW_FALSE);
        }
    }
    ImGui::End();
}

auto Application::Reset() -> void {
    mj_resetData(m_Model.get(), m_Data.get());
    mj_forward(m_Model.get(), m_Data.get());
}

auto Application::IsActive() const -> bool {
#ifndef MUJOCOEXT_BUILD_HEADLESS
    return !static_cast<bool>(glfwWindowShouldClose(m_Window.get()));
#else
    return true;
#endif
}
