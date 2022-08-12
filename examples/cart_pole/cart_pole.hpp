#pragma once

#include <core/application.hpp>

static constexpr const char* JOINT_HINGE_NAME = "hinge_1";
static constexpr const char* JOINT_SLIDE_NAME = "slider";
static constexpr const char* ACTUATOR_NAME = "force";

class CartPole : public Application {
 public:
    CartPole();

    /// Returns the current slider position
    auto GetPositionX() const -> mjtNum;

    /// Returns the current angle of deviation of the pole
    auto GetTheta() const -> mjtNum;

    /// Sets the force applied to the cart
    auto SetForceX(mjtNum force) -> void { m_CartForceX = force; }

 protected:
    auto _SimStepInternal() -> void override;

 private:
    /// The id of the hinge joint connecting the cart to the pole
    int m_JointHingeId{-1};
    /// The id of the slider joint in the mcjf model
    int m_JointSlideId{-1};
    /// The id of the actuator controlling the slide-joint in the mjcf model
    int m_ActuatorSlideId{-1};
    /// Amount of force applied at the cart
    mjtNum m_CartForceX{0.0};
};
