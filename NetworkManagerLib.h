#include "WiFi.h"

#include "FS.h"

#include "SPIFFS.h"

int ScanWifi() {
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        return n;
    }
    Serial.println("");

}

void connectToWiFi(String ssid_str, String pwd_str) {
    int attempt = 0;
    const char * ssid = ssid_str.c_str();
    const char * pwd = pwd_str.c_str();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);

        if ((++i % 16) == 0) {
            attempt += 1;
            Serial.println(F(" still trying to connect"));
        }
        if (attempt == 3) {
            SPIFFS.format();
            ESP.restart();
        }
    }

    Serial.print(F("Connected. My IP address is: "));
    Serial.println(WiFi.localIP());
}

void NetworkManagerStart() {
    String ssid;
    String pwd;
    bool inited = true;

    if (!SPIFFS.begin()) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        bool formatted = SPIFFS.format();
        if (formatted) {
            Serial.println("\n\nSuccess formatting");
        } else {
            Serial.println("\n\nError formatting");
        }
    }

    /*bool formatted = SPIFFS.format();
      if(formatted){
          Serial.println("\n\nSuccess formatting");
      }else{
          Serial.println("\n\nError formatting");
      }
    */

    if (!SPIFFS.exists("/wifi/pwd")) {
        inited = false;
    }
    if (!SPIFFS.exists("/wifi/ssid")) {
        inited = false;
    }

    if (!inited) {
        bool ssidtyped = false;
        bool pwdtyped = false;
        int n = ScanWifi();
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }

        while (!ssidtyped) {
            if (Serial.available() > 0) {
                String incomingByte = Serial.readString();
                Serial.println("Got ssid");
                int id = incomingByte.toInt();
                ssid = WiFi.SSID(id);
                ssidtyped = true;
            }
        }

        // send data only when you receive data:
        Serial.print("Enter Password for ");
        Serial.println(ssid);

        while (!pwdtyped) {
            if (Serial.available() > 0) {
                String incomingByte = Serial.readString();
                pwd = incomingByte;
                pwd.trim();
                Serial.print("Connecting to >>");
                Serial.print(ssid);
                Serial.print("<< with >>");
                Serial.print(pwd);
                Serial.println("<<");
                pwdtyped = true;
            }
        }
        File file1 = SPIFFS.open("/wifi/ssid", "w");
        if (!file1) {
            // File not found
            Serial.println("Failed to open test file.");
            return;
        } else {
            file1.print(ssid);
            file1.close();
        }
        File file2 = SPIFFS.open("/wifi/pwd", "w");
        if (!file2) {
            // File not found
            Serial.println("Failed to open test file");
            return;
        } else {
            file2.print(pwd);
            file2.close();
        }
    } else {
        File ssid_file = SPIFFS.open("/wifi/ssid");
        File pwd_file = SPIFFS.open("/wifi/pwd");
        while (ssid_file.available()) {
            ssid = ssid_file.readString();
        }
        while (pwd_file.available()) {
            pwd = pwd_file.readString();
        }
        ssid_file.close();
        pwd_file.close();
    }
    connectToWiFi(ssid, pwd);
}
