#ifndef __CUIT_H__
#define __CUIT_H__

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <iostream>

typedef enum CuitStatus 
{
    CUIT_STATUS_COUNT = 5, CUIT_NO_ACTION = 0, CUIT_SSO_LOGIN = 1, CUIT_OAA_LOGIN = 2, CUIT_OAA_CAPTCHA = 3, CUIT_OAA_FUCK = 4
} CuitStatus_t;

class CUIT
{
private:
    /* data */
    String cookie;
    String ocrServer;
    String captchaCode;
public:
    CUIT(String ocrServer);
    ~CUIT();

    void setCookie(String cookie);
    void setOCRServer(String ocrServer);
    String getCaptcha();
    String postOCRPic(String captcha);
    boolean checkCaptcha(String captcha, String profiledId);
    String courseName2Id(String profiledId, String name);
    boolean isAvailable(String profiledId);
    boolean fuckCourse(String profiledId, String lessonId);
};

extern CUIT cuit;
extern CuitStatus_t CUIT_STATUS;

void CUIT_Init(boolean onlyData = false);
void CUIT_Monitor();
#endif