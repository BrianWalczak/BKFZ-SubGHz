# BKFZ SubGHz - KiCad PCB
A custom-designed printed circuit board designed for connecting a [Wemos D1 Mini](https://s.click.aliexpress.com/e/_DCh9q31) (ESP32), a [CC1101](https://s.click.aliexpress.com/e/_DlNeVmT) module, and a [TP4056](https://s.click.aliexpress.com/e/_Dd8V8J1) battery module (optional). This PCB was designed for the BKFZ SubGHz as an easy way to connect the components.

This PCB was designed in KiCad (you can import it with `File > Open Project` and exported as a ready-to-print Gerber file. I printed this PCB for about **$3.50** (quanity of 5) on [JLCPCB](https://jlcpcb.com/) (not sponsored).

### Assembly
I'm still waiting for the PCB to arrive (takes about 2 weeks from shipping date), but I'll update this README.md with some photos of the assembly when it arrives. For this project, I'll be soldering female headers on the PCB for easy attachment and removal of the ESP32 and CC1101.

Since this PCB design has not yet been tested, I would not recommended having it printed as of now. This documenation will be updated once it has been tested and confirmed working by me.

### Photos
This project utilized the Wemos D1 Mini symbol and footprint by [@r0oland](https://github.com/r0oland/ESP32_mini_KiCad_Library), as well as the TP4056 battery module footprint by [@ccadic](https://github.com/ccadic/TP4056-18650). The CC1101 symbol and footprint was designed by me, as I was unable to find it online anywhere.

#### Schematic
<img src="https://github.com/user-attachments/assets/2ef62ee9-d988-4840-83af-1605c1f3d801" alt="Schematic" width="600"/>

#### PCB Design
<img src="https://github.com/user-attachments/assets/6347270d-f7c1-4772-a043-a6090f3d70bd" alt="PCB Design" width="200"/>
