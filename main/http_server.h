#include <string.h>

#include <esp_event.h>
#include <esp_system.h>
#include <sys/param.h>
#include <esp_http_server.h>

#include <wifi_handler.h>
#include <html_creator.h>

#define HTTP_STACK_SIZE 8192

void http_server_config(void);
