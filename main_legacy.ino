#include <Arduino.h>

#include <SPI.h>
#include <SD.h>
#include "Adafruit_RA8875.h"
#include <Adafruit_STMPE610.h>
#include <Adafruit_GFX.h>
#include <Wire.h>   

#define sd_cs 3
#define RA8875_INT 2
#define RA8875_CS 5
#define RA8875_RESET 4
#define FS_NO_GLOBALS
#include <FS.h>
#ifdef ESP32
  #include "SPIFFS.h"
#endif  

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

#include "corelib.h" //Core

#include "bmpdraw.h" //contains function to draw bmp

#include "UISet.h"  //contains most UI

#include "FS.h"    //FS

#include "NetworkManagerLib.h" //Network Manager, Wi-Fi

#include "ChessDotCom.h"  //Chess.com Library

void chess_setup() { //functions we want to call to correctly run a game, from scratch, including connection to chess.com account.
                     //this function is not called.
    powermode(LOW);
    delay(1000);
    Serial.print("Starting main thread on #");
    Serial.println(xPortGetCoreID());
    NetworkManagerStart();
    //benchmark();
    ChessComLogin();
    ChessComCreateGame();
}

void setup() {
    Serial.begin(115200); //start serial

    if (!SPIFFS.begin(true)) { //mount user storage
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    Serial.println("initialization done.");

    //while (!Serial) {;}
    Serial.println("RA8875 start");

    tft.begin(RA8875_800x480); //initialize TFT library

    tft.displayOn(true);
    tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    tft.PWM1out(255);

    pinMode(RA8875_INT, INPUT);
    digitalWrite(RA8875_INT, HIGH);

    tft.touchEnable(true); //enable Touch support!

    tsCalibrate(); //calibrate touchscreen
    
    splashscreen(); //Show Splashscreen while loading.
    
    brightness(255); //brightness to max 
    
    loadbmp(); //preload big bmp from storage
    
    //powermode(LOW);
    NetworkManagerStart(); //start networkmanager and connect to wifi
    interface(); //show interface
}

void loop() {
    if (ota_enabled){
      ArduinoOTA.handle(); //Over The Air Wi-Fi Updates
    }
    
    if (tft.touched()) { //check if touch detected
        if ((millis() - lastDebounceTime) > debounceDelay) { //check how long ago was last touch to debounce touchscreen.
          
            lastDebounceTime = millis(); //record time of last touch.
            
            tsPoint_t calibrated = getCalibratedTouch();
          
            /* Draw a single pixel at the calibrated point */
            tx = calibrated.x;
            ty = calibrated.y;
            
            tx = 800 - tx;
            ty = 480 - ty;

            button_newgame = ((tx > 60 && tx < 190 && ty >= 30 && ty <= 160) && screen == 0); //get boolean value of if `new game` button was pressed.

            if (screen == 1) { //if in `playing` screen, use the function `handle_board_touch` to handle touch event.
                handle_board_touch(tx, ty);
            } else if (button_newgame) { //if `new game` button is pressed, then go to `playing` screen
                chessboard();
            }
        }
    }
}
