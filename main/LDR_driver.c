#include <LDR_driver.h>

static adc_unit_t unit = ADC_UNIT_1;
static adc_atten_t atten = ADC_ATTEN_DB_11;
static adc_channel_t channel = ADC1_CHANNEL_5;
static esp_adc_cal_characteristics_t adc_chars;

void ldr_init(void)
{
  adc_init(unit, atten, channel, &adc_chars);
}

int get_light_intensity(void)
{

  uint32_t voltage = get_adc_reading(channel, &adc_chars);
  voltage += 200; // calibration voltage
  // ESP_LOGI(LDR_TAG, "ADC Voltage: %d", voltage);

  /*
    Minimum voltage means maximum luminosity intensity while
    maximum voltage means maximum intensity
  */
  float light_intensity = ((float)voltage - MIN_V_VAL) / LI_RANGE;
  // ESP_LOGI(LDR_TAG, "LI: %f", light_intensity);

  light_intensity *= -1;
  light_intensity += 1;
  // uint32_t R_LDR = (voltage * R_S_LDR) / (LDR_VIN - voltage);
  // R_LDR += R_S_LDR;
  // uint32_t luminosity = LDR_CONST * pow(R_LDR, LDR_R_POW_FACTOR);

  int result = (int)(light_intensity * 100);

  return result;
}