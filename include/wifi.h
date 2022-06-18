#ifndef __WIFI_H__
#define __WIFI_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>

extern int WIFI_Status;

boolean WIFI_Connect(String ssid, String sspsk);
boolean WIFI_CreateAP(String id, String psk);
void WIFI_Monitor();

#endif