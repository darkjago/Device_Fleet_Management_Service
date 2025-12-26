# Device_Fleet_Management_Service

**How to setup:

*To install gRPC on a new enviroment follow the oficial documentation for c++:
<https://grpc.io/docs/languages/cpp/quickstart/>

*C++ Compilation:
mkdir build && cd build
cmake ..
make

*Python environment setup:
Run python_set_env.sh, this script create the virtual environmet, install lib requirements listed on requirements.txt and will compilate the proto file for python

To execute the server run fleet_management binary on a terminal

The client runs in a different terminal with the client.py script

service.proto:

syntax = "proto3";

package fleet;

enum Status {  IDLE = 0;  BUSY = 1;  OFFLINE = 2;  MAINTENANCE = 3;  UPDATING = 4;}
enum ActionState {  PENDING = 0;  RUNNING = 1;  COMPLETED = 2;  FAILED = 3;}

message RegisterRequest
{
  string device_id = 1;
  Status initial_status = 2;
}

message StatusRequest
{
  string device_id = 1;
  Status status = 2;
}

message InfoRequest
{
  string device_id = 1;
}

message ActionRequest
{
  string device_id = 1;
  string action_type = 2;
  string version = 3;
}

message ActionResponse
{
  string action_id = 1;
}

message DeviceResponse
{
  string device_id = 1;
  Status status = 2;
  string message = 3;
}

message ActionStatusRequest
{
  string action_id = 1;
}

message ActionStatusResponse
{
  ActionState state = 1;
}

service DeviceManager
{
  rpc RegisterDevice(RegisterRequest) returns (DeviceResponse);
  rpc SetDeviceStatus(StatusRequest) returns (DeviceResponse);
  rpc GetDeviceInfo(InfoRequest) returns (DeviceResponse);
  rpc InitiateDeviceAction(ActionRequest) returns (ActionResponse);
  rpc GetActionStatus(ActionStatusRequest) returns (ActionStatusResponse);
}

**Architecture:

The server bin set himself as a service and will listen messages on port 50051. When a message arrive it will execute the corresponding action and return a response. The devices and actions are stored on memory
The client will send messages to client asking for an action to be executed and will print the response of the server.
The device id is a string value

**How to use the CLI:
The cli uses argparse to handle the commands:

python3 client.py "command" [arg name 1] [arg_value 1] [arg name 2] [arg_value 1]

*Commands and arguments:
Register device [id is mandatory]:
    register --id "string device id" ex: rasp-01
Device info [id is mandatory]
    info --id "string device id" ex: rasp-01
Update device software version [id is mandatory, ver is optional with a default value of 0.0.1] returns action id
    update --id "string device id" ex: rasp-01 --ver "string software version" ex: "0.0.1"
Track Action [action_id is mandatory, use value return by update command]
    track --action_id "string action id" ex "act-1"

Examples:
    python3 client.py register --id android-01
    python3 client.py info --id android-01
    python3 client.py update --id android-01 --ver 1.0.0 . returns act-1
    python3 client.py track --act_id act-1

**Next steps:
-Remove hardcode and magic values used on ports, commands and the time to emulated the processing action.
-Store devices and action on non volative memory as a shared memory, json file or a database
-Change python script to run as a process until user close it, change args for inline commands
