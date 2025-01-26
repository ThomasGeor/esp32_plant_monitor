/* ADC driver public interface */
#include "esp_adc/adc_oneshot.h"
void adc_init(void);
void adc_ch_init(adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t* adc1_ch_cali_handle);
int get_adc_voltage(adc_channel_t channel, adc_cali_handle_t adc1_ch_cali_handle);
