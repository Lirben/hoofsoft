#ifndef HOOFSERIAL_H
#define HOOFSERIAL_H

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
    void println(String payload);

  private:
    Stream* _serial;
    String _serialInput;
    CommandPacket decodeJson(String jsonString);    
};

#endif
