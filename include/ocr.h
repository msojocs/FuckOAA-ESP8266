#ifndef __OCR_H__
#define __OCR_H__
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "storage.h"

String OCR_postOCRPic(String& captcha);
#endif