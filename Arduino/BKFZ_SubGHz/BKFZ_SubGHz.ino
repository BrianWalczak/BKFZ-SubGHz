// Copyright 2024 - © Brian Walczak (view the LICENSE for more information)

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <esp_attr.h>
#include <Arduino.h>
#include <SPI.h>
#include <SPIFFS.h>

// -- CC1101 configurations -- //
#include <config/config.h> // used to configure basic variables (such as pinout, max samples, etc.)
#include <config/presets.h> // contains all presets w/ their settings
#include <config/user_settings.h> // default user settings and their options

int samples[MAX_SAMPLES];
int tempSmooth[MAX_SAMPLES];
volatile int sampleIndex = 0;
volatile unsigned long lastTime = 0;

// -- Signal Detection Parameters -- //
bool DETECT_RUNNING = false;
bool DETECT_QUEUED = false;

Preferences preferences;
AsyncWebServer server(SERVER_PORT);
AsyncWebSocket ws("/ws");

// Event handler for web sockets (used for Frequency Analyzer as quick data transmission)
void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
  switch(type) {
    case WS_EVT_CONNECT:
      break;
      
    case WS_EVT_DISCONNECT:
      if(DETECT_RUNNING == true) {
        DETECT_RUNNING = false; // disable analyzer if enabled while disconnect
        Serial.println("A websocket user has been disconnected from Frequency Analyzer.");
      }
      break;
      
    case WS_EVT_ERROR:
      Serial.printf("WebSocket error: %s\n", (char*)arg);
      break;
      
    case WS_EVT_DATA:
      if (len > 0) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data, len);

        if (doc["type"] == "detect") { // If the websocket request is from analyzer
          JsonObject dataObject = doc["data"];
          if (dataObject.containsKey("rssi")) {
            // Update the Frequency Analyzer RSSI to the requested one
            settings.detect_rssi = dataObject["rssi"].as<int>(); // make sure its int

            Serial.println("Updated detect_rssi to " + String(settings.detect_rssi));
          }
        }
      }
      break;
      
    default:
      break;
  }
}

// Updates the settings for the CC1101 (utilizes user settings)
void setupCC1101(bool transmit, int retry = false) {
  ELECHOUSE_cc1101.setSpiPin(SCK_CPIN, MISO_CPIN, MOSI_CPIN, CSN_CPIN);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setMHZ(settings.frequency / 1000000.0);

  if(transmit) {
    pinMode(GDO0_CPIN, OUTPUT);
    ELECHOUSE_cc1101.SetTx(); // Enables transmit mode (used for sending)
  } else {
    pinMode(GDO2_CPIN, INPUT);
    ELECHOUSE_cc1101.SetRx(); // Enables receive mode (used for listening)
  }

  const Preset* preset = findPreset(settings.preset);
  applyConfiguration(preset->data, preset->length);
  
  if(!ELECHOUSE_cc1101.getCC1101()) {
    if(!retry) {
      Serial.println("Connection error with CC1101, retrying...");
      setupCC1101(transmit, true);
    } else {
      Serial.print("Failed CC1101 connection retry. Please check your pins.");
    }
  }
}

// Simple function to inject settings w/ options in window (used for web server)
String injectSettings() {
  String json = settingsToJson();
  String jsonOptions = settingsOptionsToJson();
  String script = "<script>window.settings = " + json + "</script>";
  String options = "<script>window.settings.options = " + jsonOptions + "</script>";

  return script + options;
}

// Saves the current settings/configurations as non-volatile storage
void saveSettings() {
  preferences.begin("settings", false);

  preferences.putString("preset", settings.preset);
  preferences.putInt("frequency", settings.frequency);
  preferences.putInt("rssi", settings.rssi);
  preferences.putInt("detect_rssi", settings.detect_rssi);

  preferences.end();
}

void setup() {
  Serial.begin(9600);
  delay(500);
  while (!Serial) { ; }

  // -- Web Server Setup -- //
  /* This is the regular configuration, however you can use the already-existing network for testing purposes */
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();

  preferences.begin("settings", false);
  settings.preset = preferences.getString("preset", settings.preset);
  settings.frequency = preferences.getInt("frequency", settings.frequency);
  settings.rssi = preferences.getInt("rssi", settings.rssi);
  settings.detect_rssi = preferences.getInt("detect_rssi", settings.detect_rssi);
  preferences.end();

  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS. Please check if SPIFFS is properly installed.");
    return;
  }

  // -- All Pages -- //
  server.serveStatic("/assets", SPIFFS, "/assets");

  server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/home.html", "r");
    String content = file.readString();

    file.close();
    request->send(200, "text/html", content);
  });

  server.on("/record", HTTP_GET, [] (AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/record.html", "r");
    String content = file.readString();

    file.close();
    request->send(200, "text/html", injectSettings() + content);
  });

  server.on("/play", HTTP_GET, [] (AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/play.html", "r");
    String content = file.readString();

    file.close();
    request->send(200, "text/html", content);
  });

  server.on("/analyzer", HTTP_GET, [] (AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/frequency_analyzer.html", "r");
    String content = file.readString();

    file.close();
    DETECT_QUEUED = true;
    request->send(200, "text/html", injectSettings() + content);
  });

  server.on("/settings", HTTP_GET, [] (AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/settings.html", "r");
    String content = file.readString();

    file.close();
    request->send(200, "text/html", injectSettings() + content);
  });

  // -- API Requests -- //

  server.on("/api/record/start", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("Recording has been successfully started for preset " + settings.preset + ", frequency " + String(settings.frequency / 1000000.0) + " MHz, w/ RSSI " + settings.rssi + ".");
    startRecording();

    request->send(200, "text/html", "OK");
  });

  server.on("/api/record/stop", HTTP_GET, [] (AsyncWebServerRequest *request) {
    stopRecording();
    Serial.println("Found " + String(sampleIndex) + " RAW samples, smoothing needed.");
    delay(100);
    smoothenSamples();
    Serial.println("Recording has been successfully finished with a " + String(sampleIndex) + " sample index detected.");

    // Handle signal data accordingly
    String prepend = "";
    String presetName = settings.preset;
    int freq_format = settings.frequency;
    presetName.replace("AM270", "FuriHalSubGhzPresetOok270Async");
    presetName.replace("AM650", "FuriHalSubGhzPresetOok650Async");
    presetName.replace("FM238", "FuriHalSubGhzPreset2FSKDev238Async");
    presetName.replace("FM476", "FuriHalSubGhzPreset2FSKDev238Async");

    String result = "Filetype: Flipper SubGhz RAW File\nVersion: 1\n# Created with BKFZ SubGHz\nFrequency: " + String(freq_format) + "\nPreset: " + presetName + "\nProtocol: RAW\nRAW_Data: ";

    // remove first sample if it's a negative number
    if (samples[0] < 0) {
      // Shift all elements to the left by one
      for (int i = 0; i < sampleIndex - 1; ++i) {
        samples[i] = samples[i + 1];
      }
      sampleIndex--; // Decrease the total number of elements
    }

    // add each sample to file
    for (int i = 0; i < sampleIndex; ++i) {
        String valueToAdd = prepend + String(samples[i]);
        result += valueToAdd;
        prepend = " ";

        // After every 512 samples, add a new line and reset prepend
        if ((i + 1) % 512 == 0) {
          result += "\nRAW_Data: ";  // Move to the next line after every 512 samples
          prepend = "";    // Reset prepend for the new line
        }
    }

    request->send(200, "text/html", result);
  });
  
  server.on("/api/play", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("samples", true) && request->hasParam("frequency", true) && request->hasParam("length", true) && request->hasParam("preset", true)) {
      // Store old settings to revert when done
      String old_preset = settings.preset;
      int old_freq = settings.frequency;


      String samplesParam = request->getParam("samples", true)->value();
      String frequencyParam = request->getParam("frequency", true)->value();
      String lengthParam = request->getParam("length", true)->value();
      String presetParam = request->getParam("preset", true)->value();
      int reqLength = lengthParam.toInt();
      int reqSamples[reqLength];

      // Reconstruct samples array from response
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, samplesParam);
      JsonArray array = doc.as<JsonArray>();

      for (int i = 0; i < reqLength; i++) {
          reqSamples[i] = array[i].as<int>();
      }

      // Update settings to new data (send samples already runs setupCC1101)
      settings.preset = presetParam;
      settings.frequency = frequencyParam.toInt();
      Serial.println("Now playing file requested by user, successfully updated to file settings (preset " + presetParam + ", frequency " + String(frequencyParam.toInt() / 1000000.0) + " MHz).");

      playSignal(reqSamples, reqLength);

      Serial.println("Successfully played file requested, reverting back to old settings (preset " + old_preset + ", frequency " + String(old_freq / 1000000.0) + " MHz).");
      // Revert settings back to original and run setupCC1101
      settings.preset = old_preset;
      settings.frequency = old_freq;
      setupCC1101(true); // transmit param doesn't really matter since it'll be updated anyway
      request->redirect("/play?success=true");
    } else {
      request->redirect("/play?success=false");
    }
  });

  server.on("/api/settings", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("preset", true) && request->hasParam("frequency", true) && request->hasParam("rssi", true)) {
      String frequencyParam = request->getParam("frequency", true)->value();
      String rssiParam = request->getParam("rssi", true)->value();
      String preset = request->getParam("preset", true)->value();
      int frequency = frequencyParam.toInt();
      int rssi = rssiParam.toInt();

      settings.preset = preset;
      settings.frequency = frequency;
      settings.rssi = rssi;
      saveSettings(); // Save settings in non-volatile storage

      setupCC1101(true); // transmit param doesn't really matter since it'll be updated anyway
      request->redirect("/settings?success=true");
    } else {
      request->redirect("/settings?success=false");
    }
  });

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();

  setupCC1101(true);
  Serial.print("The CC1101 is ready with an IP address of ");
  Serial.println(IP);
}

void loop() {
  if(DETECT_QUEUED && !DETECT_RUNNING) {
    DETECT_QUEUED = false;
    frequencyAnalyzer();
  }
}

// Enables receiver mode and records RAW samples
void startRecording() {
  setupCC1101(false); // Initizalize CC1101 with receiver mode
  
  // Update all of the pins and setup interrupt
  memset(samples, 0, sizeof(samples)); // Clear the sample array
  sampleIndex = 0; // Clear the sample count
  attachInterrupt(digitalPinToInterrupt(GDO2_CPIN), onSignalChange, CHANGE);
}

// Stops recording and checks if successful
bool stopRecording(void) {
  detachInterrupt(digitalPinToInterrupt(GDO2_CPIN));

  if (micros() - lastTime > 100000){
    return 1;
  }else{
    return 0;
  }
}

// Capture and analyze nearby frequencies w/ RSSI
void frequencyAnalyzer() {
  DETECT_RUNNING = true;
  Serial.println("Frequency analyzer has been started by the user (watch for websockets).");

  // Store old settings to revert when done
  int old_freq = settings.frequency;

  while(DETECT_RUNNING == true) {
    int highestRssi = -INFINITY;
    int strongestFreq = 0;

    for(int frequency : hopperFrequenciesUSA) {
      settings.frequency = frequency; // Update to hopper frequency
      setupCC1101(false);

      int rssi = ELECHOUSE_cc1101.getRssi(); // Get the current RSSI

      // This signal is stronger than the one before
      if(rssi >= highestRssi) {
        highestRssi = rssi;
        strongestFreq = frequency;
      }
    }

    // If the strongest signal is within the RSSI threshold
    if(highestRssi >= settings.detect_rssi) {
      DynamicJsonDocument doc(1024);
      doc["type"] = "detect";
      doc["data"]["freq"] = String(strongestFreq);
      doc["data"]["rssi"] = String(highestRssi);

      String jsonString;
      serializeJson(doc, jsonString);

      ws.textAll(jsonString);
    }
  }

  // Revert settings back to original and run setup
  settings.frequency = old_freq;
  setupCC1101(true); // transmit param doesn't really matter since it'll be updated anyway
  Serial.println("Frequency analyzer has been stopped by the user.");
}

// Play a signal from client-side file
void playSignal(int reqSamples[], int reqLength) {
  Serial.println("Now transmitting " + String(reqLength) + " samples...");
  setupCC1101(true);

  // Transmit all of the sample data
  for (int i = 0; i < reqLength; i++) {
    if (reqSamples[i] > 0) {
      digitalWrite(GDO0_CPIN, HIGH);
    } else {
      digitalWrite(GDO0_CPIN, LOW);
    }
    delayMicroseconds(abs(reqSamples[i]));
  }
}

// Smoothens out the RAW samples to correct format (not written by me!)
void smoothenSamples() {
  #define signalstorage 10
  
  // Initialize variables for signal storage, counts, and sums
  int signalanz = 0;
  int timingdelay[signalstorage];
  long signaltimings[signalstorage * 2];
  int signaltimingscount[signalstorage];
  long signaltimingssum[signalstorage];
  long signalsum = 0;

  // Initialize signal timings with default values
  for (int i = 0; i < signalstorage; i++) {
    signaltimings[i * 2] = 100000;  // Minimum timing
    signaltimings[i * 2 + 1] = 0;   // Maximum timing
    signaltimingscount[i] = 0;      // Count of timings in this range
    signaltimingssum[i] = 0;        // Sum of timings in this range
  }

  // Sum up all signal samples
  for (int i = 1; i < sampleIndex; i++) {
    signalsum += samples[i];
  }

  // Group signals into timing ranges
  for (int p = 0; p < signalstorage; p++) {
    for (int i = 1; i < sampleIndex; i++) {
      // Find the minimum timing for the group
      if (p == 0) {
        if (samples[i] < signaltimings[p * 2]) {
          signaltimings[p * 2] = samples[i];
        }
      } else {
        if (samples[i] < signaltimings[p * 2] && samples[i] > signaltimings[p * 2 - 1]) {
          signaltimings[p * 2] = samples[i];
        }
      }
    }

    // Find the maximum timing for the group
    for (int i = 1; i < sampleIndex; i++) {
      if (samples[i] < signaltimings[p * 2] + ERROR_TOLERANCE && samples[i] > signaltimings[p * 2 + 1]) {
        signaltimings[p * 2 + 1] = samples[i];
      }
    }

    // Count how many samples fall into this timing range and sum their values
    for (int i = 1; i < sampleIndex; i++) {
      if (samples[i] >= signaltimings[p * 2] && samples[i] <= signaltimings[p * 2 + 1]) {
        signaltimingscount[p]++;
        signaltimingssum[p] += samples[i];
      }
    }
  }

  // Determine how many signal groups are active
  signalanz = signalstorage;
  for (int i = 0; i < signalstorage; i++) {
    if (signaltimingscount[i] == 0) {
      signalanz = i;
      break;
    }
  }

  // Sort signal groups by count (from most frequent to least frequent)
  for (int s = 1; s < signalanz; s++) {
    for (int i = 0; i < signalanz - s; i++) {
      if (signaltimingscount[i] < signaltimingscount[i + 1]) {
        // Swap the signal group data
        int temp1 = signaltimings[i * 2];
        int temp2 = signaltimings[i * 2 + 1];
        int temp3 = signaltimingssum[i];
        int temp4 = signaltimingscount[i];

        signaltimings[i * 2] = signaltimings[(i + 1) * 2];
        signaltimings[i * 2 + 1] = signaltimings[(i + 1) * 2 + 1];
        signaltimingssum[i] = signaltimingssum[i + 1];
        signaltimingscount[i] = signaltimingscount[i + 1];

        signaltimings[(i + 1) * 2] = temp1;
        signaltimings[(i + 1) * 2 + 1] = temp2;
        signaltimingssum[i + 1] = temp3;
        signaltimingscount[i + 1] = temp4;
      }
    }
  }

  // Calculate average timing for each group
  for (int i = 0; i < signalanz; i++) {
    timingdelay[i] = signaltimingssum[i] / signaltimingscount[i];
  }

  // Correct raw data based on timing groups and assign high/low values
  bool lastbin = false;  // Tracks whether the last bin was high (false = low, true = high)
  int smoothCount = 0;

  for (int i = 1; i < sampleIndex; i++) {
    float r = (float)samples[i] / timingdelay[0];
    int calculate = r;
    r = r - calculate;
    r *= 10;
    if (r >= 5) {
      calculate += 1;
    }

    if (calculate > 0) {
      // Toggle the bin state between high and low
      lastbin = !lastbin;

      // Assign positive for high and negative for low
      tempSmooth[smoothCount] = (lastbin ? 1 : -1) * (calculate * timingdelay[0]);
      smoothCount++;
    }
  }

  // Output smoothed data
  memset(samples, 0, sizeof(samples)); // Clear the sample array
  sampleIndex = smoothCount; // Update sample index to smoothed count
  for (int i = 0; i < smoothCount; i++) {
    samples[i] = tempSmooth[i];
  }

  return;
}

// Handle event changes of CC1101
void onSignalChange() {
  const unsigned long time = micros();
  const unsigned int duration = time - lastTime;
  int rssi = ELECHOUSE_cc1101.getRssi(); // Get the current RSSI

  if (duration > 100000) {
    sampleIndex = 0;
  }

  if (duration >= 100 && sampleIndex < MAX_SAMPLES && rssi >= settings.rssi) {
    samples[sampleIndex++] = duration;
  }

  lastTime = time;
}
