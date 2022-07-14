#include <iostream>
#include <string>

#include <simple_pendulum/simple_pendulum.hpp>

auto mjModelDeleter::operator()(mjModel* ptr) const -> void {
    mj_deleteModel(ptr);
}

auto mjDataDeleter::operator()(mjData* ptr) const -> void {
    mj_deleteData(ptr);
}

auto mjvSceneDeleter::operator()(mjvScene* ptr) const -> void {
    mjv_freeScene(ptr);
}

auto mjrContextDeleter::operator()(mjrContext* ptr) const -> void {
    mjr_freeContext(ptr);
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
    m_MjcContext = std::unique_ptr<mjrContext, mjrContextDeleter>( new mjrContext() );

    mjv_defaultCamera(&m_MjcCamera);
    mjv_defaultOption(&m_MjcOption);

    mjv_defaultScene(m_MjcScene.get());
    mjr_defaultContext(m_MjcContext.get());

    mjv_makeScene(mjc_model, m_MjcScene.get(), NUM_MAX_GEOMETRIES);
    mjr_makeContext(mjc_model, m_MjcContext.get(), mjFONTSCALE_150);
}

auto SimplePendulum::Step() -> void {
    mjtNum simstart = m_MjcData->time;
    while (m_MjcData->time - simstart < 1.0 / SIMULATION_FPS) {
        // @todo(wilbert): grab controls and set actuator on each internal step
        // ...

        // Step the simulation by the physics-step (0.02 default in mjcf-xml)
        mj_step(m_MjcModel.get(), m_MjcData.get());
    }
}

auto SimplePendulum::Render() -> void {
    // @todo(wilbert): implement the render step using glfw
}

auto SimplePendulum::Reset() -> void {
    // @todo(wilbert): implement resetting mjData fields to initial config.
}

auto SimplePendulum::GetTheta() const -> double {
    // @todo(wilbert): implement getting the angle from qpos and the joint id
    return 0.0;
}

