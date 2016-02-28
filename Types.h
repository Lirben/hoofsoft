#ifndef TYPES_H
#define TYPES_H

//Define hoofs
#define FRONT_LEFT  "FL"
#define FRONT_RIGHT "FR"
#define HIND_LEFT   "HL"
#define HIND_RIGHT  "HR"

//Define types of packages
static const int COMMAND_PACKAGE_TYPE = 0;
static const int ANSWER_PACKAGE_TYPE = 1;
static const int DATA_PACKAGE_TYPE = 2;

//Define Structs
struct DataPacket
{
  int type;
  String hoof;
  unsigned int timeStamp;
  int data[4];
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
  String hoof;
  String parameter;
  String value;
};

#endif
