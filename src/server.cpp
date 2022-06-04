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
    request->send(404, "text/plain", "Not found");
}

void Server_Start()
{
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        // Serial.println("index.html");
        response->setCode(200);
        if(LittleFS.begin()){
            Serial.println("read");
            File file = LittleFS.open("/index.html", "r");
            char buff[128] = {0};
            while (file.available())
            {
                int size = file.readBytes(buff, 128);
                response->write(buff, size);
            }

        }
        request->send(response);
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/startFuck", HTTP_GET, [] (AsyncWebServerRequest *request) {
        CUIT_Init(true);
        CUIT_STATUS = CUIT_OAA_CAPTCHA;
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
        if (request->hasParam("sid", true)) {
            globalConfig["sid"] = request->getParam("sid", true)->value();
        }
        if (request->hasParam("spass", true)) {
            globalConfig["spass"] = request->getParam("spass", true)->value();
        }
        if (request->hasParam("cookie", true)) {
            globalConfig["cookie"] = request->getParam("cookie", true)->value();
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