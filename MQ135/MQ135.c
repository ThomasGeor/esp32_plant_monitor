/*
 * Brief: MQ135 driver. Provides CO2 ppm readings. Needs proper initialization
 *        and calibration.
 * author: Thomas Georgiadis
 * TODO: Create a variable in Flash memory to check if the sensor has
 *       already been burned for 48 hours or not.
 */
#include "esp_log.h"
#include "adc.h"
#include "timer_driver.h"
#include <math.h>

// Parameters to model temperature and humidity dependence
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018
#define CORE -.003333333
#define CORF -.001923077
#define CORG 1.130128205

// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

// Atmospheric CO2 level for calibration purposes
#define ATMOCO2 414.72

// Calibration resistance at atmospheric CO2 level
// #define CALIBRATION_MODE
#define RZERO 81.9
// The load scaling resistance value on the board
#define RLOAD 20

#define MQ135_STARTUP_BURN_IN_PERIOD 20 // seconds
// Initialiy requires 48 hours burn in
#define MQ135_CALIBRATION_BURN_IN_PERIOD 172800 // seconds
#define MQ135_ADC_DC_OFFSET 0.49

static const char *MQ135_TAG = "MQ135";
static uint8_t mq135_initialized = 0;

static const adc_channel_t channel = ADC_CHANNEL_6;
static const adc_atten_t atten = ADC_ATTEN_DB_12;
adc_cali_handle_t adc1_ch6_cali_handle;

/*
 * Brief: Initialize an ADC channel to read analog inputs from the sensor and
 *        block any readings until the sensor's initial burn in period has
 *        passed.
 */
void mq135_init(void)
{
  esp_log_level_set(MQ135_TAG, ESP_LOG_ERROR);
#ifdef CALIBRATION_MODE
  one_shot_timer_init(MQ135_CALIBRATION_BURN_IN_PERIOD, &mq135_initialized);
#else
  one_shot_timer_init(MQ135_STARTUP_BURN_IN_PERIOD, &mq135_initialized);
#endif
  adc_ch_init(channel, atten, &adc1_ch6_cali_handle);
}

#ifdef CALIBRATION_MODE
// Read R0 after calibration burn in
static float getRZero(float Rs)
{
  return (Rs * pow((ATMOCO2 / PARA), (1. / PARB)));
}
#endif

float get_co2_ppm_value(void)
{
  float co2_ppm = 0.0;
  if (mq135_initialized != 0)
  {
    // Read the input from MQ135 through it's anolog output pin
    int voltage = get_adc_voltage(channel, adc1_ch6_cali_handle);
    ESP_LOGI(MQ135_TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, channel, voltage);

    // Convert the input to CO2 ppm in the air
    float Vout = ((float)voltage) / 1000;
    Vout += MQ135_ADC_DC_OFFSET; // ADC manual error compensation/calibration offset mean value
    float Rs = Vout * RLOAD;
    ESP_LOGI(MQ135_TAG, "Rs : %f", Rs);
#ifdef CALIBRATION_MODE
    ESP_LOGI(MQ135_TAG, "Initial burn in finished. MQ135 ready for measurements");
    co2_ppm = getRZero(Rs);
#else
    co2_ppm = PARA * pow((Rs / RZERO), -PARB);
#endif
  }
  return co2_ppm;
}
