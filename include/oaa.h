#ifndef __OAA_H__
#define __OAA_H__

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "storage.h"

/**
 * 教务处处理类
 * 地址：http://jwgl.cuit.edu.cn
 * 
 * 
 * */
class OAA
{
private:
    /* data */
    String OAA_SESSION = globalConfig["OAA_SESSION"];
public:
    OAA(/* args */);
    ~OAA();
    int loginByTGC(String tgc);
};


#endif