#ifndef SERVO_CONFIG_H
#define SERVO_CONFIG_H

// ==========================
// Servo Configuration Header
// ==========================
// This file defines the characteristics (timing and movement range) for different servo types
// used in your robot or project. These values tell the microcontroller how to send the right
// signals to move each kind of servo correctly.

// This structure holds key info about a servo motor:
// - minPulse and maxPulse: Pulse width range in microseconds (for PWM control)
// - minAngle and maxAngle: The movement range in degrees (typically 0–180)
struct ServoSpecs {
  int minPulse;   // Minimum pulse width (in microseconds)
  int maxPulse;   // Maximum pulse width (in microseconds)
  int minAngle;   // Minimum angle the servo can rotate to
  int maxAngle;   // Maximum angle the servo can rotate to
};

// Below we define the specs for each servo motor type used in our camp:
// You can add more if needed – just match the format.

const ServoSpecs MG996R = {500, 2500, 0, 180};  // High-torque servo (e.g., robot arm base)
const ServoSpecs MG90S  = {500, 2400, 0, 180};  // Mid-size servo (e.g., arm joints)
const ServoSpecs SG92R  = {500, 2400, 0, 180};  // Compact servo (e.g., gate servos)
const ServoSpecs SG90   = {500, 2400, 0, 180};  // Standard micro servo (e.g., small flippers)

#endif // SERVO_CONFIG_H
