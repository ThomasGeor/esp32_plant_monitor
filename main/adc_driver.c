#include "adc_driver.h"

// static const char *LDR_TAG = "ADC";

void adc_init(adc_unit_t unit,
              adc_atten_t atten,
              adc_channel_t channel,
              esp_adc_cal_characteristics_t *adc_chars)
{
  esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_DEFAULT, 0, adc_chars);
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
  ESP_ERROR_CHECK(adc1_config_channel_atten(channel, atten));
}

uint32_t get_adc_reading(adc_channel_t channel,
                         esp_adc_cal_characteristics_t *adc_chars)
{
  // Oversampling for more accurate results.
  uint32_t adc_reading = 0;
  for (uint8_t i = 0; i < NUM_OF_SAMPLES; i++)
  {
    adc_reading += adc1_get_raw(channel);
  }
  adc_reading /= NUM_OF_SAMPLES;

  uint32_t voltage;
  voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
  return voltage;
}