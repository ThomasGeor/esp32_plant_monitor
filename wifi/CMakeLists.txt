idf_component_register(SRCS "wifi.c"
		       PRIV_REQUIRES espressif__mdns
	               REQUIRES http_server esp_wifi nvs_flash
		       INCLUDE_DIRS "include")
