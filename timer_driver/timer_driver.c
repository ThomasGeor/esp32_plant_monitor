/* Brief: One shot timer driver implementation.
 * author: Thomas Georgiadis
 */
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/gptimer.h"

const char *timer_tag = "TMR";

static bool single_use_timer_isr_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    uint8_t* status = (uint8_t*) user_data;
    uint64_t timer_counter_value = 0;
    ESP_ERROR_CHECK(gptimer_get_raw_count(timer, &timer_counter_value));
    gptimer_stop(timer);
    gptimer_disable(timer);
    gptimer_del_timer(timer);
    ESP_LOGI(timer_tag, "Timer value %llu.", timer_counter_value);
    ESP_LOGI(timer_tag, "Status  %d.", *status);
    *status  = 1;
    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

/**
 * @brief Initialize selected timer of timer group
 *
 * @param timer_interval_sec alarm interval
 * @param *share_status user input/output variable indicating alarm event to the caller
 */
void one_shot_timer_init(int timer_interval_sec, uint8_t *shared_status)
{
   esp_log_level_set(timer_tag, ESP_LOG_ERROR);

   gptimer_handle_t timer = NULL;
   gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000,
   };

   ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer));
   gptimer_event_callbacks_t cb = {.on_alarm = single_use_timer_isr_cb,};
   ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer, &cb, shared_status));
   ESP_ERROR_CHECK(gptimer_enable(timer));

   gptimer_alarm_config_t alarm_cfg = {.alarm_count = (timer_interval_sec*timer_config.resolution_hz),};
   ESP_ERROR_CHECK(gptimer_set_alarm_action(timer, &alarm_cfg));
   ESP_ERROR_CHECK(gptimer_start(timer));
   ESP_LOGI(timer_tag, "Timer started.");
}
