#include "Arduino.h"
#include "HoofSerial.h"
#include "HardwareSerial.h"

#include <ArduinoJson.h>


/**********************************************************************/
void HoofSerial::setSerial(Stream &serial)
{
  _serial = &serial;
  _serialInput.reserve(200);
}

/**********************************************************************/
bool HoofSerial::dataAvailable()
{
  return _serial->available();
}

/**********************************************************************/
String HoofSerial::readln()
{
  bool stringComplete = false;
  
  _serialInput = "";
  
  while (!stringComplete) {
    
    //Wait for the next byte to arrive
    while(!_serial->available())
    { }

    char inChar = (char) _serial->read();                // Get the new byte
    
    if (inChar != '\r' && inChar != '\n')
    {      
      _serialInput += inChar;
    }
    
    if (inChar == '\n')
    {
      stringComplete = true;
    } 
  }

  return _serialInput;
}

/**********************************************************************/
CommandPacket HoofSerial::readPacket()
{
  CommandPacket cmdPacket = { COMMAND_PACKAGE_TYPE, "null", "null", "null"};
  
  if(this->dataAvailable())
  { 
    String jsonString = this->readln();
    cmdPacket = decodeJson(jsonString);    
  }

  Serial.print(cmdPacket.type);
  Serial.print(" ");
  Serial.print(cmdPacket.command);
  Serial.print(" ");
  Serial.print(cmdPacket.parameter);
  Serial.print(" ");
  Serial.println(cmdPacket.value);

  return cmdPacket;  
}

/**********************************************************************/
void HoofSerial::sendData(const DataPacket& dataPacket)
{
  String output;
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(4);
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();       //Build object tree in memory
    
  root["type"] = dataPacket.type;
  root["hoof"] = dataPacket.hoof.c_str();
  root["time"] = dataPacket.timeStamp;

  JsonArray& data = root.createNestedArray("data");
      
  for(unsigned int i = 0; i < (sizeof(dataPacket.data) / sizeof(int)); i++)
  {
    data.add(dataPacket.data[i]);    
  }
  
  root.printTo(output);       //Generate the JSON string  
  this->println(output);      //Send over the Serial medium 
}

/**********************************************************************/
void HoofSerial::sendResponse(const ResponsePacket& responsePacket)
{
  String output;
  StaticJsonBuffer<JSON_OBJECT_SIZE(4)> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();       //Build object tree in memory
  
  root["type"] = responsePacket.type;
  root["hoof"] = responsePacket.hoof.c_str();
  root["parameter"] = responsePacket.parameter.c_str();
  root["value"] = responsePacket.value.c_str();
  
  root.printTo(output);       //Generate the JSON string  
  this->println(output);      //Send over the Serial medium 
}

/**********************************************************************/
void HoofSerial::println(String payload)
{
  _serial->println(payload);
}

/**********************************************************************/
CommandPacket HoofSerial::decodeJson(String jsonString)
{  
  char jsonChar[jsonString.length() + 1];    
  jsonString.toCharArray(jsonChar, sizeof(jsonChar));         //Convert to char array

  StaticJsonBuffer<200> jsonBuffer; 
  JsonObject& root = jsonBuffer.parseObject(jsonChar); 

  if (root.success())
  {
    return (CommandPacket) { root["type"], root["command"], root["parameter"], root["value"] };
  }

  return (CommandPacket) { 0, "null", "null", "null" };
}
