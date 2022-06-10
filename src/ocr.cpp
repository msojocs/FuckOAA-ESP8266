#include "ocr.h"

/**
 * OCR识别验证码
 * 
 * @param captcha 字符串（二进制）形式验证码图像数据
 * @return 四位验证码字符
 * */
String OCR_postOCRPic(String& captcha){
    WiFiClient client;
    HTTPClient http;
    Serial.println("ocr server: " + (String)globalConfig["ocr_server"]);
    if((String)globalConfig["ocrServer"] == "")return "";
    delay(100);

    http.begin(client, globalConfig["ocr_server"]);
    http.addHeader("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundary8XsPye75Y6cInBRK");
    
    int httpCode = http.POST("------WebKitFormBoundary8XsPye75Y6cInBRK\r\nContent-Disposition: form-data; name=\"captcha\" ; filename=\"captcha.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n" + captcha + "\r\n------WebKitFormBoundary8XsPye75Y6cInBRK--");
    if(httpCode <= 0){
        Serial.printf("ocr 请求失败 code: %d \n", httpCode);
    }else{
        Serial.printf("ocr code: %d \n", httpCode);
        if(httpCode == HTTP_CODE_OK){
            String result = http.getString();
            // Serial.println(result);
            
            DynamicJsonDocument doc(1024);
            JsonObject json;
            deserializeJson(doc, result);
            json = doc.as<JsonObject>();
            http.end();
            return json["result"];
        }
    }
    http.end();
    return "";
}