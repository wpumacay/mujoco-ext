#include <simple_pendulum/simple_pendulum.hpp>

SimplePendulum::SimplePendulum()
    : Application("Simple Pendulum", "pendulum.xml") {
    // NOLINTNEXTLINE
    m_JointHingeId = mj_name2id(&model(), mjOBJ_JOINT, JOINT_NAME);
    // NOLINTNEXTLINE
    m_ActuatorHingeId = mj_name2id(&model(), mjOBJ_ACTUATOR, ACTUATOR_NAME);
}

auto SimplePendulum::_SimStepInternal() -> void {
    constexpr double BASE_CONTROL_COMMAND = 0.1;
    data().ctrl[m_ActuatorHingeId] = BASE_CONTROL_COMMAND;  // NOLINT
}

auto SimplePendulum::GetTheta() const -> double {
    return data().qpos[m_JointHingeId];  // NOLINT
}

auto main() -> int {
    SimplePendulum sim;

    while (sim.IsActive()) {
        sim.Step();
        sim.Render();
    }
    return 0;
}
