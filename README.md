# ESPChess

---
 **NOTE**
 
This Project is a work in progress and is nowhere near being finished nor fully functional at this stage. This project right now is merely usable as is. This is a huge project, and I hope to be nearly done by the 20th of June.

---

Online MultiThreaded Chess Playing Library for ESP32.

This library has some easy to use c++ API which communicates over an HTTP api. This means that with little code, you can make an arduino play on chess.com.
This library includes some classes which can be initialised to login with your chess account and another one to create a game. 

This project uses a RA8875 Chip on a 7" LCD TFT Display with resistive touch from BuyDisplay.

This includes so far:
 - Keyboard Interface to Login to Wi-Fi
 - OTA Wi-Fi Updates
 - A Boot Logo
 - Simple WiFi Network Manager
 - A Simple GUI to create a game
 - A Fast Fen String Renderer

My deadline for the TFT screen to work and to have a proper GUI to play chess online (Using the ESP32): 20th June

## Wiring

This is the wiring done to connect an ESP32 to the RA8875. 

---
> **NOTE**
> 
> Mind that you will need the ESP32 to be powered by 5V or USB, so the screen gets required voltage. An alternative is to plug the Pin 4 to an external power supply instead of the VIN Port, or to use a boost converter to step up from 3.7V to 5V.

> Also mind that if using a boost converter to power the screen, the 3.7V Power Rail is likely not powerful enough to power the screen at full brightness. To reduce power usage, you may use `powermode(LOW);` in the setup function. This will reduce brightness and underclock the CPU to save energy.

---

| BuyDisplay RA8875 | ESP32 |
|:-------------:|:-------------:|
| Pin 2         |      GND      |
| Pin 4         |      VIN       |
| Pin 5         |      D5       |
| Pin 6         |      D19      |
| Pin 7         |      D23      |
| Pin 8         |      D18      |
| Pin 11        |      D4       |
| Pin 13        |      D2       |

## Running

The code should compile with ease. Only two things:
- You need to install the Libraries saved in the Library folder to the Arduino/Platformio IDE/plugin.
- You need to install https://github.com/me-no-dev/arduino-esp32fs-plugin. In order to Upload the Spiff from the Data Folder. Here are instructions: https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/.

Then compile and run + Ta-Da



