#include <MQ135.h>

static adc_unit_t unit = ADC_UNIT_1;
static adc_atten_t atten = ADC_ATTEN_DB_11;
static adc_channel_t channel = ADC1_CHANNEL_6;
static esp_adc_cal_characteristics_t adc_chars;

void mq135_init(void)
{
  adc_init(unit, atten, channel, &adc_chars);
  xTaskCreate(timer_int_task, "timer_int_task", 2048, NULL, 10, NULL);
}

// Read R0 after burning in
static float getRZero(float Rs)
{
  return (Rs * pow((ATMOCO2 / PARA), (1. / PARB)));
}

float get_co2_ppm_value(void)
{

  if (is_mq135_ready() != 0)
  {
    // also convert Vout from mV to V
    float Vout = ((float)get_adc_reading(channel, &adc_chars)) / 1000;
    Vout += 0.49; // calibration
    // Vout = 2.58;
    float Rs = Vout * RLOAD;
    float co2_ppm;
#ifdef CALIBRATION_MODE
    co2_ppm = getRZero(Rs);
#else
    // ESP_LOGI(MQ135_TAG, "Rs : %f", Rs);
    co2_ppm = PARA * pow((Rs / RZERO), -PARB);
#endif
    return co2_ppm;
  }
  return 0;
}
