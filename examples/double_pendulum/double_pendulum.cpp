#include <double_pendulum/double_pendulum.hpp>

DoublePendulum::DoublePendulum()
    : Application("Double Pendulum", "double_pendulum.xml") {
    m_Joint1HingeId = mj_name2id(&model(), mjOBJ_JOINT, JOINT_1_NAME);
    m_Joint1HingeId = mj_name2id(&model(), mjOBJ_JOINT, JOINT_2_NAME);
    m_Actuator1HingeId = mj_name2id(&model(), mjOBJ_ACTUATOR, ACTUATOR_1_NAME);
    m_Actuator2HingeId = mj_name2id(&model(), mjOBJ_ACTUATOR, ACTUATOR_2_NAME);
}

auto DoublePendulum::_SimStepInternal() -> void {
    data().ctrl[m_Actuator1HingeId] = m_Actuator1Active ? m_Torque1Ctrl : 0.0; // NOLINT
    data().ctrl[m_Actuator2HingeId] = m_Actuator2Active ? m_Torque2Ctrl : 0.0; // NOLINT
}

auto DoublePendulum::GetTheta1() const -> double {
    return data().qpos[m_Joint1HingeId];  // NOLINT
}

auto DoublePendulum::GetTheta2() const -> double {
    return data().qpos[m_Joint2HingeId];  // NOLINT
}

auto main() -> int {
    DoublePendulum sim;

    while (sim.IsActive()) {
        sim.Step();
        sim.Render();
    }
    return 0;
}
