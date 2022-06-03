#ifndef __UI_H__
#define __UI_H__
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

class UI
{
private:
    /* data */
public:
    UI(/* args */);
    ~UI();
    void init();
    void setNum(String num);
    void setNotice(String notice);
    void setIP(String ip);
    void test();
};
#endif