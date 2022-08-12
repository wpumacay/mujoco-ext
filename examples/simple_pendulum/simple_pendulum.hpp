#pragma once

#include <core/application.hpp>

static constexpr const char* BODY_NAME = "pole";
static constexpr const char* JOINT_NAME = "hinge";
static constexpr const char* ACTUATOR_NAME = "torque";

static constexpr const char* SENSOR_JNTPOS = "sns_jntpos";
static constexpr const char* SENSOR_JNTVEL = "sns_jntvel";

struct JntSensorPos {
    int id = -1;
    int adr = -1;
    float value = 0.0F;
};

struct JntSensorVel {
    int id = -1;
    int adr = -1;
    float value = 0.0F;
};

class SimplePendulum : public Application {
 public:
    SimplePendulum();

    /// Returns the current angle of the pendulum
    auto GetTheta() const -> double;

 protected:
    auto _RenderUiInternal() -> void override;

    auto _SimStepInternal() -> void override;

    auto _ReloadInternal() -> void override;

 private:
    int m_BodyPoleId{-1};
    int m_JointHingeId{-1};
    int m_ActuatorHingeId{-1};
    int m_JntAdr{-1};
    int m_DofAdr{-1};
    JntSensorPos m_SensorJntPos;
    JntSensorVel m_SensorJntVel;
};
