#include <ADC.h>
#include <ArduinoJson.h>

#include "Types.h"
#include "Parameter.h"
#include "HoofSerial.h"

//Declare variables
HoofSerial xbee;
DataPacket dataPacket;
int hoofLocation;
ADC *adc = new ADC();

//Parameter variables
volatile bool ledHigh;
volatile bool transmitRaw;
volatile bool dataAvailable;
volatile unsigned int prevTime;
volatile unsigned int timeBase;
volatile int dataBufferId;

//Declare timers
IntervalTimer ledTimer;
IntervalTimer analogTimer;

//Define pins
#define led 13
#define sideSwitch 23
#define sensorPin0 A0
#define sensorPin1 A1
#define sensorPin2 A2
#define sensorPin3 A3


/**********************************************************************/
void initialise()
{
  transmitRaw = false;
  dataAvailable = false;
  dataBufferId = 0;
  timeBase = 0;
  prevTime = 0;
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
void detectLocation()
{
  pinMode(sideSwitch, INPUT);
  
  hoofLocation = FRONT_LEFT;

  if(digitalRead(sideSwitch) == true)
  {
      hoofLocation = FRONT_RIGHT;
  }
}

/**********************************************************************/
void setup()
{  
  delay(500);         //Startup delay
  
  initialise();       //Initialise parameters & variables
  detectLocation();   //Detect the location of the hoof
  setup_serial();     //Initialise the serial connections
  setup_adc();        //Initialise the analogue pins & timers

  //Initialise the blinking led
  ledTimer.priority(128);
  ledTimer.begin(ledBlink, 500000);
}


/**********************************************************************/
void loop()
{
  
  if (xbee.dataAvailable())
  {    
    CommandPacket cmdPacket = xbee.readPacket();

    if((cmdPacket.type) == COMMAND_PACKAGE_TYPE)
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
  
  if(dataAvailable)                               //Send new data over Xbee if available
  {
    xbee.sendData(dataPacket);
    dataAvailable = false;
  }
}


/**********************************************************************/
void readAnalog()
{
  ///Calculate time base
  unsigned int curTime = millis();
  
  noInterrupts();
  timeBase += (curTime - prevTime);
  prevTime = curTime;
  interrupts();

  Serial.println(timeBase);

  ///Measure & transmit if applicable
  if(transmitRaw)
  {    
    DataContent dataContent = { timeBase, {0, 0, 0, 0} };
    
    if(dataBufferId == 0)
    {
      dataPacket = { DATA_PACKAGE_TYPE, {dataContent, dataContent, dataContent, dataContent, dataContent} };
    }
    
    noInterrupts();
    dataContent.data[0] = adc->analogRead(sensorPin0);
    dataContent.data[1] = adc->analogRead(sensorPin1);
    dataContent.data[2] = adc->analogRead(sensorPin2);
    dataContent.data[3] = adc->analogRead(sensorPin3);
    interrupts();

    dataPacket.data[dataBufferId] = dataContent;

    if(dataBufferId == DATA_ARRAY_SIZE - 1)
    {
      dataAvailable = true;
      dataBufferId = -1;
    }

    dataBufferId++;
  }
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
    response = (String) hoofLocation;
  }
  else if(key == "transmitRaw")
  {
    response = (String) transmitRaw;
  }
  else if(key == "timeBase")
  {    
    response = (String) timeBase;
  }

  sendResponse(key, response);
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
  }
  else if(param.readKey() == "timeBase")
  {
    noInterrupts();
    timeBase = (unsigned int) value.toInt(); 
    interrupts();
    
    response = String(timeBase);
  }
      
  Serial.print("Parameter: ");
  Serial.print(key);
  Serial.print(" changed to ");
  Serial.println(transmitRaw);  

  sendResponse(key, response);
}


/**********************************************************************/
void sendResponse(String key, String response)
{
  ResponsePacket pcktResponse = { ANSWER_PACKAGE_TYPE, key, response };
  xbee.sendResponse(pcktResponse);
}
