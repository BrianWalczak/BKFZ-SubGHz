#ifndef CC1101CONFIG_H
#define CC1101CONFIG_H

/* 
----- ESP32 to CC1101 -----
  GND connects to GND
  
  3.3V connects to VCC
  G5 connects to CSN/SS
  G13 connects to SI/MOSI 
  G19 connects to SO/MISO
  G14 connects to SCK
  G16 connects to GDO0
  G18 connects to GDO2
*/

int MISO_CPIN = 19;
int MOSI_CPIN = 13;
int SCK_CPIN = 14;
int CSN_CPIN = 5;
int GDO0_CPIN = 16;
int GDO2_CPIN = 18;


// -- Recording Signal Parameters -- //
const int MAX_SAMPLES = 8000;
const int ERROR_TOLERANCE = 200;

// -- Access Point Information -- //
const char* ssid = "BKFZ SubGHz"; // Set an SSID for the access point
const char* password = ""; // Set a password for the access point (optional)
const int SERVER_PORT = 80; // Set the desired port for the web interface


#endif