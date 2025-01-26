/*
 * Brief: Http local server creation and handling.
 * author: Thomas Georgiadis
 */
#include "esp_log.h"
#include "esp_system.h"
#include "sys/param.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "html_pages.h"
#include "sntp_client.h"

#define HTTP_STACK_SIZE 8192 //bytes

static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server_handler = NULL;

/* Static functions declarations */
static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data);
static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
static esp_err_t temperature_get_handler(httpd_req_t *req);
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
static httpd_handle_t start_webserver(void);
static esp_err_t stop_webserver(httpd_handle_t server);

// URI handlers
static const httpd_uri_t temperature = {
    .uri = "/temperature",
    .method = HTTP_GET,
    .handler = temperature_get_handler,
    .user_ctx = "Temperature Reading"};

/* Function implementations*/

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server == NULL)
  {
    ESP_LOGI(TAG, "Starting webserver");
    sntp();
    *server = start_webserver();
  }
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server)
  {
    ESP_LOGI(TAG, "Stopping webserver");
    if (stop_webserver(*server) == ESP_OK)
    {
      *server = NULL;
    }
    else
    {
      ESP_LOGE(TAG, "Failed to stop http server");
    }
  }
}

/* An HTTP GET handler */
static esp_err_t temperature_get_handler(httpd_req_t *req)
{
  sntp();
  char resp[HTTP_STACK_SIZE / 2] = {};
  instant_status(resp);
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
  if (strcmp("/temperature", req->uri) == 0)
  {
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/temperature URI is not available");
    /* Return ESP_OK to keep underlying socket open */
    return ESP_OK;
  }
  else if (strcmp("/echo", req->uri) == 0)
  {
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
    return ESP_FAIL;
  }
  /* For any other URI send 404 and close socket */
  httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
  esp_log_level_set(TAG, ESP_LOG_ERROR);
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.lru_purge_enable = true;
  config.stack_size = HTTP_STACK_SIZE;

  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK)
  {
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &temperature);
    return server;
  }

  ESP_LOGI(TAG, "Error starting server!");
  return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
  return httpd_stop(server);
}

void http_server_config(void)
{
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server_handler));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server_handler));
}
