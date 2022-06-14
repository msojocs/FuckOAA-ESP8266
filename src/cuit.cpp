#include "cuit.h"
#include "storage.h"
#include "ui.h"
#include "sso.h"
#include "oaa.h"

typedef void (*CUIT_STATUS_CB)(void);
CuitStatus_t CUIT_STATUS;


// ================================================================================
// ================================================================================
// ================================================================================
// ================================================================================
// ================================================================================


CUIT_STATUS_CB cuitStatusCB[CUIT_STATUS_COUNT] = {nullptr};
void CUIT_SSO_LOGIN_F(){
    ui.addNotice("登录统一认证中心");
    SSO sso(globalConfig["sid"], globalConfig["spass"]);
    sso.prepare();
    if(sso.isLogin()){
        CUIT_STATUS = CUIT_OAA_LOGIN;
        return;
    }
    if(!sso.isLogin()){
        int r;
        ui.addNotice("获取验证码");
        String captcha = sso.getCaptcha();
        Serial.println("tupian：" + captcha);
        String captchaCode = OCR_postOCRPic(captcha);
        Serial.println("SSO验证码识别结果：" + captchaCode);
        ui.addNotice("登录......");
        r = sso.login(captchaCode);
        Serial.println("登录结果：" + String(r));
        if(r == 1){
            ui.addNotice("登录成功");
            CUIT_STATUS = CUIT_OAA_LOGIN;
            return;
        }
        if(r == 3){
            ui.addNotice("用户名或密码错误");
            CUIT_STATUS = CUIT_NO_ACTION;
            return;
        }
        if(r == 4){
            ui.addNotice("账户锁定");
            CUIT_STATUS = CUIT_NO_ACTION;
            return;
        }
    }
    ui.addNotice("已登录");

}
void CUIT_OAA_LOGIN_F(){
    ui.addNotice("登录教务处");
    OAA oaa;
    int r = oaa.loginByTGC(globalConfig["SSO_TGC"]);
    if(r == 1){
        ui.addNotice("登录成功");
        CUIT_STATUS = CUIT_OAA_CAPTCHA;
        return;
    }
    ui.addNotice("登录失败");
}
void CUIT_OAA_CAPTCHA_F(){
    ui.addNotice("尝试获取验证码");
    OAA oaa;
    String captchaCode;
    while (true)
    {
        String captcha = oaa.getCaptcha();
        delay(100);

        Serial.println("postOCRPic");
        captchaCode = OCR_postOCRPic(captcha);
        Serial.println("postOCRPic: " + captchaCode);
        delay(100);

        boolean result = oaa.checkCaptcha(captchaCode, globalConfig["profiled_id"]);
        Serial.println("checkCaptcha: " + String(result));
        if (result){
            ui.addNotice("验证码识别成功");
            CUIT_STATUS = CUIT_OAA_FUCK;
            break;
        }
        if(CUIT_STATUS != CUIT_OAA_CAPTCHA){
            ui.addNotice("异常终止 :(");
            break;
        }
        delay(100);
    }
}
void CUIT_OAA_FUCK_F(){
    // 检测开放状态
    ui.addNotice("检测选课是否开放");
    OAA oaa;
    while (true)
    {
        boolean result = oaa.isAvailable(globalConfig["profiled_id"]);
        Serial.println("isAvailable: " + String(result));
        if (result){
            ui.addNotice("可以选课");
            break;
        }

        if(CUIT_STATUS != CUIT_OAA_FUCK){
            ui.addNotice("异常终止 :(");
            return;
        }
        ui.addNotice("暂时不能选课");
        delay(1000);
    }

    // 获取lessonId
    Serial.println("获取lessonId");
    ui.addNotice("获取" + (String)globalConfig["course_name"] + "的ID");
    String lessonId = oaa.courseName2Id(globalConfig["profiled_id"], globalConfig["course_name"]);
    Serial.println("获取lessonId: " + lessonId);
    delay(200);
    if (lessonId == "")
    {
        Serial.println("没有找到相关课程");
        ui.addNotice("没有找到相关课程");
        CUIT_STATUS = CUIT_NO_ACTION;
        return;
    }

    // 抢课
    Serial.println("抢课");
    ui.addNotice("开始抢课: " + (String)globalConfig["course_name"]);
    while (true)
    {
        boolean result = oaa.fuckCourse(globalConfig["profiled_id"], lessonId);
        Serial.println("fuckCourse: " + String(result));
        if (result){
            ui.addNotice("选课成功 ^_^");
            CUIT_STATUS = CUIT_NO_ACTION;
            break;
        }
        if(CUIT_STATUS != CUIT_OAA_FUCK){
            ui.addNotice("异常终止 :(");
            return;
        }
        delay(200);
    }
}
void CUIT_Init(boolean onlyData){
    if(!onlyData){
        cuitStatusCB[CUIT_SSO_LOGIN] = CUIT_SSO_LOGIN_F;
        cuitStatusCB[CUIT_OAA_LOGIN] = CUIT_OAA_LOGIN_F;
        cuitStatusCB[CUIT_OAA_CAPTCHA] = CUIT_OAA_CAPTCHA_F;
        cuitStatusCB[CUIT_OAA_FUCK] = CUIT_OAA_FUCK_F;
    }
}

void CUIT_Monitor(){
    if(cuitStatusCB[CUIT_STATUS] != nullptr)cuitStatusCB[CUIT_STATUS]();
}