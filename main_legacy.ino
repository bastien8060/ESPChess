#include <Arduino.h>

#include "FS.h"

#include "corelib.h"

#include "NetworkManagerLib.h"

#include "ChessDotCom.h"


void setup() {
    powermode(LOW);
    Serial.begin(115200);
    delay(1000);
    Serial.print("Starting main thread on #");
    Serial.println(xPortGetCoreID());
    NetworkManagerStart();
    //benchmark();
    ChessComLogin();
    ChessComCreateGame();
}

void loop() {

}
