#ifndef CC1101PRESETS_H
#define CC1101PRESETS_H

#include <ELECHOUSE_CC1101_SRC_DRV.h>

/* Documentation & References /*
# We should have a minimum RSSI of -85 and maximum of -40 (w/ steps of 5).
  https://github.com/flipperdevices/flipperzero-firmware/blob/5272eb75500bca6927947f15f6d2aa828a6ab3b2/applications/main/subghz/scenes/subghz_scene_receiver_config.c#L15

# A great guide exists explaining the presets that Flipper Zero utilizes, as well as other SubGHz features.
  https://github.com/jamisonderek/flipper-zero-tutorials/wiki/Sub-GHz#read-raw---subghz

# You can also reference the Flipper Zero official firmware source code for CC1101 configurations.
  https://github.com/flipperdevices/flipperzero-firmware/blob/7c88a4a8f1062063b74277c03617fb9e083e538b/lib/subghz/devices/cc1101_configs.c#L76
*/

const uint8_t AM270[] = {
  CC1101_MDMCFG2, 0x30, // Format ASK/OOK, No preamble/sync
  CC1101_PKTCTRL0, 0x32, // Async, continious, no whitening
  CC1101_MDMCFG4, 0x67, // Rx BW filter is 270.833333kHz
  CC1101_MDMCFG3, 0x32, // Data rate is 3.79372 kBaud

  CC1101_MDMCFG0, 0x00, // Channel spacing is 25kHz
  CC1101_MDMCFG1, 0x00, // Channel spacing is 25kHz
};

const uint8_t AM650[] = {
  CC1101_MDMCFG2, 0x30, // Format ASK/OOK, No preamble/sync
  CC1101_PKTCTRL0, 0x32, // Async, continious, no whitening
  CC1101_MDMCFG4, 0x17, // Rx BW filter is 650.000kHz
  CC1101_MDMCFG3, 0x32, // Data rate is 3.79372 kBaud

  CC1101_MDMCFG0, 0x00, // Channel spacing is 25kHz
  CC1101_MDMCFG1, 0x00, // Channel spacing is 25kHz
};

const uint8_t FM238[] = {
  CC1101_MDMCFG2, 0x04, // Format 2-FSK/FM, No preamble/sync, Disable (current optimized)
  CC1101_PKTCTRL0, 0x32, CC1101_PKTCTRL1, 0x04, // Async, continious, no whitening
  CC1101_MDMCFG4, 0x67, //Rx BW filter is 270.833333 kHz
  CC1101_MDMCFG3, 0x83, // Data rate is 4.79794 kBaud
  CC1101_DEVIATN, 0x04, //Deviation 2.380371 kHz

  CC1101_MDMCFG0, 0x00, // Channel spacing is 25kHz
  CC1101_MDMCFG1, 0x02, // Channel spacing is 100 kHz
};

const uint8_t FM476[] = {
  CC1101_MDMCFG2, 0x04, // Format 2-FSK/FM, No preamble/sync, Disable (current optimized)
  CC1101_PKTCTRL0, 0x32, CC1101_PKTCTRL1, 0x04, // Async, continious, no whitening
  CC1101_MDMCFG4, 0x67, //Rx BW filter is 270.833333 kHz
  CC1101_MDMCFG3, 0x83, // Data rate is 4.79794 kBaud
  CC1101_DEVIATN, 0x47, //Deviation 47.60742 kHz

  CC1101_MDMCFG0, 0x00, // Channel spacing is 25kHz
  CC1101_MDMCFG1, 0x02, // Channel spacing is 100 kHz
};


struct Preset {
    const char* name;
    const uint8_t* data;
    size_t length;
};

Preset Presets[] = {
    { "AM270", AM270, sizeof(AM270) / sizeof(AM270[0]) },
    { "AM650", AM650, sizeof(AM650) / sizeof(AM650[0]) },
    { "FM238", FM238, sizeof(FM238) / sizeof(FM238[0]) },
    { "FM476", FM476, sizeof(FM476) / sizeof(FM476[0]) }
};

constexpr size_t numPresets = sizeof(Presets) / sizeof(Presets[0]);

Preset* findPreset(String name) {
    for (size_t i = 0; i < numPresets; ++i) {
        if (strcmp(Presets[i].name, name.c_str()) == 0) {
            return &Presets[i];
        }
    }
    return nullptr;
}

// Applies the specified preset configuration to CC1101
void applyConfiguration(const uint8_t* config, size_t length) {
  for (size_t i = 0; i < length; i += 2) {
    uint8_t reg = config[i];
    uint8_t value = config[i + 1];
    ELECHOUSE_cc1101.SpiWriteReg(reg, value);
  }
}

#endif