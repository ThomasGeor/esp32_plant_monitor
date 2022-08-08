#include "http_server.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */
#define EXAMPLE_ESP_WIFI_SSID "Vodafone_2.4G-12861"
#define EXAMPLE_ESP_WIFI_PASS "tom_bill_katina"
#define EXAMPLE_MAX_STA_CONN 2
#define EXAMPLE_ESP_MAXIMUM_RETRY 2

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *TAGE = "example";
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

void temperature_reading(char *buffer);

/* An HTTP GET handler */
static esp_err_t temperature_get_handler(httpd_req_t *req)
{
  char resp[50];
  temperature_reading(resp);
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

static const httpd_uri_t temperature = {
    .uri = "/temperature",
    .method = HTTP_GET,
    .handler = temperature_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = "Temperature Reading!"};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /temperature and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /temperature or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /temperature). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
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
    /* Return ESP_FAIL to close underlying socket */
    return ESP_FAIL;
  }
  /* For any other URI send 404 and close socket */
  httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.lru_purge_enable = true;

  // Start the httpd server
  ESP_LOGI(TAGE, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK)
  {
    // Set URI handlers
    ESP_LOGI(TAGE, "Registering URI handlers");
    httpd_register_uri_handler(server, &temperature);
    return server;
  }

  ESP_LOGI(TAGE, "Error starting server!");
  return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
  // Stop the httpd server
  return httpd_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server)
  {
    ESP_LOGI(TAGE, "Stopping webserver");
    if (stop_webserver(*server) == ESP_OK)
    {
      *server = NULL;
    }
    else
    {
      ESP_LOGE(TAGE, "Failed to stop http server");
    }
  }
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server == NULL)
  {
    ESP_LOGI(TAGE, "Starting webserver");
    *server = start_webserver();
  }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
    {
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI(TAGE, "retry to connect to the AP");
    }
    else
    {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGI(TAGE, "connect to the AP fail");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAGE, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void temperature_reading(char *buffer)
{
  uint8_t temperature_value;
  i2c_master_read_temp(I2C_MASTER_NUM, &temperature_value);
  sprintf(buffer, "Temperature is : %d", temperature_value);
  ESP_LOGI(TAGE, "Temperature is : %d", temperature_value);
  // set standby mode for testing (5uA consuption)
  i2c_master_set_tc74_mode(I2C_MASTER_NUM, SET_STANBY_VALUE);
}

void http_server_config(void)
{
  static httpd_handle_t server = NULL;
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_t *p_netif = esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &wifi_event_handler,
                                                      NULL,
                                                      NULL));

  wifi_config_t wifi_config = {
      .sta = {
          .ssid = EXAMPLE_ESP_WIFI_SSID,
          .password = EXAMPLE_ESP_WIFI_PASS}};

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAGE, "wifi_init_sta finished.");

  /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
   * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         500);
  if (bits & WIFI_CONNECTED_BIT)
  {
    ESP_LOGI(TAGE, "connected to ap SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
  }
  else if (bits & WIFI_FAIL_BIT)
  {
    ESP_LOGI(TAGE, "Failed to connect to SSID:%s, password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
  }
  else
  {
    ESP_LOGE(TAGE, "UNEXPECTED EVENT");
  }

  esp_netif_ip_info_t if_info;
  ESP_ERROR_CHECK(esp_netif_get_ip_info(p_netif, &if_info));
  ESP_LOGE("Geros", "ESP32 IP:" IPSTR, IP2STR(&if_info.ip));

  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

  /* Start the server for the first time */
  server = start_webserver();
}