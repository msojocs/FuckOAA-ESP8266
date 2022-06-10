#include "oaa.h"

OAA::OAA(/* args */)
{
}

OAA::~OAA()
{
}
/**
 * 使用TGC登录教务处
 * */
int OAA::loginByTGC(String tgc){
    WiFiClient httpClient;
    WiFiClientSecure httpsClient;
    HTTPClient http;
    const char *header[] = {"set-cookie"};
    String t, location;
    httpsClient.setInsecure();

    // 请求session
    http.begin(httpClient, "http://jwgl.cuit.edu.cn/eams/home.action");
    http.addHeader("cookie", "JSESSIONID=" + this->OAA_SESSION);
    http.collectHeaders(header, 1);
    int httpCode = http.GET();
    if(httpCode < 0){
        return -1;
    }
    if(httpCode == HTTP_CODE_OK){
        // 已经登录
        return 1;
    }
    t = http.header("set-cookie");
    Serial.println("set-cookie: " + t);
    int start = t.indexOf("JSESSIONID=");
    if(start != -1){
        start += 11;
        int end = t.indexOf(";", start);
        this->OAA_SESSION = t.substring(start, end);
    }
    Serial.println("OAA_SESSION: " + this->OAA_SESSION);
    http.end();

    // get ticket
    http.begin(httpsClient, "https://sso.cuit.edu.cn/authserver/login?service=http://jwgl.cuit.edu.cn/eams/login.action;jsessionid=" + this->OAA_SESSION);
    http.addHeader("cookie", tgc);
    httpCode = http.GET();
    if(httpCode != HTTP_CODE_FOUND){
        Serial.println("get ticket响应码" + String(httpCode) + "不正确，期望收到302");
        return -1;
    }
    location = http.getLocation();
    Serial.println("location: " + location);
    http.end();

    // active1
    http.begin(httpClient, location);
    http.addHeader("cookie", "JSESSIONID=" + this->OAA_SESSION);
    httpCode = http.GET();
    if(httpCode != HTTP_CODE_FOUND){
        Serial.println("active响应码" + String(httpCode) + "不正确，期望收到302");
        return -1;
    }
    location = http.getLocation();
    Serial.println("active1 location: " + location);
    http.end();

    // active2
    http.begin(httpClient, location);
    http.addHeader("cookie", "JSESSIONID=" + this->OAA_SESSION);
    httpCode = http.GET();
    if(httpCode != HTTP_CODE_FOUND){
        Serial.println("active响应码" + String(httpCode) + "不正确，期望收到302");
        return -1;
    }
    location = http.getLocation();
    Serial.println("active2 location: " + location);
    http.end();
    int pos = location.indexOf("/eams/home.action");
    if(pos == -1){
        return -1;
    }
    globalConfig["OAA_SESSION"] = this->OAA_SESSION;
    Storage_SaveConfig();
    return 1;
}