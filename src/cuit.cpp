#include "cuit.h"

CUIT::CUIT(String cookie, String ocrServer)
{
    this->cookie = cookie;
    this->ocrServer = ocrServer;
}

CUIT::~CUIT()
{
}

void CUIT::getPic(){
    String url = "http://jwgl.cuit.edu.cn/eams/captcha/image.action";
    WiFiClient client;
    HTTPClient http;

    http.begin(client, url);
    http.addHeader("cookie", this->cookie);
    int httpCode = http.GET();
    if(httpCode <= 0){
        // 请求失败，可能本地网络存在问题
        Serial.println("请求失败，可能本地网络存在问题");

    }else{
        // Serial.println("请求成功，状态码：" + String(httpCode));
        // 请求成功
        if(httpCode == HTTP_CODE_OK){
            // 状态码OK
            this->pic = http.getString();
        }
    }
    http.end();
}

String CUIT::postOCRPic(){
    WiFiClient client;
    HTTPClient http;

    http.begin(client, this->ocrServer);
    http.addHeader("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundary8XsPye75Y6cInBRK");
    
    int httpCode = http.POST("------WebKitFormBoundary8XsPye75Y6cInBRK\r\nContent-Disposition: form-data; name=\"captcha\" ; filename=\"captcha.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n" + this->pic + "\r\n------WebKitFormBoundary8XsPye75Y6cInBRK--");
    if(httpCode <= 0){
        Serial.printf("ocr 请求失败 code: %d \n", httpCode);
    }else{
        // Serial.printf("ocr code: %d \n", httpCode);
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

boolean CUIT::checkCaptcha(String captcha, String profiledId){
    String url = "http://jwgl.cuit.edu.cn/eams/stdElectCourse!defaultPage.action";
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
    http.addHeader("cookie", this->cookie);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.POST("captcha_response=" + captcha + "&electionProfile.id=" + profiledId);
    if(httpCode > 0){
        Serial.println("checkCaptcha code: " + String(httpCode));
        Serial.println("location: " + http.getLocation());
        if(httpCode == HTTP_CODE_OK)return true;
        if(http.getLocation().indexOf("sso") > 0){
            Serial.println("cookie失效");
            return false;
        }
    }
    return false;
}

String CUIT::courseName2Id(String profiledId, String courseName){
    delay(1);
    String url = "http://jwgl.cuit.edu.cn/eams/stdElectCourse!data.action?profileId=" + profiledId;
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
    http.addHeader("cookie", this->cookie);
    int httpCode = http.GET();
    String result = "";
    delay(1);
    if(httpCode == HTTP_CODE_OK){
        WiFiClient c = http.getStream();
        int len = http.getSize(); // 如果响应头没有 Content-Length 那么为 -1
        while (http.connected() && (len > 0 || len == -1))
        {
            // 注：正则匹配会触发看门狗
            delay(1);
            String t = c.readStringUntil('}');
            if(t == "")break;
            // Serial.println("读到的数据长度：" + String(t.length()));
            // Serial.println("len: " + String(len));
            delay(1);
            if (t.indexOf(courseName) > -1)
            {
                delay(1);
                int idStart = t.indexOf("id:");
                int idEnd = t.indexOf(",no");
                result = t.substring(idStart + 3, idEnd);
                break;
            }
            if(len > 0)
                len -= t.length();
            // Serial.println("没找到，len: " + String(len));
        }
    }
    http.end();
    delay(1);
    return result;
}

boolean CUIT::isAvailable(String captcha, String profiledId){
    String url = "http://jwgl.cuit.edu.cn/eams/stdElectCourse!defaultPage.action";
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
    http.addHeader("cookie", this->cookie);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    int httpCode = http.POST("captcha_response=" + captcha + "&electionProfile.id=" + profiledId);
    if(httpCode > 0){
        Serial.println("checkCaptcha code: " + String(httpCode));
        if(httpCode == HTTP_CODE_OK){
            int len = http.getSize();
            Serial.println("checkCaptcha len: " + String(len));
            char buff[128] = { 0 };
            while (http.connected() && (len > 0 || len == -1))
            {
                int c = client.readBytes(buff, std::min((size_t)len, (size_t)sizeof(buff)));
                // Serial.printf("readBytes: %d\n", c);
                // Serial.println(String(buff));
                if (!c) {
                    Serial.println("read timeout");
                    break;
                }

                // write it to Serial
                // TODO: 分段读取可能会导致 "不在选课时间内" 被拆分，导致判断错误
                if(String(buff).indexOf("不在选课时间内") > 0){
                    http.end();
                    return false;
                }

                if (len > 0) {
                    len -= c;
                }
                delay(1);
                if(c < 128)break;
            }
            http.end();
            
            return true;
        }
        if(http.getLocation().indexOf("sso") > 0){
            Serial.println("cookie失效");
            http.end();
            return false;
        }
    }
    http.end();
    return false;
}

boolean CUIT::fuckCourse(String profiledId, String lessonId){
    String url = "http://jwgl.cuit.edu.cn/eams/stdElectCourse!batchOperator.action?profileId=" + profiledId;
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
    
    http.addHeader("cookie", this->cookie);
    http.addHeader("X-Requested-With", "XMLHttpRequest");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Referer", "http://jwgl.cuit.edu.cn/eams/stdElectCourse!batchOperator.action?profileId=" + profiledId);
    http.setUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4147.0 Safari/537.36 Edg/84.0.521.0");

    String data = "optype=true&operator0=" + lessonId + "%3Atrue%3A0&lesson0=" + lessonId + "&schLessonGroup_" + lessonId + "=undefined";
    int httpCode = http.POST(data);
    if(httpCode > 0){
        if(httpCode == HTTP_CODE_OK){
            String str = client.readStringUntil('/');
            int start = str.indexOf("margin:auto;");
            str = str.substring(start + 15, str.length() - 1);
            Serial.println(str);
            if(str.indexOf("已经选过") > -1)return true;
        }
    }
    return false;
}