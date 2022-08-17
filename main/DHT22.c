#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "DHT22.h"

static const char *DHT_TAG = "DHT";

int dhtPin = GPIO_NUM_27; // my default DHT pin = 4

void setDhtPin(int gpio)
{
  dhtPin = gpio;
}

void errorHandler(int response)
{
  switch (response)
  {
  case DHT_TIMEOUT_ERROR:
    ESP_LOGE(DHT_TAG, "Sensor Timeout\n");
    break;
  case DHT_CHECKSUM_ERROR:
    ESP_LOGE(DHT_TAG, "CheckSum error\n");
    break;
  case DHT_OK:
    break;
  default:
    ESP_LOGE(DHT_TAG, "Unknown error\n");
  }
}

/*-------------------------------------------------------------------------------
;
;	get next state
;
;	I don't like this logic. It needs some interrupt blocking / priority
;	to ensure it runs in realtime.
;
;--------------------------------------------------------------------------------*/

static int getSignalLevel(int usTimeOut, bool state)
{

  int uSec = 0;
  while (gpio_get_level(dhtPin) == state)
  {
    if (uSec > usTimeOut)
    {
      return -1;
    }
    else
    {
      ++uSec;
      ets_delay_us(1); // uSec delay
    }
  }
  return uSec;
}

int readDHT(float *humidity, float *temperature)
{
  int uSec = 0;
  uint8_t dhtData[DHTDataWords];
  uint8_t byteIdx = 0;
  uint8_t bitIdx = 7;

  // Clear the data buffer
  for (int k = 0; k < DHTDataWords; k++)
    dhtData[k] = 0;

  // Send start signal to DHT sensor
  gpio_set_direction(dhtPin, GPIO_MODE_OUTPUT);

  // pull down for 3 ms for a smooth and nice wake up
  gpio_set_level(dhtPin, DHT_LEVEL_LOW);
  ets_delay_us(3000);

  // pull up for 25 us for a gentile asking for data
  gpio_set_level(dhtPin, DHT_LEVEL_HIGH);
  ets_delay_us(25);

  // change dht pin direction to data receiver
  gpio_set_direction(dhtPin, GPIO_MODE_INPUT);

  // DHT will keep the line low for 80 us and then high for 80us
  uSec = getSignalLevel(80, 0);
  //	ESP_LOGI( DHT_TAG, "Response = %d", uSec );

  if (uSec < 0)
    return DHT_TIMEOUT_ERROR;

  // -- 80us up --
  uSec = getSignalLevel(80, 1);
  //	ESP_LOGI( DHT_TAG, "Response = %d", uSec );
  if (uSec < 0)
    return DHT_TIMEOUT_ERROR;

  // No errors from the sensor -> read the 40 data bits
  for (uint8_t k = 0; k < 40; k++)
  {
    // Start new data transmission with >50us low signal
    uSec = getSignalLevel(52, 0);
    if (uSec < 0)
      return DHT_TIMEOUT_ERROR;

    // Check if after 70us rx data is a 0 or a 1
    uSec = getSignalLevel(70, 1);
    if (uSec < 0)
      return DHT_TIMEOUT_ERROR;

    /*
        Add the current read to the output data
        since all dhtData array was set to 0 at the start,
        only look for "1" (>28us us)
    */
    if (uSec > 40)
    {
      dhtData[byteIdx] |= (1 << bitIdx);
    }

    // index to next byte
    if (bitIdx == 0)
    {
      bitIdx = 7;
      ++byteIdx;
    }
    else
    {
      bitIdx--;
    }
  }

  // Get humidity from Data[0] and Data[1] ==========================
  *humidity = dhtData[0];
  *humidity *= 0x100; // >> 8
  *humidity += dhtData[1];
  *humidity /= 10; // get the decimal

  // Get temp from Data[2] and Data[3]
  *temperature = dhtData[2] & 0x7F;
  *temperature *= 0x100; // >> 8
  *temperature += dhtData[3];
  *temperature /= 10;

  if (dhtData[2] & 0x80) // negative temp, brrr it's freezing
    *temperature *= -1;

  // Verify if checksum is ok
  // Checksum is the sum of Data 8 bits masked out 0xFF
  if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF))
  {
    return DHT_OK;
  }
  else
  {
    return DHT_CHECKSUM_ERROR;
  }
}
