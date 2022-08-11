/*
  DHT22 temperature sensor driver
*/
#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

#define DHTDataWords 5 // to complete 40 = 5*8 Bits
#define DHT_LEVEL_LOW 0
#define DHT_LEVEL_HIGH 1

void errorHandler(int response);
void setDhtPin(int gpio);
int readDHT(float *humidity, float *temperature);
