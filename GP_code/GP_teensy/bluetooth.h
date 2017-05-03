
// BLUETOOTH MODULE COMMUNICATION
//***********************************************************************
#define BTSERIAL Serial3
#define MESSAGE_BEGIN 29
#define MESSAGE_END 30
#define MAX_MESSAGE_SIZE 100

char messageBuffer[MAX_MESSAGE_SIZE]; // bluetooth message content
int bufferPos = 0; //keeps track of last char in messageBuffer
unsigned long int btPeriod = 100; // ms
unsigned long int prevBtTime = 0;

//Check if complete bluetooth packet was received
bool packetAvailable();

//Parse bt packet into name and value
void packetParse();

// Executes bluetooth command 
void exec_packet_cmd(char *cmd_name, char* cmd_value);

// Sends telemetry data through bluetooth back to computer monitoring
void telemetry();
