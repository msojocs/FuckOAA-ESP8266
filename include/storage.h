#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <ArduinoJson.h>
#include <LittleFS.h>

extern JsonObject globalConfig;

/**
 * 读取配置
 * */
boolean Storage_ReadConfig();

/**
 * 保存配置
 * */
void Storage_SaveConfig();

#endif