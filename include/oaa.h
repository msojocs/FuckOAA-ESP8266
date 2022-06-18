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
    String captchaCode;
public:
    OAA(/* args */);
    ~OAA();
    /**
     * 使用统一中心的 TGC 登录教务处
     * 
     * @return 1登录成功|-1错误
     * */
    int loginByTGC(String tgc);
    /**
     * 获取教务处选课验证码
     * 
     * @return String 验证码数据
     * */
    String getCaptcha();
    /**
     * 检测教务处选课验证码是否有效
     * 
     * @param captchaCode 验证码
     * @param profiledId 轮次ID
     * 
     * @return boolean true有效|false无效
     * */
    boolean checkCaptcha(String captchaCode, String profiledId);
    /**
     * 将要选取的课程转换为课程id
     * 
     * @param profiledId 轮次id
     * @param name 课程名
     * 
     * @return String 课程id
     * */
    String courseName2Id(String profiledId, String name);
    /**
     * 检测是否可以选课
     * 
     * @param profiledId 轮次id
     * 
     * @return boolean true可以|false不可以
     * */
    boolean isAvailable(String profiledId);
    /**
     * 选课操作
     * 
     * @param profiledId 轮次id
     * @param lessonId 课程id
     * 
     * @return boolean true选到了|false没有选到
     * */
    boolean fuckCourse(String profiledId, String lessonId);
};

#endif
