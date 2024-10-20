#ifndef CC1101SETTINGS_H
#define CC1101SETTINGS_H

/* Documentation & References /*
# We should have a minimum RSSI of -85 and maximum of -40 (w/ steps of 5).
  https://github.com/flipperdevices/flipperzero-firmware/blob/5272eb75500bca6927947f15f6d2aa828a6ab3b2/applications/main/subghz/scenes/subghz_scene_receiver_config.c#L15

# A great guide exists explaining the presets that Flipper Zero utilizes, as well as other SubGHz features.
  https://github.com/jamisonderek/flipper-zero-tutorials/wiki/Sub-GHz#read-raw---subghz

# You can also reference the Flipper Zero official firmware source code for CC1101 configurations.
  https://github.com/flipperdevices/flipperzero-firmware/blob/7c88a4a8f1062063b74277c03617fb9e083e538b/lib/subghz/devices/cc1101_configs.c#L76
*/

int hopperFrequenciesUSA[] = {
    310000000,
    315000000,
    318000000,
    390000000,
    433920000,
    868350000,
};

struct Settings {
    String preset; // pointer (*) to reference Preset
    int frequency;
    int rssi;
    int detect_rssi;
};

struct SettingsOptions {
  String preset[numPresets];
  int frequency[17];
  int rssi[10];
};

// Used to display the status of the device (detecting, running, etc.)
struct Status {
    String detect;
    String record;
};

// Settings which contains default presets (can be updated through the website)
static Settings settings = {
  "AM650", // Preset
  433920000, // Frequency
  -65, // Overall RSSI threshold
  -40 // RSSI threshold for Frequency Analyzer ONLY (added for ease-of-use, made it low to prevent noise)
};

// Array which contains the current status of the device
static Status status = {
  "IDLE", // Detect (frequency analyzer)
  "IDLE" // Recording (record)
};

// The available options for each setting (used to display on UI)
static SettingsOptions settingsOptions = {
  { "AM270", "AM650", "FM238", "FM476"}, // Presets
  { // Frequency
    /* 300 - 348 */
    300000000,
    303875000,
    304250000,
    310000000,
    315000000,
    318000000,

    /* 387 - 464 */
    390000000,
    418000000,
    433075000,
    433420000,
    433920000,
    434420000,
    434775000,
    438900000,

    /* 779 - 928 */
    868350000,
    915000000,
    925000000,
  },
  { -85, -80, -75, -70, -65, -60, -55, -50, -45, -40 } // RSSI threshold
};


// Converts the user settings as a readable JSON string (courtesy of ChatGPT)
String settingsToJson() {
  DynamicJsonDocument doc(256);

  doc["preset"] = settings.preset;
  doc["frequency"] = settings.frequency;
  doc["rssi"] = settings.rssi;
  doc["detect_rssi"] = settings.detect_rssi;

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

// Converts the settings options as a readable JSON string (courtesy of ChatGPT, mainly used for displaying in /settings)
String settingsOptionsToJson() {
  DynamicJsonDocument doc(256);

  JsonArray presets = doc.createNestedArray("preset");
  for (String preset : settingsOptions.preset) {
    presets.add(preset);
  }

  JsonArray frequencies = doc.createNestedArray("frequency");
  for (int frequency : settingsOptions.frequency) {
    frequencies.add(frequency);
  }

  JsonArray rssiThreshold = doc.createNestedArray("rssi");
  for (int threshold : settingsOptions.rssi) {
    rssiThreshold.add(threshold);
  }

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

// Converts the status as a readable value (courtesy of ChatGPT)
String statusToJson() {
  DynamicJsonDocument doc(128);

  doc["detect"] = status.detect;
  doc["record"] = status.record;

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

#endif