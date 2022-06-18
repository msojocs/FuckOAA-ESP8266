#ifndef __OCR_H__
#define __OCR_H__
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "storage.h"

/**
 * OCR识别验证码
 * 
 * @param captcha 验证码数据
 * 
 * @return String 验证码识别结果
 * 
 * */
String OCR_postOCRPic(String& captcha);

#endif
