/* Brief: Plant monitor system initialization.
 * author: Thomas Georgiadis
*/

#include "esp_log.h"
#include "adc.h"
#include "tc74.h"
#include "MQ135.h"
#include "dht22.h"
#include "ldr.h"
#include "sen0193.h"
#include "wifi.h"

static const char *tag = "Debug";

void init(void)
{
  adc_init();
  tc74_init();
  mq135_init();
  ldr_init();
  sen0193_init();
  wifi_init();
}

void app_main(void)
{
  esp_log_level_set(tag, ESP_LOG_ERROR);
  init();
}
