#include "sso.h"
#include "ocr.h"
#include "oaa.h"

SSO::SSO(String id, String password)
{
    this->id = id;
    this->password = password;
}

SSO::~SSO()
{
}
void SSO::prepare(){
    WiFiClientSecure client;
    HTTPClient https;
    String url = "https://sso.cuit.edu.cn/authserver/login";
    const char* header[] = {"set-cookie"};
    client.setInsecure();
    https.begin(client, url);
    https.addHeader("cookie", this->SSO_SESSION + ";" + this->SSO_TGC);
    https.setUserAgent("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.5005.63 Safari/537.36 Edg/102.0.1245.33");
    https.collectHeaders(header, 1);
    int httpCode = https.GET();
    String buffer;
    if(httpCode > 0){
        if(httpCode == HTTP_CODE_OK){
            String cookie = https.header("set-cookie");
            int start;
            if((start = cookie.indexOf("SESSION"))> 0){
                int end = cookie.indexOf(";", start);
                this->SSO_SESSION = cookie.substring(start, end);
                Serial.println("get session: " + this->SSO_SESSION);
            }
            while (client.available())
            {
                buffer = client.readStringUntil('>');
                if(buffer.indexOf("登录成功") != -1){
                    // 已登录状态
                    this->loginOK = true;
                    break;
                }
                if(buffer.indexOf("execution") != -1){
                    int start = buffer.indexOf("value=\"");
                    int end = buffer.indexOf("\"", start + 7);
                    this->SSO_Execution = buffer.substring(start + 7, end);
                    Serial.println("execution: " + this->SSO_Execution);
                    break;
                }
            }
            
        }else{
            Serial.println("login code: " + String(httpCode));
        }
    }
    https.end();
}
String SSO::getCaptcha(){
    // const uint8_t fingerprint[20] = {0x4A, 0x4E, 0x56, 0x26, 0x21, 0x8E, 0x8F, 0xCB, 0xB6, 0x89, 0x64, 0x97, 0x76, 0x84, 0x0C, 0xA7, 0x7B, 0x75, 0x9D, 0xE7};
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    // WiFiClientSecure client;
    // client->setFingerprint(fingerprint);
    client->setInsecure();
    HTTPClient https;
    String url = "https://sso.cuit.edu.cn/authserver/captcha";
    https.setUserAgent("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.5005.63 Safari/537.36 Edg/102.0.1245.33");
    https.setTimeout(1000);
    String captcha = "";
    delay(10);
    const char* header[] = {"set-cookie"};
    if(https.begin(*client, url)){
        https.addHeader("cookie", this->SSO_SESSION);
        Serial.print("[HTTPS] GET...\n");
        delay(10);
        https.collectHeaders(header, 1);
        int httpCode = https.GET();
        Serial.println("图片响应码: " + String(httpCode));
        if(httpCode > 0){
            if(httpCode == HTTP_CODE_OK){
                delay(10);
                String cookie = https.header("set-cookie");
                Serial.println("get cookie: " + cookie);
                // cookie.
                int start;
                if((start = cookie.indexOf("SESSION"))> 0){
                    int end = cookie.indexOf(";", start);
                    this->SSO_SESSION = cookie.substring(start, end);
                    Serial.println("get session: " + this->SSO_SESSION);
                }
                captcha = https.getString();
                
            }
        }
        https.end();
    }
    return captcha;
}

/**
 * 使用验证码登录统一中心
 * 
 * @param captchaCode 验证码
 * @return -2未知错误|1登录成功|2验证码无效|3用户名或密码错误|4锁定
 * 
 * */
int SSO::login(String& captchaCode){
    WiFiClientSecure client;
    HTTPClient https;
    String url = "https://sso.cuit.edu.cn/authserver/login";
    const char* header[] = {"set-cookie"};
    client.setInsecure();
    https.begin(client, url);
    https.addHeader("Cookie", this->SSO_SESSION );
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");
    https.addHeader("Referer", "https://sso.cuit.edu.cn/authserver/login");
    https.setUserAgent("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.5005.63 Safari/537.36 Edg/102.0.1245.33");
    https.collectHeaders(header, 1);
    String data = "execution=" + this->SSO_Execution + "&_eventId=submit&lm=usernameLogin&geolocation=&username=" + this->id + "&password=" + this->password + "&captcha=" + captchaCode;
    Serial.println("login data: " + data);
    int httpCode = https.POST(data);
    Serial.println("响应码：" + String(httpCode));
    String buffer;
    if(httpCode > 0){
        if(httpCode == HTTP_CODE_OK){
            while (client.available())
            {
                buffer = client.readStringUntil('>');
                if(buffer.indexOf("textError") != -1){
                    client.readStringUntil('>');
                    buffer = client.readStringUntil('>');
                    // 有错误信息
                    int end = buffer.indexOf("<");
                    Serial.println(buffer.substring(0, end));
                }
                if(buffer.indexOf("登录成功") != -1){
                    String cookie = https.header("set-cookie");
                    Serial.println("get cookie: " + cookie);
                    int start = cookie.indexOf("TGC");
                    int end = cookie.indexOf(";", start);
                    this->SSO_TGC = cookie.substring(start, end);
                    Serial.println("TGC: " + this->SSO_TGC);
                    https.end();
                    globalConfig["SSO_TGC"] = this->SSO_TGC;
                    Storage_SaveConfig();
                    this->loginOK = true;
                    return 1;
                }else if(buffer.indexOf("验证码无效") != -1){
                    https.end();
                    return 2;
                }else if(buffer.indexOf("密码错误") != -1){
                    // 用户名或密码错误
                    https.end();
                    return 3;
                }else if(buffer.indexOf("账号被锁定") != -1){
                    // 账号被锁定
                    https.end();
                    return 4;
                }
                if(buffer.indexOf("execution") != -1){
                    int start = buffer.indexOf("value=\"");
                    int end = buffer.indexOf("\"", start + 7);
                    this->SSO_Execution = buffer.substring(start + 7, end);
                    Serial.println("更新 execution: " + this->SSO_Execution);
                }
            }
            
        }else{
            Serial.println("login code: " + String(httpCode));
            https.end();
            return -1;
        }
    }
    https.end();
    return -2;
}

String SSO::getTGC(){
    return this->SSO_TGC;
}
boolean SSO::isLogin(){
    return this->loginOK;
}
