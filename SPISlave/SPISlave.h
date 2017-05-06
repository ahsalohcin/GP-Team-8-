const byte WRITE_CMD = 'w';
const byte READ_CMD = 'r';
const byte FLOAT_SIZE = 4;

void init_SPI();
void pointToData(float * data_point, byte * settings);
void resetSPI();
