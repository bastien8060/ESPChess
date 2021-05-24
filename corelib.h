#include "PrintStream.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

bool ota_enabled = false;

void powermode(int mode) { //change cpu clock speed and tft brightness for battery saving.
    if (mode == LOW) {
        tft.brightness(60);
        setCpuFrequencyMhz(80);
    }
    if (mode == HIGH) {
        tft.brightness(255);
        setCpuFrequencyMhz(240);
    }
}
