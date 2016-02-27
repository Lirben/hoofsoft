//Constants
#define FRONT_LEFT  "FL"
#define FRONT_RIGHT "FR"
#define HIND_LEFT   "HL"
#define HIND_RIGHT  "HR"

//Define pins
#define led 13
#define sensorPin0 A0
#define sensorPin1 A1
#define sensorPin2 A2
#define sensorPin3 A3

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

