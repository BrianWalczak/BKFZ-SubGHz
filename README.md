# BKFZ SubGHz (Beta)
A robust program for your ESP32 to capture and replay SubGHz frequencies, inspired by the Flipper Zero and written in Arduino (C++).

> [!WARNING]
> **This project is currently in beta state as I work on bug fixes and improvements. If you find a bug, please report it <a href='https://github.com/BrianWalczak/BKFZ-SubGHz/issues'>here</a> :)**

## Features
- **Frequency Analyzer**: Scan and analyze nearby devices and their frequencies with ease.
- **Record RAW**: Capture and record nearby signals with precise RSSI configuration.
- **Replay RAW**: Replay previously captured signals from a file (no SD card required).
- **Preset Configuration**: Configure the frequency of your device, and set the preset modulation (just like the Flipper Zero!).
- **Flipper Zero Support:** Effortlessly replay any Flipper Zero SUB file, including the many publicly available ones, whenever you need.

BKFZ SubGHz utilizes the Flipper Zero **SUB** file format to save and replay signals, ensuring seamless compatibility between both devices (Flipper Zero not needed!).

## Requirements
To start, you'll need (by minimum) an [**ESP32**](https://s.click.aliexpress.com/e/_DCh9q31) [$2.97] of your choice and a [**CC1101 RF Transceiver**](https://s.click.aliexpress.com/e/_DlNeVmT) [$2.01]. You can purchase both of these parts in the links provided (I chose an ESP32-D1-Mini because of its size and price, but a standard ESP32 w/ voltage regulator will also be sufficient. I am also using a CC1101 module with an [SMA connector](https://s.click.aliexpress.com/e/_Dn4dKAr), but these tend to be more expensive).

> [!CAUTION]
> **When choosing a CC1101 module, make sure to select the operating frequency for your region. The** `433 MHz` **frequency band is most commonly used, but this may vary on your device. Your module can use either a wire antenna or an SMA antenna—both options are compatible.**

If you're looking to build a fully-fledged BKFZ SubGHz device, you'll want to purchase these additional components (about **~$13.03**):
- [2000mAh Lithium Battery](https://s.click.aliexpress.com/e/_DnKGR3V): **$6.03** on AliExpress (**ensure your ESP32 includes a built-in voltage regulator, or attach one separately**)
- [TP4056 Module (5pk)](https://s.click.aliexpress.com/e/_Dd8V8J1): **$1.36** on AliExpress
- [Latching Buttons (10pk)](https://s.click.aliexpress.com/e/_DkSsHEL): **$1.13** on AliExpress 
- [JST 2mm Connectors (10pk)](https://s.click.aliexpress.com/e/_DCTEiy7): **$1.01** on AliExpress 
- [Custom PCB Print](https://github.com/BrianWalczak/BKFZ-SubGHz/tree/main/KiCad_PCB#bkfz-subghz---kicad-pcb): **~$3.50** on JLCPCB (w/ shipping)
<br>
<img src="https://github.com/user-attachments/assets/beffe9a9-9035-4349-9055-57c599767048" alt="Assembly Parts" width="400"/>

The custom PCB design, as found [here](https://github.com/BrianWalczak/BKFZ-SubGHz/tree/main/KiCad_PCB#bkfz-subghz---kicad-pcb), is specifically designed to work with the 433 MHz module(s) linked above.

## Installation
To flash the BKFZ SubGHz program on your ESP32, you'll need to connect the CC1101 module first. By default, the following pinout is used:
| ESP32 | CC1101 |
|----------|----------|
| GND | GND |
| 3.3V | VCC |
| G5 | CSN |
| G13 | MOSI |
| G19 | MISO |
| G14 | SCK |
| G16 | GDO0 |
| G18 | GDO2 |

If you'd like to use a different pinout, you can configure the proper pins in the **config.h** configuration file (located in the **config** folder).
Once you've connected the ESP32 to your computer, you'll need to download and install the [Arduino IDE](https://www.arduino.cc/en/software), as well as the [SPIFFS Upload](https://github.com/espx-cz/arduino-spiffs-upload) plugin (you can find instructions on installation and usage in the repository). The plugin is crucial to serve essential files through the web server (CSS, fonts, icons, etc).

After installing the IDE, download the latest release for this repository and extract the ZIP file. Then, launch the `BKFZ_SubGHz.ino` file located inside the folder, and upload the data files through SPIFFS (`Ctrl` + `Shift` + `P`, then `Upload SPIFFS to Pico/ESP8266/ESP32`). **Make sure your Serial Monitor is closed during the uploading process.**

Lastly, click the `Upload` icon in the Arduino IDE to flash the code on your ESP32. You'll see `Hard resetting via RTS pin...` once the writing is complete.

## Usage
To access the user interface of the BKFZ SubGHz, open the WiFi settings on your mobile device and connect to the `BKFZ SubGHz` WiFi network (you can configure the SSID and password in the **config.h** file of the project). Once you're connected to the WiFi network, open your browser and navigate to `192.168.4.1`. If the page is not found, check the `Gateway` IP address of the WiFi network and navigate to that IP address.

If you'd like to add the user interface as a web app to your home screen, you can click `Share > Add to Home Screen` in Safari (for Android, click the 3 dots in Chrome).

Finally, open the settings tab on the user interface to configure the frequency, preset, and RSSI. Once you've configured these settings, you're all set!

## Customization (experimental)
If you'd like to configure the settings used by the BKFZ SubGHZ, modify the default user configuration, or manage the presets, open the **config** folder in the project. You can configure the following files:
- **config.h:** Used to store configurations by the device (such as pins, WiFi settings, etc).
- **presets.h:** Stores all of the available presets (as used by the Flipper Zero) and its configurations.
- **user_settings.h:** Contains the default user configuration, as well as their available options (also has hopper frequencies as used in the Frequency Analyzer).

If you'd like to customize the user interface, you can find all of the HTML files in the **static** folder. Additionally, custom fonts, icons, and other libraries such as JQuery are found in the **data** folder (as used by SPIFFS). All of these files are served at `/static`.

## Credits/Authors
This project was made possible by utilizing the following dependencies:
- [`ELECHOUSE_CC1101_SRC_DRV`](https://www.arduino.cc/reference/en/libraries/smartrc-cc1101-driver-lib/) | A library for controlling the CC1101 RF module, which is commonly used for wireless communication in Arduino projects.
- [`ESPAsyncWebServer`](https://www.arduino.cc/reference/en/libraries/espasyncwebserver/) | A library that enables the creation of web servers on the ESP32 and ESP8266.
- [`ArduinoJson`](https://www.arduino.cc/reference/en/libraries/arduinojson/) | A library for efficiently handling JSON data in Arduino projects, useful for parsing and generating JSON files.
- [`ESP32 Core Libraries`](https://github.com/espressif/arduino-esp32/tree/master/libraries) | A collection of libraries pre-installed on the ESP32 (including Wi-Fi, SPI, SPIFFS, Preferences, and much more).
- [`Flipper Zero`](https://github.com/flipperdevices/flipperzero-firmware) | This project couldn't be made possible without the extensive documentation and awesome team over at Flipper Zero ♥

## FAQ
### Why is no background noise recorded with a lower RSSI threshold?
Unlike the Flipper Zero, the BKFZ SubGHz configures the CC1101 to filter out background noise that could interfere with recording. This functionality is intentional to ensure a greater accuracy during the capturing process.

### Does this project allow me to copy devices with iButton, NFC/RFID, or Infrared technology?
No, the BKFZ SubGHz can **not** copy any of these types of devices, unlike the Flipper Zero. This project is specifically focused on replicating the SubGHz functionality of the Flipper Zero. However, it is technically possible to achieve this with an ESP32 and the appropriate module (though it's unlikely to happen). However, contributions to the project are always welcome!

### Do I need to have an internet connection?
To operate your BKFZ SubGHz, you'll need to connect to its WiFi network. However, this doesn't require an internet connection, as the BKFZ SubGHz functions entirely offline. To access the user interface, you'll need to check the gateway IP of the BKFZ SubGHz WiFi network (which is usually `192.168.4.1`).

### Who is maintaining this project behind the scenes?
At the moment, this project is solely developed and maintained by me (thanks to the amazing resources [here](#creditsauthors)). That said, I highly encourage you to contribute to the project by submitting a pull request! It would be greatly appreciated.

### I found a bug or would like to submit feedback.
That's great to hear! You can submit any feedback, or any bugs that you find, on the <a href='https://github.com/BrianWalczak/BKFZ-SubGHz/issues'>issues page</a>. I check these very frequently, and I highly encourage you to find bugs.

### I love this project! How can I support its maintenance?
I'm glad to hear that you find this project useful! If you'd like to support this project and its development, you can send me a donation <a href='https://buymeacoffee.com/briann'>here</a> :)

<br>
  <p align="center">Made with ♡ by <a href="https://www.brianwalczak.com">Briann</a></p>
