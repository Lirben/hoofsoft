#include "Arduino.h"
#include "Parameter.h"

/**********************************************************************/
Parameter::Parameter(String key, String value)
{
  _key = key;
  _blValue = false;
  _strValue = value;
  _intValue = value.toInt();
  
  if(_strValue.toLowerCase() == "true")
  {
    _blValue = true;
  }
}

/**********************************************************************/
bool Parameter::readBlValue()
{
  return _blValue;
}

/**********************************************************************/
int Parameter::readIntValue() {
  return _intValue;
}

/**********************************************************************/
String Parameter::readStrValue() {
  return _strValue;
}

/**********************************************************************/
String Parameter::readKey() {
  return _key;
}
