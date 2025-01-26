/*
 * Brief: LDR module driver. Provides the output via an analog and digital pin.
 *        The environmental light intensity percentage is provided online based
 *        on the offline calculated range values (max -> darkness), (min -> strong
 *        light).
 * author: Thomas Georgiadis
 * TODO: Find the sensor's measurement range in lumens and produce such values.
 *       Use a lumens meter in max ranges and create a linear (?) formula on the
 *       value? Is the photoresistor's voltage output linear though? What is it's
 *       formula?
 *       TIP: Configure the module's potentiometer to the preferred range
 *       settings.
 */
#include "esp_log.h"
#include "adc.h"

#define R_S_LDR 10000
#define LDR_VIN 3300 // mV
#define LDR_R_POW_FACTOR -1.4059
#define LDR_CONST 12500000

#define CAL_V 200 // ADC mean error offline calculated (mV)
#define MIN_V_VAL 142 // Minimum voltage output from LDR (mV)
#define MAX_V_VAL 3139 + CAL_V // Maximum voltage output from LDR (mV)
#define LI_RANGE (MAX_V_VAL - MIN_V_VAL) // Expected online measurement range.

static const adc_atten_t atten = ADC_ATTEN_DB_12;
static const adc_channel_t channel = ADC_CHANNEL_5;
static adc_cali_handle_t adc1_ch5_cali_handle;

static const char *LDR_TAG = "LDR";

void ldr_init(void)
{
  esp_log_level_set(LDR_TAG, ESP_LOG_ERROR);
  adc_ch_init(channel, atten, &adc1_ch5_cali_handle);
}

int get_light_intensity(void)
{

  uint32_t voltage = get_adc_voltage(channel, adc1_ch5_cali_handle);
  voltage += CAL_V; // calibration voltage
  ESP_LOGI(LDR_TAG, "ADC Voltage: %lu", voltage);

  /* Voltage is inversly proportional to luminosity. */
  float light_intensity = ((float)voltage - MIN_V_VAL) / LI_RANGE;
  ESP_LOGI(LDR_TAG, "LI: %f", light_intensity);

  light_intensity *= -1;
  light_intensity += 1;
  // uint32_t R_LDR = (voltage * R_S_LDR) / (LDR_VIN - voltage);
  // R_LDR += R_S_LDR;
  // uint32_t luminosity = LDR_CONST * pow(R_LDR, LDR_R_POW_FACTOR);

  int result = (int)(light_intensity * 100);

  return result;
}
