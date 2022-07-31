#include <cart_pole/cart_pole.hpp>

CartPole::CartPole() : Application("CartPole", "cart_pole.xml") {
    m_JointHingeId = mj_name2id(&model(), mjOBJ_JOINT, JOINT_HINGE_NAME);
    m_JointSlideId = mj_name2id(&model(), mjOBJ_JOINT, JOINT_SLIDE_NAME);
    m_ActuatorSlideId = mj_name2id(&model(), mjOBJ_ACTUATOR, ACTUATOR_NAME);
}

auto CartPole::_SimStepInternal() -> void {
    data().ctrl[m_ActuatorSlideId] = m_CartForceX;  // NOLINT
}

auto CartPole::GetTheta() const -> double {
    return data().qpos[m_JointHingeId];  // NOLINT
}

auto CartPole::GetPositionX() const -> mjtNum {
    return data().qpos[m_JointSlideId];  // NOLINT
}

auto main() -> int {
    CartPole sim;

    while (sim.IsActive()) {
        sim.Step();
        sim.Render();
    }
    return 0;
}
