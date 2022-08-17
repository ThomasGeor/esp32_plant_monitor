#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include "esp_netif.h"

#include "mdns.h"
#include "lwip/apps/netbiosns.h"

#define WIFI_SSID
#define WIFI_PASS
#define MAXIMUM_RETRY 2

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_TIMEOUT 300

#define MDNS_HOST_NAME "plant"
#define MDNS_INSTANCE "esp home web server"

void wifi_init(void);
