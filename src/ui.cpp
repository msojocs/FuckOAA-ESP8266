#include "ui.h"

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
UI ui;

UI::UI(/* args */)
{
}

UI::~UI()
{
}
void UI::init(){
    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
}
void UI::setNum(String num){
    char str[num.length() + 1];
    num.toCharArray(str, num.length()+1);
    u8g2.drawStr(0, 10, "num: ");	// write something to the internal memory
    u8g2.drawStr(30, 10, str);	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
}
void UI::setNotice(String notice){
    char str[notice.length() + 1];
    notice.toCharArray(str, notice.length()+1);
    u8g2.drawStr(0, 10, str);	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
}
void UI::setIP(String ip){
    char str[ip.length() + 1];
    ip.toCharArray(str, ip.length()+1);
    u8g2.drawStr(0, 20, str);	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
}
void UI::test(){
    Serial.println("UI test");
    u8g2.clearBuffer();					// clear the internal memory
    u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
}