#ifndef HOOFSERIAL_H
#define HOOFSERIAL_H

#include <XBee.h>
#include "Types.h"

class HoofSerial
{
  public:
    void setSerial(Stream &serial);
    bool dataAvailable();
    String readln();
    CommandPacket readPacket();
    void sendData(const DataPacket& dataPacket);
    void sendResponse(const ResponsePacket& responsePacket);
    void println(String payload, bool disableAck);

  private:
    XBee _xbee = XBee();   
    CommandPacket decodeJson(String jsonString);    
};

#endif
