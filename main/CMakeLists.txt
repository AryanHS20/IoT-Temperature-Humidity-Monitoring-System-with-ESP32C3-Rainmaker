idf_component_register(SRCS ./app_driver.c ./app_main.c
                       INCLUDE_DIRS "."
                       REQUIRES dht esp_idf_lib_helpers nvs_flash 
                       PRIV_REQUIRES app_network app_reset app_insights)