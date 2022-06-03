
#include "storage.h"
DynamicJsonDocument doc(1024);
JsonObject globalConfig;
/**
 * 读取配置
 * 
 * */
boolean Storage_ReadConfig(){
    
  // 文件测试
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return false;
  }
  
  File file = LittleFS.open("/config.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return false;
  }
  
  Serial.println("File Content:");
  String config = "";
  while(file.available()){
    // Serial.write(file.read());
    config += file.readString();
  }
  Serial.println(config);
  file.close();

  // You can use a String as your JSON input.
  // WARNING: the string in the input  will be duplicated in the JsonDocument.
  deserializeJson(doc, config);
  globalConfig = doc.as<JsonObject>();
  return true;
}

/**
 * 保存配置
 * 
 * */
void Storage_SaveConfig(){

    String configStr;
    serializeJson(globalConfig, configStr);
    

    // 文件测试
    if(!LittleFS.begin()){
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    
    File file = LittleFS.open("/config.json", "w");
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    
    Serial.println("File Content:");
    file.print(configStr);
    file.close();

}