#include "WiFi.h"

#include "FS.h"

#include "SPIFFS.h"

int ScanWifi() { //scan wifi and return number of Wi-Fis
    Serial.println("Scanning Wifi");
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.printf("%i networks found \n",n);
        return n;
    }
    Serial.println("");

}

void enableOTA(){//enable OTA Service
    ArduinoOTA.setPort(5555);
    ArduinoOTA.setHostname("LouisChessboard");
    ArduinoOTA.setPasswordHash("67524210524b62ad06b8fc7c6dc7135e");
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  ota_enabled = true;
}

void connectToWiFi(String ssid_str, String pwd_str) {//Connect to wifi from ssid + pwd.
    int attempt = 0;
    const char * ssid = ssid_str.c_str();
    const char * pwd = pwd_str.c_str();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    Serial.printf("Connecting to '%s'\n",ssid,pwd);

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
    wifi_enabled = true;
    enableOTA();
}

void NetworkManagerStart() { //start network manager

    String ssid;
    String pwd;
    bool inited = true;

    /*bool formatted = SPIFFS.format();
      if(formatted){
          Serial.println("\n\nSuccess formatting");
      }else{
          Serial.println("\n\nError formatting");
      }
    */


    Serial.println("Checking Network Creds...");
    if (!SPIFFS.exists("/wifi/pwd") || !SPIFFS.exists("/wifi/ssid")) { //check if saved ssid and password exist
        inited = false;
    }


    if (!inited) { //if creds are not saved, connect with keyboard.
        bool ssidtyped = false;
        bool pwdtyped = false;
        int n = ScanWifi();
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            String this_ssid = WiFi.SSID(i);
            Serial.printf("%i: %s (%i) ",i,this_ssid,WiFi.RSSI(i));
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }

        Serial.println("Going To Open Keyboard...");      
  
        String incomingByte = wifi_choose_list(n);
        Serial.println("Got ssid");
        int id = incomingByte.toInt();
        ssid = WiFi.SSID(id);

        // send data only when you receive data:
        Serial.print("Enter Password for ");
        Serial.println(ssid);

        String msg = "Enter Password for ";
        msg += ssid;
        
        String incomingBytePwd = keyboard(msg);
        pwd = incomingBytePwd;
        pwd.trim();
        Serial.printf("Connecting to '%s' with '%s'\n",ssid,pwd);
                
        //save ssid
        File file1 = SPIFFS.open("/wifi/ssid", "w");
        if (!file1) {
            Serial.println("Failed to open ssid file.");
            return;
        } else {
            file1.print(ssid);
            file1.close();
        }
        //save pwd.
        File file2 = SPIFFS.open("/wifi/pwd", "w");
        if (!file2) {
            Serial.println("Failed to open password file");
            return;
        } else {
            file2.print(pwd);
            file2.close();
        }
    } else { //otherwise, retrieve pwd + ssid and connect.
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
    connectToWiFi(ssid, pwd); //connect to wifi
}
