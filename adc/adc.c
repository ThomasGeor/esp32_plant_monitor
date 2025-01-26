/*
 * Brief: ADC1 driver.
 */
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

static const char *ADC_TAG = "ADC";
// ADC UNIT 1 configuration (ADC2 not available on the board)
static const adc_oneshot_unit_init_cfg_t adc1_init_cfg = {.unit_id = ADC_UNIT_1};
static adc_oneshot_unit_handle_t adc1_handle;

void adc_init(void)
{
  esp_log_level_set(ADC_TAG, ESP_LOG_ERROR);
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc1_init_cfg, &adc1_handle));
}

void adc_ch_init(adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t* adc1_ch_cali_handle)
{
  // Channel configuration
  adc_oneshot_chan_cfg_t chn_cfg =
  {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = atten,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, channel, &chn_cfg));

  // Callibration configuration
  adc_cali_line_fitting_config_t ch_cal_cfg =
  {
    .unit_id = ADC_UNIT_1,
    .atten = atten,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&ch_cal_cfg, adc1_ch_cali_handle));
}

int get_adc_voltage(adc_channel_t channel, adc_cali_handle_t adc1_ch_cali_handle)
{
    // Read the analog input
    int adc_out_raw = 0;
    int adc_out_mv = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, channel, &adc_out_raw));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_ch_cali_handle, adc_out_raw, &adc_out_mv));
    ESP_LOGI(ADC_TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, channel, adc_out_mv);
    return adc_out_mv;
}
