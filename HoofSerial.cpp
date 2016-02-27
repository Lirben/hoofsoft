#ifndef HoofSerial_h
#define HoofSerial_h

#include "Arduino.h"
#include "HoofSerial.h"
#include "HardwareSerial.h"


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
void HoofSerial::println(String payload)
{
  _serial->println(payload);
}

#endif

