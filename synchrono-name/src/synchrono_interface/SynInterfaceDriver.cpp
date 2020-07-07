// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Radu Serban, Justin Madsen, Conlain Kelly
// =============================================================================
//
// Irrlicht-based GUI driver for the a vehicle. This class implements the
// functionality required by its base ChDriver class using keyboard or joystick
// inputs. If a joystick is present it will use that as an input; it will
// otherwise default to a keyboard input.
// As an Irrlicht event receiver, its OnEvent() callback is used to keep track
// and update the current driver inputs. As such it does not need to override
// the default no-op Advance() virtual method.
//
// =============================================================================

#include <algorithm>

#include "synchrono_interface/SynInterfaceDriver.h"

namespace chrono {
namespace vehicle {

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
SYN_INTERFACE_API SynInterfaceDriver::SynInterfaceDriver(ChVehicle& veh)
    : ChDriver(veh),
      m_steering_target(0),
      m_throttle_target(0),
      m_braking_target(0),
      m_stepsize(1e-3),
      m_steering_delta(1.0 / 50),
      m_throttle_delta(1.0 / 50),
      m_braking_delta(1.0 / 50),
      m_steering_gain(4.0),
      m_throttle_gain(4.0),
      m_braking_gain(4.0){}

SYN_INTERFACE_API void SynInterfaceDriver::TurnLeft() {
    m_steering_target = ChClamp(m_steering_target - m_steering_delta, -1.0, +1.0);
}

SYN_INTERFACE_API void SynInterfaceDriver::TurnRight() {
    m_steering_target = ChClamp(m_steering_target + m_steering_delta, -1.0, +1.0);
}

SYN_INTERFACE_API void SynInterfaceDriver::SpeedUp() {
    m_throttle_target = ChClamp(m_throttle_target + m_throttle_delta, 0.0, +1.0);
    if (m_throttle_target > 0)
        m_braking_target = ChClamp(m_braking_target - m_braking_delta * 3, 0.0, +1.0);
}

void SynInterfaceDriver::SlowDown() {
    m_throttle_target = ChClamp(m_throttle_target - m_throttle_delta * 3, 0.0, +1.0);
    if (m_throttle_target <= 0)
        m_braking_target = ChClamp(m_braking_target + m_braking_delta, 0.0, +1.0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
SYN_INTERFACE_API void SynInterfaceDriver::SetThrottleDelta(double delta) {
    m_throttle_delta = delta;
}

SYN_INTERFACE_API void SynInterfaceDriver::SetSteeringDelta(double delta) {
    m_steering_delta = delta;
}

SYN_INTERFACE_API void SynInterfaceDriver::SetBrakingDelta(double delta) {
    m_braking_delta = delta;
}

SYN_INTERFACE_API void SynInterfaceDriver::SetGains(double steering_gain, double throttle_gain, double braking_gain) {
    m_steering_gain = steering_gain;
    m_throttle_gain = throttle_gain;
    m_braking_gain = braking_gain;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
SYN_INTERFACE_API void SynInterfaceDriver::Synchronize(double time) {}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
SYN_INTERFACE_API void SynInterfaceDriver::Advance(double step) {
    // Integrate dynamics, taking as many steps as required to reach the value 'step'
    double t = 0;
    while (t < step) {
        double h = std::min<>(m_stepsize, step - t);

        double throttle_deriv = m_throttle_gain * (m_throttle_target - m_throttle);
        double steering_deriv = m_steering_gain * (m_steering_target - m_steering);
        double braking_deriv = m_braking_gain * (m_braking_target - m_braking);

        SetThrottle(m_throttle + h * throttle_deriv);
        SetSteering(m_steering + h * steering_deriv);
        SetBraking(m_braking + h * braking_deriv);

        t += h;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
SYN_INTERFACE_API std::string SynInterfaceDriver::GetInputModeAsString() const {
    return std::string("KEYBOARD");
}

}  // end namespace vehicle
}  // end namespace chrono
