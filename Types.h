#ifndef TYPES_H
#define TYPES_H

//Define constants
static const int DATA_ARRAY_SIZE = 2;

//Define types of packages
static const int COMMAND_PACKAGE_TYPE = 0;
static const int ANSWER_PACKAGE_TYPE = 1;
static const int DATA_PACKAGE_TYPE = 2;

//Define hoofs
static const int FRONT_LEFT = 0;
static const int FRONT_RIGHT = 1;
static const int HIND_LEFT = 2;
static const int HIND_RIGHT = 3;

//Define Structs
struct DataContent
{
  unsigned int timeStamp;  
  int data[4];
};

struct DataPacket
{
  int type;
  DataContent data[5];
};

struct CommandPacket
{
  int type;
  String command;
  String parameter;
  String value;
};

struct ResponsePacket
{
  int type;
  String parameter;
  String value;
};

#endif
