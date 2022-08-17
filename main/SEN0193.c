#include <SEN0193.h>

static adc_unit_t unit = ADC_UNIT_1;
static adc_atten_t atten = ADC_ATTEN_DB_11;
static adc_channel_t channel = ADC1_CHANNEL_7; // GPIO_NUM_35
static esp_adc_cal_characteristics_t adc_chars;

void sen0193_init(void)
{
  adc_init(unit, atten, channel, &adc_chars);
}

uint8_t get_soil_humidity(void)
{
  uint32_t Vout = (get_adc_reading(channel, &adc_chars));
  float soilHumidity = ((float)Vout - SEN0193_V_WATER) / SEN0193_V_RANGE;
  soilHumidity *= -1;
  soilHumidity += 1;
  uint8_t result = (uint8_t)(soilHumidity * 100);
  return result;
}