idf_component_register(SRCS "timer_driver.c"
		       REQUIRES esp_driver_gptimer
                       INCLUDE_DIRS "include")
