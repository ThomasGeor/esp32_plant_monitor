/* Timer Driver for ESP32 burglar alarm
 *
 *  author : Thomas Georgiadis
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "esp_log.h"

// Defintions
#define TIMER_DIVIDER (16)                           //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER) // convert counter value to seconds

#define MQ135_STARTUP_BURN_IN_PERIOD 20
#define MQ135_RLOAD 20

void timer_int_task(void *arg);
uint8_t is_mq135_ready(void);
