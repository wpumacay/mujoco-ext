#include <iostream>
#include <string>

#include <simple_pendulum/simple_pendulum.hpp>

auto mjModelDeleter::operator()(mjModel* ptr) const -> void {
    if (ptr) {
        mj_deleteModel(ptr);
    }
}

auto mjDataDeleter::operator()(mjData* ptr) const -> void {
    if (ptr) {
        mj_deleteData(ptr);
    }
}

auto mjvSceneDeleter::operator()(mjvScene* ptr) const -> void {
    if (ptr) {
        mjv_freeScene(ptr);
    }
}

auto mjrContextDeleter::operator()(mjrContext* ptr) const -> void {
    if (ptr) {
        mjr_freeContext(ptr);
    }
}

auto GLFWwindowDeleter::operator()(GLFWwindow* ptr) const -> void {
    if (ptr) {
        glfwDestroyWindow(ptr);
        glfwTerminate();
    }
}

SimplePendulum::SimplePendulum() {
    // Make sure the path to our model is correct
    m_MjcFilepath = std::string(MUJOCOEXT_RESOURCES_PATH) + "pendulum.xml";

    std::cout << "filepath: " << m_MjcFilepath << '\n';

    auto mjc_model = mj_loadXML(m_MjcFilepath.c_str(), NULL,
                                m_MjcErrorBuffer.data(), m_MjcErrorBuffer.size());

    if (!mjc_model) {
        mju_error_s("SimplePendulum >> there was an error loading the model: %s", m_MjcErrorBuffer.data());
        return;
    }
    auto mjc_data = mj_makeData(mjc_model);

    m_MjcModel = std::unique_ptr<mjModel, mjModelDeleter>(mjc_model);
    m_MjcData = std::unique_ptr<mjData, mjDataDeleter>(mjc_data);
    m_MjcScene = std::unique_ptr<mjvScene, mjvSceneDeleter>( new mjvScene() );

    mjv_defaultCamera(&m_MjcCamera);
    mjv_defaultOption(&m_MjcOption);
    mjv_defaultScene(m_MjcScene.get());
    mjv_makeScene(mjc_model, m_MjcScene.get(), NUM_MAX_GEOMETRIES);

#ifndef MUJOCOEXT_BUILD_HEADLESS
    if (!glfwInit()) {
        mju_error("Couldn't initialize GLFW");
        m_IsHeadless = true;
        return;
    }

    auto glfw_window = glfwCreateWindow(WINDOW_WIDTH,
                                        WINDOW_HEIGHT,
                                        WINDOW_NAME, NULL, NULL);
    if (!glfw_window) {
        mju_error("Couldn't create GLFW window");
        m_IsHeadless = true;
    }
    glfwMakeContextCurrent(glfw_window);
    glfwSwapInterval(GLFW_TRUE);
    // Keep ownership of this glfw window for later usage
    m_Window = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(glfw_window);

    // Initialize MuJoCo rendering context (we can do this step now that we
    // have a valid GLFW window and GL context properly setup)
    m_MjcContext = std::unique_ptr<mjrContext, mjrContextDeleter>( new mjrContext() );
    mjr_defaultContext(m_MjcContext.get());
    mjr_makeContext(mjc_model, m_MjcContext.get(), mjFONTSCALE_150);
#endif
}

auto SimplePendulum::Step() -> void {
    mjtNum simstart = m_MjcData->time;
    while (m_MjcData->time - simstart < 1.0 / SIMULATION_FPS) {
        // @todo(wilbert): grab controls and set actuator on each internal step
        // ...

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
    mjv_updateScene(m_MjcModel.get(), m_MjcData.get(), &m_MjcOption, NULL,
                    &m_MjcCamera, mjCAT_ALL, m_MjcScene.get());
#ifndef MUJOCOEXT_BUILD_HEADLESS
    mjrRect viewport = {0, 0, 0, 0};
    glfwGetFramebufferSize(m_Window.get(), &viewport.width, &viewport.height);
    // Render the current scene
    mjr_render(viewport, m_MjcScene.get(), m_MjcContext.get());
    // Swap buffers (blocking call due to v-sync)
    glfwSwapBuffers(m_Window.get());
#endif
}

auto SimplePendulum::Reset() -> void {
    // @todo(wilbert): implement resetting mjData fields to initial config.
}

auto SimplePendulum::GetTheta() const -> double {
    // @todo(wilbert): implement getting the angle from qpos and the joint id
    return 0.0;
}

auto main() -> int {
    SimplePendulum sim;

    while( true ) {
        sim.Step();
    }
    return 0;
}
