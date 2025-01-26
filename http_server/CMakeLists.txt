idf_component_register(SRCS "http_server.c"
                       REQUIRES html_pages esp_wifi esp_http_server sntp_client
                       INCLUDE_DIRS "include")
