// =============================================
// Project Configuration – 6DOF Robotic Arm + Gates
// Artemis DSAI 2025 Camp – Servo Role Map
// =============================================
// Each servo is driven by PCA9685 (I2C, 50Hz PWM)
// Servo types: SG92R, MG996R, MG90S, SG90 (~270° range)
// Logic level: 3.3V (ESP32), Power: Shared 5V rail
// Note: Always center servos to ~1500µs (~90°) before mounting

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include "servo_config.h"  // Ensure servo types like MG996R are defined

// === Role-based PCA9685 channel mapping ===
// Format: SERVO_<ROLE>_<ATTRIBUTE>

// ====================
// GPIO Pin Assignments
// ====================
#define SERVO_BASE_PIN          6    // MG996R – Arm Base
#define SERVO_SHOULDER_PIN      7    // MG996R – Arm Shoulder
#define SERVO_ELBOW_PIN         12   // MG90S  – Arm Elbow
#define SERVO_WRIST_TILT_PIN    13   // MG90S  – Wrist Tilt
#define SERVO_WRIST_ROTATE_PIN  14   // MG90S  – Wrist Rotate
#define SERVO_GRIPPER_PIN       15   // MG90S  – Gripper
#define SERVO_GATE_PIN          8    // SG92R  – Conveyor Gate
#define SERVO_AUX_PIN           0    // SG90   – Optional

// ====================
// Servo Type Bindings
// ====================
#define SERVO_BASE_TYPE             MG996R
#define SERVO_SHOULDER_TYPE         MG996R
#define SERVO_ELBOW_TYPE            MG90S
#define SERVO_WRIST_TILT_TYPE       MG90S
#define SERVO_WRIST_ROTATE_TYPE     MG90S
#define SERVO_GRIPPER_TYPE          MG90S
#define SERVO_GATE_TYPE             SG92R
#define SERVO_AUX_TYPE              SG90

// =============================
// Role-based PCA9685 Channels
// =============================
#define SERVO_BASE_CHANNEL          0
#define SERVO_SHOULDER_CHANNEL      1
#define SERVO_ELBOW_CHANNEL         2
#define SERVO_WRIST_TILT_CHANNEL    3
#define SERVO_WRIST_ROTATE_CHANNEL  4
#define SERVO_GRIPPER_CHANNEL       5
#define SERVO_GATE_CHANNEL          6
#define SERVO_AUX_CHANNEL           7

// ==========================
// Servo Angle Movement Ranges
// ==========================
#define SERVO_BASE_MIN_ANGLE         90
#define SERVO_BASE_MAX_ANGLE         175

#define SERVO_SHOULDER_MIN_ANGLE     20
#define SERVO_SHOULDER_MAX_ANGLE     90

#define SERVO_ELBOW_MIN_ANGLE        50
#define SERVO_ELBOW_MAX_ANGLE        80

#define SERVO_WRIST_TILT_MIN_ANGLE   45
#define SERVO_WRIST_TILT_MAX_ANGLE   135

#define SERVO_WRIST_ROTATE_MIN_ANGLE 0
#define SERVO_WRIST_ROTATE_MAX_ANGLE 180

#define SERVO_GRIPPER_MIN_ANGLE      0
#define SERVO_GRIPPER_MAX_ANGLE      30

#define SERVO_GATE_MIN_ANGLE         0
#define SERVO_GATE_MAX_ANGLE         90

#define SERVO_AUX_MIN_ANGLE          0
#define SERVO_AUX_MAX_ANGLE          90

#endif // PROJECT_CONFIG_H
