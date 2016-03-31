#include "Arduino.h"
#include "HoofSerial.h"
#include "HardwareSerial.h"

#include <ArduinoJson.h>


/**********************************************************************/
void HoofSerial::setSerial(Stream &serial)
{
  _xbee.setSerial(serial);
}

/**********************************************************************/
bool HoofSerial::dataAvailable()
{
  _xbee.readPacket();
  return _xbee.getResponse().isAvailable();
}

/**********************************************************************/
CommandPacket HoofSerial::readPacket()
{
  String jsonString = "";
  Rx16Response rx16 = Rx16Response();
  CommandPacket cmdPacket = { COMMAND_PACKAGE_TYPE, "null", "null", "null"};

  if (_xbee.getResponse().getApiId() == RX_16_RESPONSE)
  {
    _xbee.getResponse().getRx16Response(rx16);

    for(int i = 0; i < rx16.getDataLength(); i++)
    {
      char test = rx16.getData(i);
      jsonString += test;
    }
    
    cmdPacket = decodeJson(jsonString);
  
    Serial.print(cmdPacket.type);
    Serial.print(" ");
    Serial.print(cmdPacket.command);
    Serial.print(" ");
    Serial.print(cmdPacket.parameter);
    Serial.print(" ");
    Serial.println(cmdPacket.value);
  }
  return cmdPacket;  
}

/**********************************************************************/
void HoofSerial::sendData(const DataPacket& dataPacket)
{
  String output;
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(1) + (JSON_ARRAY_SIZE(DATA_ARRAY_SIZE) * (JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(4)));
  
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();       //Build object tree in memory
  
  root["type"] = dataPacket.type;
  JsonArray& dataObj = root.createNestedArray("sample");

  for(unsigned int i = 0; i < DATA_ARRAY_SIZE; i++)
  {
    JsonObject& test = dataObj.createNestedObject();
    test["t"] = dataPacket.data[i].timeStamp; 
     
    JsonArray& obj = test.createNestedArray("f");   

    for(unsigned int j = 0; j < 4; j++)
    {
      obj.add(dataPacket.data[i].data[j]);
    }    
  }
  
  root.printTo(output);       //Generate the JSON string  
  
  this->println(output, true);      //Send over the Serial medium
}

/**********************************************************************/
void HoofSerial::sendResponse(const ResponsePacket& responsePacket)
{
  String output;
  StaticJsonBuffer<JSON_OBJECT_SIZE(4)> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();       //Build object tree in memory
  
  root["type"] = responsePacket.type;
  root["parameter"] = responsePacket.parameter.c_str();
  root["value"] = responsePacket.value.c_str();
  
  root.printTo(output);               //Generate the JSON string  
  this->println(output, false);      //Send over the Serial medium 
}

/**********************************************************************/
void HoofSerial::println(String jsonString, bool disableAck = false)
{ 
  char jsonChar[jsonString.length() + 1];  
  jsonString.toCharArray(jsonChar, sizeof(jsonChar));  

  //Serial.println(jsonChar);

  Tx16Request tx;
  
  if(!disableAck)
  {
    tx = Tx16Request(0x0005, (unsigned char*) jsonChar, sizeof(jsonChar));
  }
  else
  {
    tx = Tx16Request(0x0005, 0x0001, (unsigned char*) jsonChar, sizeof(jsonChar), 0x0000);
  }   

  _xbee.send(tx);
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
