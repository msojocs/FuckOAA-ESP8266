#include "server.h"
#include "wifi.h"
#include "storage.h"
#include "cuit.h"

/**
 * https://github.com/me-no-dev/ESPAsyncWebServer#important-things-to-remember
 * 异步服务器内部不能有delay yield
 * 
 * */
AsyncWebServer server(80);


const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    Serial.println("url: " + request->url());
    request->send(404, "text/plain", "Not found");
}

void Server_Start()
{
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.serveStatic("/", LittleFS, "/web/").setDefaultFile("index.html");

    // Send a GET request to <IP>/get?message=<message>
    server.on("/startFuck", HTTP_GET, [] (AsyncWebServerRequest *request) {
        CUIT_STATUS = CUIT_SSO_LOGIN;
        request->send(200, "text/plain", "Hello ");
        // request->send(200, "text/plain", "Hello " + String(test));
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });
    
    server.on("/saveConfig", HTTP_POST, [](AsyncWebServerRequest *request){
        if(!LittleFS.begin()){
            request->send(500, "text/plain; charset=utf-8", "文件系统错误！");
            return ;
        }
        if (request->hasParam("wifiName", true)) {
            globalConfig["sta"][0] = request->getParam("wifiName", true)->value();
            WIFI_Status = 0;
        }
        if (request->hasParam("wifiPass", true)) {
            globalConfig["sta"][1] = request->getParam("wifiPass", true)->value();
            WIFI_Status = 0;
        }
        if (request->hasParam("apName", true)) {
            globalConfig["ap"][0] = request->getParam("apName", true)->value();
        }
        if (request->hasParam("apPass", true)) {
            globalConfig["ap"][1] = request->getParam("apPass", true)->value();
        }
        if (request->hasParam("sid", true)) {
            globalConfig["sid"] = request->getParam("sid", true)->value();
        }
        if (request->hasParam("spass", true)) {
            globalConfig["spass"] = request->getParam("spass", true)->value();
        }
        if(request->hasParam("sid", true) || request->hasParam("spass", true)){
            globalConfig["SSO_TGC"] = globalConfig["OAA_SESSION"] = "";
        }
        if (request->hasParam("profiledId", true)) {
            globalConfig["profiled_id"] = request->getParam("profiledId", true)->value();
        }
        if (request->hasParam("courseName", true)) {
            globalConfig["course_name"] = request->getParam("courseName", true)->value();
        }
        if (request->hasParam("ocrServer", true)) {
            globalConfig["ocr_server"] = request->getParam("ocrServer", true)->value();
        }
        Storage_SaveConfig();
        
        request->send(200, "text/plain; charset=utf-8", "已保存");
    });
    server.on("/getConfig", HTTP_GET, [](AsyncWebServerRequest *request){
        String configStr;
        serializeJson(globalConfig, configStr);
        request->send(200, "application/json; charset=utf-8", configStr);
    });

    server.onNotFound(notFound);

    server.begin();
}