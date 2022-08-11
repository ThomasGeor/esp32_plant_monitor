/* Timer Driver for ESP32 burglar alarm
 *
 *  author : Thomas Georgiadis
 *
 */

#include "timer_driver.h"

const char *timer_tag = "TMR";
static uint8_t mq135_initialized;
xQueueHandle s_timer_queue;

typedef struct
{
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
} custom_timer_info_t;

/**
 * @brief A sample structure to pass events from the timer ISR to task
 *
 */
typedef struct
{
    custom_timer_info_t info;
    uint64_t timer_counter_value;
} custom_timer_event_t;

static bool IRAM_ATTR timer_group_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    custom_timer_info_t *info = (custom_timer_info_t *)args;

    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(info->timer_group, info->timer_idx);

    /* Prepare basic event data that will be then sent back to task */
    custom_timer_event_t evt = {
        .info.timer_group = info->timer_group,
        .info.timer_idx = info->timer_idx,
        .info.auto_reload = info->auto_reload,
        .info.alarm_interval = info->alarm_interval,
        .timer_counter_value = timer_counter_value};

    if (!info->auto_reload)
    {
        timer_counter_value += info->alarm_interval * TIMER_SCALE;
        timer_group_set_alarm_value_in_isr(info->timer_group, info->timer_idx, timer_counter_value);
    }

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(s_timer_queue, &evt, &high_task_awoken);

    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

/**
 * @brief Initialize selected timer of timer group
 *
 * @param group Timer Group number, index from 0
 * @param timer timer ID, index from 0
 * @param auto_reload whether auto-reload on alarm event
 * @param timer_interval_sec interval of alarm
 */
static void custom_timer_init(int group, int timer, bool auto_reload, int timer_interval_sec)
{
    // Set the LOGS that you want to see.
    esp_log_level_set(timer_tag, ESP_LOG_VERBOSE);

    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = auto_reload,
    }; // default clock source is APB
    timer_init(group, timer, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(group, timer, 0);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(group, timer, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(group, timer);

    custom_timer_info_t *timer_info = calloc(1, sizeof(custom_timer_info_t));
    timer_info->timer_group = group;
    timer_info->timer_idx = timer;
    timer_info->auto_reload = auto_reload;
    timer_info->alarm_interval = timer_interval_sec;
    timer_isr_callback_add(group, timer, timer_group_isr_callback, timer_info, 0);

    timer_start(group, timer);
    ESP_LOGV(timer_tag, "Timer started.");
}

void timer_int_task(void *arg)
{
    mq135_initialized = 0;
    // Timer initialization
    s_timer_queue = xQueueCreate(10, sizeof(custom_timer_event_t));
    // should be called in door state change
    custom_timer_init(TIMER_GROUP_0, TIMER_0, true, MQ135_STARTUP_BURN_IN_PERIOD);
    custom_timer_event_t evt;
    for (;;)
    {
        if (xQueueReceive(s_timer_queue, &evt, portMAX_DELAY))
        {
            timer_pause(TIMER_GROUP_0, TIMER_0);
            ESP_LOGI(timer_tag, "MQ135 is ready.");
            mq135_initialized = 1;
            vTaskDelete(NULL);
        }
    }
}

uint8_t is_mq135_ready(void)
{
    return mq135_initialized;
}