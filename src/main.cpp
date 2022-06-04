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
    delay(1000);
    Storage_ReadConfig();
    
    Serial.println("wifi...");
    WiFi.mode(WIFI_STA);
    Serial.println("connecting...");

    // WiFiMulti.addAP(globalConfig["ap"][0], globalConfig["ap"][1]);
    WIFI_Status = 1;

    if (WiFiMulti.run(5000) == WL_CONNECTED)
    {
        Serial.print("WiFi connected: ");
        Serial.print(WiFi.SSID());
        Serial.print(" ");
        Serial.println(WiFi.localIP());
        ui.setIP(WiFi.localIP().toString());
    }
    else
    {
        if (!globalConfig.containsKey("ap"))
        {
            Serial.println("ap lost...");
        }
        Serial.println("WiFi not connected!");
        WiFi.mode(WIFI_AP_STA);
        String apssid = globalConfig["ap"][0] | "Esp8266";
        String appsk = globalConfig["ap"][1] | "12345678";
        WIFI_CreateAP(apssid, appsk);
    }
    Server_Start();
}

void loop()
{
    WIFI_Monitor();
    ui.setNum(globalConfig["sid"]);
    delay(1000);
    return;
    String cookie = globalConfig["cookie"];
    String captcha;
    String profiledId = globalConfig["profiled_id"];

    CUIT cuit(cookie, globalConfig["ocr_server"]);
    Serial.println("getPic");
    while (true)
    {
        cuit.getPic();
        delay(100);

        Serial.println("postOCRPic");
        captcha = cuit.postOCRPic();
        Serial.println("postOCRPic: " + captcha);
        delay(100);

        boolean result = cuit.checkCaptcha(captcha, profiledId);
        Serial.println("checkCaptcha: " + String(result));
        if (result)
            break;
        delay(1000);
    }

    // 检测开放状态
    while (true)
    {
        boolean result = cuit.isAvailable(captcha, profiledId);
        Serial.println("isAvailable: " + String(result));
        if (result)
            break;

        delay(1000);
    }

    // 获取lessonId
    Serial.println("获取lessonId");
    String lessonId = cuit.courseName2Id(profiledId, globalConfig["course_name"]);
    Serial.println("获取lessonId: " + lessonId);
    delay(300);
    if (lessonId == "")
    {
        Serial.println("没有找到相关课程");
        delay(60000);
        return;
    }

    // 抢课
    Serial.println("抢课");
    while (true)
    {
        boolean result = cuit.fuckCourse(profiledId, lessonId);
        Serial.println("fuckCourse: " + String(result));
        if (result)
            break;
        delay(2000);
    }

    delay(60000);
}
