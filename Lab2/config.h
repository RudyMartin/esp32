#ifndef CONFIG_TEMPLATE_H
#define CONFIG_TEMPLATE_H

// 🧪 Team Configuration Template
// Rename this file to `config.h` and fill in your team-specific details.

struct SystemConfig {
  // Team identity
  const char* source = "group_X";        // Replace with your group ID (e.g., "group_1")
  const char* sensor = "camera_X";       // Optional: label for your sensor device
  const char* model = "YourModelName";   // Name of the ML model you are using
  const char* experiment = "Lab2_TFLite";// Optional: label for current experiment

  // WiFi configuration
  bool enable_wifi = true;
  const char* wifi_ssid = "YOUR_WIFI_SSID";
  const char* wifi_password = "YOUR_WIFI_PASSWORD";

  // NAS upload paths
  const char* nas_main_folder = "/nas/artemis2025/";
  const char* nas_group_folder = "/nas/artemis2025/group_X/";  // Match your group ID
  const char* upload_target = "nas.local"; // Can be IP or hostname of your NAS
};

// Declare global config instance
static const SystemConfig config;

#endif