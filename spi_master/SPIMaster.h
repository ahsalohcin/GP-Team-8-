#define SPI_CLK 4000

const int slaveSelect = 10;
const byte FLOAT_SIZE = 4;
const char READ_CMD = 'r';
const char WRITE_CMD = 'w';

union Data{
  float data_float;
  byte data_byte[sizeof(float)];
};

void sendByte(byte, byte);
float getFlt(void);
byte getByte(byte);
byte sendReceive(byte data);
void SPIMasterInit();
boolean SPIReadError();
