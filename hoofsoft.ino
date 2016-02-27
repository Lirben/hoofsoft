#include <ADC.h>
#include <ArduinoJson.h>

#include "HoofSerial.h"
#include "Parameter.h"
#include "hoofsoft.h"


//Declare variables
HoofSerial xbee;
const char* hoofLocation;
ADC *adc = new ADC();
DataPacket dataPacket;

//Parameter variables
volatile bool ledHigh;
volatile bool transmitRaw;
volatile bool dataAvailable;

//Declare timers
IntervalTimer ledTimer;
IntervalTimer analogTimer;

/**********************************************************************/
void initialise()
{
  hoofLocation = FRONT_LEFT;
  transmitRaw = false;
  dataAvailable = false;
}


/**********************************************************************/
void setup_adc()
{ 
  pinMode(sensorPin0, INPUT);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);  

  analogTimer.priority(0);    //Highest priority!
  analogTimer.begin(readAnalog, 50000);
}


/**********************************************************************/
void setup_serial()
{  
  Serial.begin(9600);
  Serial1.begin(57600);
  xbee.setSerial(Serial1);  
}


/**********************************************************************/
void setup()
{  
  delay(500);         //Startup delay
  
  initialise();      //Initialise parameters & variables  
  setup_serial();     //Initialise the serial connections
  setup_adc();        //Initialise the analogue pins & timers

  //Initialise the blinking led
  ledTimer.priority(128);
  ledTimer.begin(ledBlink, 500000);

}


/**********************************************************************/
void loop()
{
  
  if (xbee.dataAvailable())                           //Get command from the serial connection if available
  {    
    String jsonString = xbee.readln();

    Serial.println(jsonString);
       
    CommandPacket cmdPacket = decodeJson(jsonString);
    
    if((cmdPacket.type) == 0)
    {
      if(cmdPacket.command == "read")
      {
        readParameter(cmdPacket.parameter);
      }
        
      if(cmdPacket.command == "update")
      {
        updateParameter(cmdPacket.parameter, cmdPacket.value);
      }      
    }
  }
  
  /*if(dataAvailable)                               //Send new data over Xbee if available
  {
    xbee.println(encodeJson(dataPacket));
    dataAvailable = false;
  }*/

}

/**********************************************************************/
void readAnalog()
{ 
  /* 
  //if(transmitRaw)
  {
    dataPacket = { 2, hoofLocation, millis(), {0, 0, 0, 0} };
    
    noInterrupts();  
    dataPacket.data[0] = adc->analogRead(sensorPin0);
    dataPacket.data[1] = adc->analogRead(sensorPin1);
    dataPacket.data[2] = adc->analogRead(sensorPin2);
    dataPacket.data[3] = adc->analogRead(sensorPin3);
    interrupts();

    dataAvailable = true;  
  }*/
}


/**********************************************************************/
void ledBlink()
{  
  if (ledHigh)
  {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }  
  ledHigh = !ledHigh;
}


/**********************************************************************/
void readParameter(String key)
{  
  String response = "NaN";

  if(key == "location")
  {
    response = hoofLocation;
  }
  
  if(key == "transmitRaw")
  {
    response = (String) transmitRaw;
  }

  //sendResponse(key, response);
}


/**********************************************************************/
void updateParameter(String key, String value)
{
  String response = "NaN";
  
  //Create Parameter
  Parameter param(key, value);
    
  if(param.readKey() == "transmitRaw")
  {
    transmitRaw = param.readBlValue();
    response = value;
    
    Serial.print("Parameter: ");
    Serial.print(key);
    Serial.print(" changed to ");
    Serial.println(transmitRaw);
  }

  sendResponse(key, response);
}


/**********************************************************************/
void sendResponse(String key, String response)
{
  ResponsePacket pcktResponse = { 1, "FL", key, response };
  String strResponse = encodeJson(pcktResponse);
  xbee.println(strResponse);
}


/**********************************************************************/
CommandPacket decodeJson(String jsonString)
{ 
  char jsonChar[200];
  StaticJsonBuffer<200> jsonBuffer;
  
  jsonString.toCharArray(jsonChar, jsonString.length() + 1);
  JsonObject& root = jsonBuffer.parseObject(jsonChar);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return (CommandPacket) { 0, "null", "null" };
  }

  return (CommandPacket) { root["type"], root["command"], root["parameter"], root["value"] };
}


/**********************************************************************/
String encodeJson(DataPacket dataPacket)
{
  StaticJsonBuffer<200> jsonBuffer;  
  
  //Build object tree in memory
  JsonObject& root = jsonBuffer.createObject();
    
  root["type"] = dataPacket.type;
  root["hoof"] = dataPacket.hoof.c_str();
  root["time"] = dataPacket.timeStamp;

  JsonArray& data = root.createNestedArray("data");
      
  for(unsigned int i = 0; i < (sizeof(dataPacket.data) / sizeof(int)); i++)
  {
    data.add(dataPacket.data[i]);    
  }
    
  //Generate the JSON string
  char buffer[256];
  root.printTo(buffer, sizeof(buffer));
   
  return String(buffer);
}


/**********************************************************************/
String encodeJson(const ResponsePacket& data)
{
  StaticJsonBuffer<JSON_OBJECT_SIZE(4)> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  root["type"] = data.type;
  root["hoof"] = data.hoof.c_str();
  root["parameter"] = data.parameter.c_str();
  root["value"] = data.value.c_str();
        
  //Generate the JSON string
  char buffer[256];
  root.printTo(buffer, sizeof(buffer));
   
  return String(buffer);  
}
