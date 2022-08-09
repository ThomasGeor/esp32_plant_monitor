/* Example of using the TC74 library on ESP32 WROVER B

    author : Thomas Georgiadis

   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "http_server.h"

static const char *_tag = "Plant_Server";

void tc74_init(void)
{
  // setup the sensor
  ESP_ERROR_CHECK(i2c_master_init());
}

void app_main(void)
{
  // Set the LOGS that you want to see.
  esp_log_level_set(_tag, ESP_LOG_ERROR);

  tc74_init();
  http_server_config();
}
