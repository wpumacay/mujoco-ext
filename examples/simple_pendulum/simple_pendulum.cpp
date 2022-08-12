#include <imgui.h>

#include <simple_pendulum/simple_pendulum.hpp>

/// The torque applied to the joint of the pendulum
static float s_torque = 0.0F;  // NOLINT

struct PendulumSettings {
    bool ctrl_limited = true;
    float ctrl_range_min = -1.0;
    float ctrl_range_max = 1.0;
    float damping = 0.1;  // NOLINT
};

static PendulumSettings s_settings;  // NOLINT

SimplePendulum::SimplePendulum()
    : Application("Simple Pendulum", "simple_pendulum.xml") {
    // NOLINTNEXTLINE
    m_JointHingeId = mj_name2id(&model(), mjOBJ_JOINT, JOINT_NAME);
    // NOLINTNEXTLINE
    m_ActuatorHingeId = mj_name2id(&model(), mjOBJ_ACTUATOR, ACTUATOR_NAME);
    // NOLINTNEXTLINE
    m_SensorJntPos.id = mj_name2id(&model(), mjOBJ_SENSOR, SENSOR_JNTPOS);
    // NOLINTNEXTLINE
    m_SensorJntVel.id = mj_name2id(&model(), mjOBJ_SENSOR, SENSOR_JNTVEL);
    //  Get the address|index|offset for this joint (both qpos and qvel
    //  buffers)
    m_JntAdr = model().jnt_qposadr[m_JointHingeId];  // NOLINT
    m_DofAdr = model().jnt_dofadr[m_JointHingeId];   // NOLINT
    // Get the address|index|offset for this joint's sensors
    m_SensorJntPos.adr = model().sensor_adr[m_SensorJntPos.id];  // NOLINT
    m_SensorJntVel.adr = model().sensor_adr[m_SensorJntVel.id];  // NOLINT
    // Get the ctrl-limits from the actuator's section of the mjModel
    s_settings.ctrl_range_min =
        model().actuator_ctrlrange[2 * m_ActuatorHingeId + 0];  // NOLINT
    s_settings.ctrl_range_max =
        model().actuator_ctrlrange[2 * m_ActuatorHingeId + 1];  // NOLINT
    s_settings.ctrl_limited =
        model().actuator_ctrllimited[m_ActuatorHingeId] == 1;  // NOLINT
    s_settings.damping = model().dof_damping[m_DofAdr];        // NOLINT
}

auto SimplePendulum::_RenderUiInternal() -> void {
    ImGui::Begin("Simple Pendulum");
    if (ImGui::CollapsingHeader("Controls")) {
        if (s_settings.ctrl_limited) {
            ImGui::SliderFloat("Torque", &s_torque, s_settings.ctrl_range_min,
                               s_settings.ctrl_range_max);
        } else {
            ImGui::InputFloat("Torque", &s_torque);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear torque")) {
            s_torque = 0.0F;
        }
    }
    if (ImGui::CollapsingHeader("Properties")) {
        auto old_damping = s_settings.damping;
        ImGui::SliderFloat("Damping", &s_settings.damping, 0.0F,
                           10.0F);  // NOLINT
        if (old_damping != s_settings.damping) {
            model().dof_damping[m_DofAdr] = s_settings.damping;  // NOLINT
        }
    }
    if (ImGui::CollapsingHeader("Sensors")) {
        ImGui::Text("Joint-qpos: %.3f", m_SensorJntPos.value);  // NOLINT
        ImGui::Text("Joint-qvel: %.3f", m_SensorJntVel.value);  // NOLINT
    }
    ImGui::End();
}

auto SimplePendulum::_SimStepInternal() -> void {
    data().ctrl[m_ActuatorHingeId] = static_cast<mjtNum>(s_torque);  // NOLINT
    // Get the sensor values
    m_SensorJntPos.value = data().sensordata[m_SensorJntPos.adr];  // NOLINT
    m_SensorJntVel.value = data().sensordata[m_SensorJntVel.adr];  // NOLINT
}

auto SimplePendulum::GetTheta() const -> double {
    return data().qpos[m_JointHingeId];  // NOLINT
}

auto main() -> int {
    SimplePendulum sim;
    sim.Initialize();

    while (sim.IsActive()) {
        sim.Step();
        sim.Render();
    }
    return 0;
}
