#ifndef __CUIT_H__
#define __CUIT_H__
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <iostream>

class CUIT
{
private:
    /* data */
    String cookie;
    String ocrServer;
    String pic;
public:
    CUIT(String cookie, String ocrServer);
    ~CUIT();

    void getPic();
    String postOCRPic();
    boolean checkCaptcha(String captcha, String profiledId);
    String courseName2Id(String profiledId, String name);
    boolean isAvailable(String captcha, String profiledId);
    boolean fuckCourse(String profiledId, String lessonId);
};
#endif