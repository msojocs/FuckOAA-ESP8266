#include "wifi.h"
#include "ui.h"
#include "storage.h"

typedef void (*WIFI_STATUS_CB)(void);
int WIFI_Status;

/**
 * WIFI控制
 * 
 * */

boolean WIFI_Connect(String ssid, String sspsk){
    Serial.println("try connect to " + ssid + ", " + sspsk);
    delay(100);
    Serial.println("begin");
    
    WiFi.begin(ssid, sspsk);
    
    delay(100);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        if(++i > 10)return false;
        Serial.print(".");
        delay(1000);
    }
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
    ui.setIP(WiFi.localIP().toString());
    
    return true;
}

boolean WIFI_CreateAP(String id = "Esp8266", String psk = "12345678"){
    Serial.println("WiFi not connected!");
    String apssid = id;
    String appsk = psk;
    bool createResult = WiFi.softAP(apssid, appsk);
    if(!createResult)return false;
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    ui.setIP(myIP.toString());
    return true;
}

void WIFI_UpdateSTA(){
    Serial.println("wifi 更新账户");
    
    WiFi.begin((String)globalConfig["sta"][0], (String)globalConfig["sta"][1]);
    WIFI_Status = 1;
}
void WIFI_Check(){
    if(WiFi.status() != WL_CONNECTED){
        // Serial.println("wifi 未链接");
        // 无线未链接
        if(WiFi.getMode() == WIFI_AP_STA)return;

        // Serial.println("wifi 启动热点");
        WiFi.mode(WIFI_AP_STA);
        WIFI_Connect(globalConfig["sta"][0], globalConfig["sta"][1]);
        WIFI_CreateAP(globalConfig["ap"][0], globalConfig["ap"][1]);
    }else{
        // Serial.println("wifi 已链接");
        // 已链接
        if(WiFi.getMode() != WIFI_AP_STA)return;
        // Serial.println("wifi 切换为设备");
        WiFi.mode(WIFI_STA);
        ui.setIP(WiFi.localIP().toString());
    }
}
WIFI_STATUS_CB WIFI_Status_Callback[] = {WIFI_UpdateSTA, WIFI_Check};
/**
 * 无线监视器
 * 
 * */
void WIFI_Monitor(){
    WIFI_Status_Callback[WIFI_Status]();
}