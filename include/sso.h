#ifndef __SSO_H__
#define __SSO_H__
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "storage.h"

/**
 * 统一登录中心处理类
 * 地址：https://sso.cuit.edu.cn/authserver/login
 * 
 * 
 * */
class SSO
{
private:
    /* 学号 */
    String id;
    // 密码
    String password;
    String SSO_Execution;
    String SSO_SESSION;
    String SSO_TGC = globalConfig["SSO_TGC"];
    boolean loginOK = false;
public:
    SSO(String id, String password);
    ~SSO();
    // 预登录准备，模拟第一次访问页面
    void prepare();
    /**
     * 获取验证码
     * 
     * @return 字符串（二进制）形式的验证码
     * */
    String getCaptcha();
    /**
     * 使用验证码登录统一中心
     * 
     * @param captchaCode 验证码
     * @return -2未知错误|1登录成功|2验证码无效|3用户名或密码错误
     * 
     * */
    int login(String& captchaCode);
    String getTGC();
    boolean isLogin();
};
void SSO_Login();
#endif