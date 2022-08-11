#include <sntp.h>
#include "esp_sntp.h"
#include "http_server.h"

static const char *TAG = "SNTP";

static void obtain_time(void);
static void initialize_sntp(void);
static char timestamp_buf[64];

void time_sync_notification_cb(struct timeval *tv)
{
  ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void sntp(void)
{

  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  // Is time set? If not, tm_year will be (1970 - 1900).
  if (timeinfo.tm_year < (2016 - 1900))
  {
    ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
    obtain_time();
    // update 'now' variable with current time
    time(&now);
  }

  // Set timezone to China Standard Time
  setenv("TZ", "UTC-3", 1);
  tzset();
  localtime_r(&now, &timeinfo);
  strftime(timestamp_buf, sizeof(timestamp_buf), "%c", &timeinfo);
  // ESP_LOGI(TAG, "The current date/time in Athens is:%s", timestamp_buf);
}

static void obtain_time(void)
{

  initialize_sntp();

  // wait for time to be set
  time_t now = 0;
  struct tm timeinfo = {0};
  int retry = 0;
  const int retry_count = SNTP_MAX_RETRY_COUNT;
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
  {
    // ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  time(&now);
  localtime_r(&now, &timeinfo);
}

static void initialize_sntp(void)
{
  ESP_LOGI(TAG, "Initializing SNTP");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, SNTP_TIME_SERVER);
  sntp_setservername(1, SNTP_TIME_SERVER); // set the secondary NTP server (will be used only if SNTP_MAX_SERVERS > 1)
  sntp_set_time_sync_notification_cb(time_sync_notification_cb);
  sntp_init();

  ESP_LOGI(TAG, "List of configured NTP servers:");

  for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i)
  {
    if (sntp_getservername(i))
    {
      ESP_LOGI(TAG, "server %d: %s", i, sntp_getservername(i));
    }
    else
    {
      // we have either IPv4 or IPv6 address, let's print it
      char buff[INET6_ADDRSTRLEN];
      ip_addr_t const *ip = sntp_getserver(i);
      if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
        ESP_LOGI(TAG, "server %d: %s", i, buff);
    }
  }
}

char *get_timestamp(void)
{
  return timestamp_buf;
}