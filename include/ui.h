#ifndef __UI_H__
#define __UI_H__
#include <U8g2lib.h>
#include <deque>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/**
 * 屏幕处理类
 * 
 * */
class UI
{
private:
    /* data */
    std::deque<String> log;
public:
    UI(/* args */);
    ~UI();
    // 初始化
    void init();
    /**
     * 设置学号
     * 
     * @param num 学号
     * 
     * */
    void setNum(String num);
    /**
     * 添加显示文字（下半部分）
     * 
     * @param notice 显示文字内容
     * 
     * */
    void addNotice(String notice);
    /**
     * 设置显示IP
     * 
     * @param ip IP
     * 
     * */
    void setIP(String ip);
    // 测试
    void test();
};

extern UI ui;

#endif