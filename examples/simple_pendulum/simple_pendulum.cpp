#include <GLFW/glfw3.h>

#include <iostream>
#include <simple_pendulum/simple_pendulum.hpp>
#include <string>

auto mjModelDeleter::operator()(mjModel* ptr) const -> void {
    if (ptr != nullptr) {
        mj_deleteModel(ptr);
    }
}

auto mjDataDeleter::operator()(mjData* ptr) const -> void {
    if (ptr != nullptr) {
        mj_deleteData(ptr);
    }
}

auto mjvSceneDeleter::operator()(mjvScene* ptr) const -> void {
    if (ptr != nullptr) {
        mjv_freeScene(ptr);
    }
}

auto mjrContextDeleter::operator()(mjrContext* ptr) const -> void {
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

SimplePendulum::SimplePendulum() {
    // Make sure the path to our model is correct
    m_MjcFilepath = std::string(MUJOCOEXT_RESOURCES_PATH) + "pendulum.xml";

    std::cout << "filepath: " << m_MjcFilepath << '\n';

    auto* mjc_model =
        mj_loadXML(m_MjcFilepath.c_str(), nullptr, m_MjcErrorBuffer.data(),
                   static_cast<int>(m_MjcErrorBuffer.size()));

    if (mjc_model == nullptr) {
        mju_error_s(
            "SimplePendulum >> there was an error loading the model: %s",
            m_MjcErrorBuffer.data());
        return;
    }
    auto* mjc_data = mj_makeData(mjc_model);

    // Grab some information required for the interface to the simple pendulum
    m_JointHingeId = mj_name2id(mjc_model, mjOBJ_JOINT, JOINT_NAME);
    m_ActuatorHingeId = mj_name2id(mjc_model, mjOBJ_ACTUATOR, ACTUATOR_NAME);

    std::cout << "hinge-joint-id: " << m_JointHingeId << std::endl;
    std::cout << "hinge-actuator-id: " << m_ActuatorHingeId << std::endl;

    m_MjcModel = std::unique_ptr<mjModel, mjModelDeleter>(mjc_model);
    m_MjcData = std::unique_ptr<mjData, mjDataDeleter>(mjc_data);
    m_MjcScene = std::unique_ptr<mjvScene, mjvSceneDeleter>(new mjvScene());

    mjv_defaultCamera(&m_MjcCamera);
    mjv_defaultOption(&m_MjcOption);
    mjv_defaultScene(m_MjcScene.get());
    mjv_makeScene(mjc_model, m_MjcScene.get(), NUM_MAX_GEOMETRIES);

#ifndef MUJOCOEXT_BUILD_HEADLESS
    if (glfwInit() == GLFW_FALSE) {
        mju_error("Couldn't initialize GLFW");
        m_IsHeadless = true;
        return;
    }

    auto* glfw_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                         WINDOW_NAME, nullptr, nullptr);
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
    m_MjcContext =
        std::unique_ptr<mjrContext, mjrContextDeleter>(new mjrContext());
    mjr_defaultContext(m_MjcContext.get());
    mjr_makeContext(mjc_model, m_MjcContext.get(), mjFONTSCALE_150);

    glfwSetWindowUserPointer(glfw_window, this);

    glfwSetKeyCallback(glfw_window, [](GLFWwindow* window_ptr, int key,
                                       int scancode, int action, int mode) {
        auto* application =
            static_cast<SimplePendulum*>(glfwGetWindowUserPointer(window_ptr));
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
    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow* window_ptr,
                                               int button, int action,
                                               int mode) {
        auto* application =
            static_cast<SimplePendulum*>(glfwGetWindowUserPointer(window_ptr));
        // update buttons' state
        auto& mouse_state = application->GetMouseState();
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
        glfwGetCursorPos(window_ptr, &mouse_state.last_x, &mouse_state.last_y);
    });
    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow* window_ptr,
                                             double xpos, double ypos) {
        auto* application =
            static_cast<SimplePendulum*>(glfwGetWindowUserPointer(window_ptr));
        auto& mjc_model = application->model();
        auto& mjc_scene = *application->m_MjcScene;
        auto& mjc_camera = application->m_MjcCamera;
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
        mjv_moveCamera(&mjc_model, action, delta_x / height, delta_y / height,
                       &mjc_scene, &mjc_camera);
    });
    glfwSetScrollCallback(glfw_window, [](GLFWwindow* window_ptr, double xOff,
                                          double yOff) {
        auto* application =
            static_cast<SimplePendulum*>(glfwGetWindowUserPointer(window_ptr));
        auto& mjc_model = application->model();
        auto& mjc_scene = *application->m_MjcScene;
        auto& mjc_camera = application->m_MjcCamera;

        // emulate vertical mouse motion = 5% of window height
        constexpr double MOTION_SCALER = -0.05;
        mjv_moveCamera(&mjc_model, mjMOUSE_ZOOM, 0, MOTION_SCALER * yOff,
                       &mjc_scene, &mjc_camera);
    });
#endif
}

auto SimplePendulum::Step() -> void {
    mjtNum simstart = m_MjcData->time;
    while (m_MjcData->time - simstart < 1.0 / SIMULATION_FPS) {
        // @todo(wilbert): grab controls and set actuator on each internal step
        // ...
        m_MjcData->ctrl[m_ActuatorHingeId] = 0.1;
        // Step the simulation by the physics-step (0.02 default in mjcf-xml)
        mj_step(m_MjcModel.get(), m_MjcData.get());
    }
    // Render the scene (to abstract visualizer, and window if enabled)
    Render();
}

auto SimplePendulum::Render() -> void {
    // Update the abstract visualization scene (this is independent of wheter
    // or not we have a proper rendering context. We could sent draw calls even
    // remotely using RPC or similar protocol)
    mjv_updateScene(m_MjcModel.get(), m_MjcData.get(), &m_MjcOption, nullptr,
                    &m_MjcCamera, mjCAT_ALL, m_MjcScene.get());
#ifndef MUJOCOEXT_BUILD_HEADLESS
    mjrRect viewport = {0, 0, 0, 0};
    glfwGetFramebufferSize(m_Window.get(), &viewport.width, &viewport.height);
    // Render the current scene
    mjr_render(viewport, m_MjcScene.get(), m_MjcContext.get());
    // Swap buffers (blocking call due to v-sync)
    glfwSwapBuffers(m_Window.get());
    // Process pending GUI events, call GLFW callbacks
    glfwPollEvents();
#endif
}

auto SimplePendulum::Reset() -> void {
    // @todo(wilbert): implement resetting mjData fields to initial config.
}

auto SimplePendulum::GetTheta() const -> double {
    return m_MjcData->qpos[m_JointHingeId];
}

auto SimplePendulum::IsActive() const -> bool {
#ifndef MUJOCOEXT_BUILD_HEADLESS
    return !static_cast<bool>(glfwWindowShouldClose(m_Window.get()));
#else
    return true;
#endif
}

auto main() -> int {
    SimplePendulum sim;

    while (sim.IsActive()) {
        sim.Step();
        std::cout << "theta: " << sim.GetTheta() << std::endl;
    }
    return 0;
}
