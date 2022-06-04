/**
   StreamHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include "ui.h"
#include "LittleFS.h"
#include "storage.h"
#include "server.h"
#include "cuit.h"
#include "wifi.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>


ESP8266WiFiMulti WiFiMulti;

void setup()
{
    ui.init();
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("\r\n\r\n\r\n");
    delay(2000);
    Storage_ReadConfig();
    
    Serial.println("wifi...");
    WiFi.mode(WIFI_STA);
    Serial.println("connecting...");

    WiFiMulti.addAP(globalConfig["sta"][0], globalConfig["sta"][1]);
    WIFI_Status = 1;

    if (WiFiMulti.run(5000) == WL_CONNECTED)
    {
        Serial.print("WiFi connected: ");
        Serial.print(WiFi.SSID());
        Serial.print(" ");
        Serial.println(WiFi.localIP());
        ui.setIP(WiFi.localIP().toString());
        ui.addNotice("WIFI已连接 ^_^");
    }
    else
    {
        if (!globalConfig.containsKey("ap"))
        {
            Serial.println("ap lost...");
        }
        ui.addNotice("创建热点 >_<");
        Serial.println("WiFi not connected!");
        WiFi.mode(WIFI_AP_STA);
        String apssid = globalConfig["ap"][0] | "Esp8266";
        String appsk = globalConfig["ap"][1] | "12345678";
        WIFI_CreateAP(apssid, appsk);
    }
    Server_Start();
    CUIT_Init();
    ui.addNotice("初始化完毕!");
}

void loop()
{
    WIFI_Monitor();
    ui.setNum(globalConfig["sid"]);
    CUIT_Monitor();

    delay(100);
}
