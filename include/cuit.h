#ifndef __CUIT_H__
#define __CUIT_H__

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "ocr.h"

// 成信大状态定义
typedef enum CuitStatus 
{
    // 状态总数
    CUIT_STATUS_COUNT = 5,
    // 无操作状态
    CUIT_NO_ACTION = 0,
    // 统一中心登录状态
    CUIT_SSO_LOGIN = 1,
    // 教务处登录状态
    CUIT_OAA_LOGIN = 2,
    // 教务处验证码处理状态
    CUIT_OAA_CAPTCHA = 3,
    // 教务处抢课状态
    CUIT_OAA_FUCK = 4
} CuitStatus_t;

extern CuitStatus_t CUIT_STATUS;

// 成信大初始化操作
void CUIT_Init();

// 成信大监视器
void CUIT_Monitor();
#endif