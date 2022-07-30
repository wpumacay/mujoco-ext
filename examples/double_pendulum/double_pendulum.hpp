#pragma once

#include <interface/application.hpp>

static constexpr const char* JOINT_1_NAME = "hinge_1";
static constexpr const char* JOINT_2_NAME = "hinge_2";

static constexpr const char* ACTUATOR_1_NAME = "torque_1";
static constexpr const char* ACTUATOR_2_NAME = "torque_2";

class DoublePendulum : public Application {
 public:
    DoublePendulum();

    /// Returns the current angle of the first joint of the pendulum
    auto GetTheta1() const -> double;

    /// Returns the current angle of the second joint of the pendulum
    auto GetTheta2() const -> double;

    /// Sets the torque of the first controlled joint
    auto SetTorque1(mjtNum torque) -> void { m_Torque1Ctrl = torque; }

    /// Sets the torque of the second controlled joint
    auto SetTorque2(mjtNum torque) -> void { m_Torque2Ctrl = torque; }

    /// Toggles the state of the first actuator
    auto ToggleActuator1() -> void { m_Actuator1Active = !m_Actuator1Active; }

    /// Toggles the state of the second actuator
    auto ToggleActuator2() -> void { m_Actuator2Active = !m_Actuator2Active; }

    /// Gets the current state of actuator 1
    auto IsActuator1Active() const -> bool { return m_Actuator1Active; }

    /// Gets the current state of actuator 2
    auto IsActuator2Active() const -> bool { return m_Actuator2Active; }

 protected:
    auto _SimStepInternal() -> void override;

 private:
    int m_Joint1HingeId{-1};
    int m_Joint2HingeId{-1};

    int m_Actuator1HingeId{-1};
    int m_Actuator2HingeId{-1};

    mjtNum m_Torque1Ctrl{0.001};
    mjtNum m_Torque2Ctrl{0.001};

    bool m_Actuator1Active{true};
    bool m_Actuator2Active{true};
};
