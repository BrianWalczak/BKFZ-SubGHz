# BKFZ SubGHz - KiCad PCB
A custom-designed printed circuit board designed for connecting a [Wemos D1 Mini](https://s.click.aliexpress.com/e/_DCh9q31) (ESP32), a [CC1101](https://s.click.aliexpress.com/e/_DlNeVmT) module, and a [TP4056](https://s.click.aliexpress.com/e/_Dd8V8J1) battery module (optional). This PCB was designed for the BKFZ SubGHz as an easy way to connect the components.

This PCB was designed in KiCad (you can import it with `File > Open Project` and exported as a ready-to-print Gerber file. I printed this PCB for about **$3.50** (quanity of 5) on [JLCPCB](https://jlcpcb.com/) (not sponsored).

### Assembly
> **If you'd like to see detailed photos of each step in the assembly process, you can check them out [here](https://github.com/BrianWalczak/BKFZ-SubGHz/tree/main/Other/Assembly%20Photos).**

To assemble the BKFZ SubGHz device, I began by soldering female headers onto the PCB. This allows for easy attachment and removal of the ESP32 and CC1101 (if any changes are necessary). I started by soldering female headers on the underside of the PCB, and then soldered male headers to the ESP32, pointing upward, as shown below:

<img src="https://github.com/user-attachments/assets/cc2036b7-1e73-4152-8374-12dc13e242a6" alt="ESP32 - Soldered Male Headers" width="300"/>
<img src="https://github.com/user-attachments/assets/fef95601-7ecc-4d42-803e-0c314522f83c" alt="PCB Design - Soldering Female Headers for the ESP32" width="300"/>
<img src="https://github.com/user-attachments/assets/593378e7-201e-4953-85ce-5572a0c072ec" alt="PCB Design - Soldering Female Headers for the CC1101" width="300"/>
<br><br>

Once this was completed, I moved on to the TP4056 module and the JST connector. I soldered the positive and negative wires from the battery to the appropriate terminals on the TP4056. Next, I soldered two smaller wires to the `OUT-` and `OUT+` terminals for the power output:

<img src="https://github.com/user-attachments/assets/e312fadd-b0e5-4b4b-8dab-f72c677161c7" alt="TP4056 - Parts" width="200"/>
<img src="https://github.com/user-attachments/assets/10081ce9-1cf1-4d07-acb1-4da4f73451be" alt="TP4056 - Soldered BAT" width="200"/>
<img src="https://github.com/user-attachments/assets/0b450a1b-9914-4158-bd7f-bc8540aa91fb" alt="TP4056 - Soldered OUT" width="200"/>
<br><br>

To secure the latching power button, I used double-sided tape and attached it on the side the female headers for convenience (had to get creative with it 😅). I soldered the positive wire from the TP4056 to one end of the button, and then added a red wire to the other end. The negative wire from the TP4056 was soldered directly to the negative SMD pad on the PCB, while the positive wire (from the other end of the button) was soldered to the positive SMD pad:

<img src="https://github.com/user-attachments/assets/7788cf8b-7e10-4626-82c8-64b0edb961a1" alt="TP4056 Positive Wire" width="300"/>
<img src="https://github.com/user-attachments/assets/02bee769-b71e-4ed9-83e2-5dbedaa40158" alt="Latching Button (Positive)" width="300"/>
<img src="https://github.com/user-attachments/assets/9f4eca2c-7fbf-4c47-aaf6-9f3db86d3e48" alt="Final Result" width="300"/>
<br><br>

> [!TIP]
> **This button design allows the device to be powered on and off, helping to preserve battery life and prevent interference with other devices.**

Finally, I connected the battery to the JST connector, and that's it! We've successfully built our own BKFZ SubGHz device.

<img src="https://github.com/user-attachments/assets/f2be327c-99f0-479f-b73d-a1b742eece25" alt="Final Result w/ Battery" width="450"/>

### Photos
This project utilized the Wemos D1 Mini symbol and footprint by [@r0oland](https://github.com/r0oland/ESP32_mini_KiCad_Library), as well as the TP4056 battery module footprint by [@ccadic](https://github.com/ccadic/TP4056-18650). The CC1101 symbol and footprint was designed by me, as I was unable to find it online anywhere.

#### Schematic
<img src="https://github.com/user-attachments/assets/2ef62ee9-d988-4840-83af-1605c1f3d801" alt="Schematic" width="600"/>

#### PCB Design
<img src="https://github.com/user-attachments/assets/6347270d-f7c1-4772-a043-a6090f3d70bd" alt="PCB Design" width="200"/>
<br><br>

**If you'd like to see detailed photos of each step in the assembly process, you can check them out [here](https://github.com/BrianWalczak/BKFZ-SubGHz/tree/main/Other/Assembly%20Photos).**
