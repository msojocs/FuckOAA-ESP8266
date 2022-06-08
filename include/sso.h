#ifndef __SSO_H__
#define __SSO_H__
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

class SSO
{
private:
    /* data */
    String id;
    String password;
    String SSO_Execution;
    String SSO_SESSION;
    String SSO_TGC;
public:
    SSO(String id, String password);
    ~SSO();
    void prepare();
    String getCaptcha();
    int login(String& captchaCode);
};
void SSO_Login();
#endif