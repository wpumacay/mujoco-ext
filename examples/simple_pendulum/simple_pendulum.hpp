#pragma once

#include <interface/application.hpp>

static constexpr const char* JOINT_NAME = "hinge";
static constexpr const char* ACTUATOR_NAME = "torque";

class SimplePendulum : public Application {
 public:
    SimplePendulum();

    /// Returns the current angle of the pendulum
    auto GetTheta() const -> double;

protected:
    auto _SimStepInternal() -> void override;

 private:
    int m_JointHingeId{-1};
    int m_ActuatorHingeId{-1};
};
