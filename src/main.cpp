/**
   StreamHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include "ui.h"
#include "LittleFS.h"
#include "storage.h"
#include "server.h"
#include "cuit.h"

ESP8266WiFiMulti WiFiMulti;
UI ui;

void setup() {
  ui.init();
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  Storage_ReadConfig();
  Serial.println("wifi...");
  WiFi.mode(WIFI_STA);
  Serial.println("connecting0...");
  
  Serial.println("check...");
  if(!globalConfig.containsKey("ap")){
    Serial.println("ap lost...");
    return ;
  }
  WiFiMulti.addAP(globalConfig["ap"][0], globalConfig["ap"][1]);
  
  if (WiFiMulti.run(5000) == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
    ui.setIP(WiFi.localIP().toString());
  } else {
    Serial.println("WiFi not connected!");
    WiFi.mode(WIFI_AP);
    String apssid = globalConfig["sta"][0] | "Esp8266";
    String appsk = globalConfig["sta"][1] | "12345678";
    WiFi.softAP(apssid, appsk);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    ui.setIP(myIP.toString());
  }
  // Server_Start();
}

void loop() {
  ui.setNum(globalConfig["id"]);
  // Server_HandleClient();
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
    if(result)break;
    delay(1000);
  }
  
  // 检测开放状态
  while (true)
  {
    boolean result = cuit.isAvailable(captcha, profiledId);
    Serial.println("isAvailable: " + String(result));
    if(result)break;

    delay(1000);
  }
  
  // 获取lessonId
  Serial.println("获取lessonId");
  String lessonId = cuit.courseName2Id(profiledId, globalConfig["course_name"]);
  Serial.println("获取lessonId: " + lessonId);
  delay(300);
  if(lessonId == ""){
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
    if(result)break;
    delay(2000);
  }
  
  delay(60000);
  
}
