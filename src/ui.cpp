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
    u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
    // u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font

    u8g2.setFont(u8g2_font_wqy12_t_gb2312a);	// choose a suitable font
    u8g2.drawLine(0, 21, 128, 21);
}
void UI::setNum(String num){
    char str[num.length() + 1];
    num.toCharArray(str, num.length()+1);
    u8g2.drawStr(0, 10, "num: ");	// write something to the internal memory
    u8g2.drawStr(30, 10, str);	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
}
void UI::addNotice(String notice){
    log.push_back(notice);
    if(log.size() > 3)log.pop_front();

    std::deque<String>::iterator it;
    
    // 清空区域
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 22, 128, 64-22);
    
    u8g2.setDrawColor(1);
    for ( it = log.begin(); it != log.end(); it++)
    {
        u8g2.setCursor(0, 33 + 15 * (it - log.begin()));
        u8g2.print(*it);	// write something to the internal memory
    }
    
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
    u8g2.setDrawColor(1);
    u8g2.drawStr(0,20,"Hello World!0");	// write something to the internal memory
    delay(1000);
    u8g2.setDrawColor(0);
    u8g2.drawStr(0,30,"Hello World!1");	// write something to the internal memory
    delay(1000);
    u8g2.setDrawColor(2);
    
    u8g2.drawStr(0,40,"Hello World!2");	// write something to the internal memory
    delay(1000);
    
    u8g2.sendBuffer();					// transfer internal memory to the display
}