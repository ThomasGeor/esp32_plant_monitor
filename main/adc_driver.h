#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#define NUM_OF_SAMPLES 64

void adc_init(const adc_unit_t unit,
              const adc_atten_t atten,
              const adc_channel_t channel,
              esp_adc_cal_characteristics_t *adc_chars);

uint32_t get_adc_reading(const adc_channel_t channel,
                         esp_adc_cal_characteristics_t *adc_chars);