/*
 * Brief: SEN0193 driver. The sensor provides an analogue output.
 *        The soil humidity percentage is provided online based on the
 *        offline calculated range values (max -> in water), (min -> air).
 * author: Thomas Georgiadis
 * */
#include "adc.h"

#define SEN0193_V_AIR 2600 // conservative estimation (mV)
#define SEN0193_V_WATER 950 // conservative estimation (mV)
#define SEN0193_V_RANGE (SEN0193_V_AIR - SEN0193_V_WATER)

static adc_atten_t atten = ADC_ATTEN_DB_12;
static const adc_channel_t channel = ADC_CHANNEL_7; // GPIO_NUM_35
static adc_cali_handle_t adc1_ch7_cali_handle;

void sen0193_init(void)
{
  adc_ch_init(channel, atten,&adc1_ch7_cali_handle);
}

uint8_t get_soil_humidity(void)
{
  // +-50mV resolution 
  int Vout = (get_adc_voltage(channel, adc1_ch7_cali_handle));
  float soilHumidity = ((float)Vout - SEN0193_V_WATER) / SEN0193_V_RANGE;
  soilHumidity *= -1;
  soilHumidity += 1;
  uint8_t result = (uint8_t)(soilHumidity * 100);
  return result;
}
