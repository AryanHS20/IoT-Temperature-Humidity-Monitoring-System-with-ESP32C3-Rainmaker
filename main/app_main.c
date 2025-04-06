#include <string.h>  // For string manipulation functions like strcmp
#include <freertos/FreeRTOS.h>  // FreeRTOS core functionality for task management
#include <freertos/task.h>  // Task API for creating and managing FreeRTOS tasks
#include <esp_log.h>  // ESP32 logging library for debugging and information output
#include <nvs_flash.h>  // Non-volatile storage (NVS) initialization and management
#include <driver/gpio.h>  // GPIO driver for configuring and controlling ESP32 GPIO pins
#include <esp_rmaker_core.h>  // ESP RainMaker core API for creating and managing IoT devices
#include <esp_rmaker_standard_params.h>  // Standard RainMaker parameters (temperature, humidity, etc.)
#include <esp_rmaker_standard_devices.h>  // Standard RainMaker devices (e.g., temperature, humidity sensors)
#include <esp_rmaker_ota.h>  // OTA (Over-The-Air) update support for firmware updates
#include <app_network.h>  // Custom header for network setup (e.g., Wi-Fi connection)
#include <app_insights.h>  // Custom header for enabling insights (device diagnostics and analytics)
#include "app_priv.h"  // Private app-related header file, likely for internal app functionality

#define LED_GPIO GPIO_NUM_10  // GPIO pin for controlling the LED (used as an alert indicator)
static const char *TAG = "app_main";  // Tag for logging (used to identify logs from this file)

esp_rmaker_device_t *temp_sensor_device;  // Pointer to the temperature sensor device
esp_rmaker_device_t *humidity_sensor_device;  // Pointer to the humidity sensor device
esp_rmaker_device_t *switch_device;  // Pointer to the switch device controlling the LED

// Callback function to handle the switch (LED) state change in RainMaker
static esp_err_t app_switch_write_cb(const esp_rmaker_device_t *device,
    const esp_rmaker_param_t *param,
    const esp_rmaker_param_val_t val,
    void *priv,
    esp_rmaker_write_ctx_t *ctx)
{
    const char *param_name = esp_rmaker_param_get_name(param);
    
    if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {  // Check if the power parameter is being updated
        ESP_LOGI(TAG, "Received LED switch value: %s", val.val.b ? "ON" : "OFF");  // Log the new switch state
        gpio_set_level(LED_GPIO, val.val.b ? 1 : 0);  // Set the LED GPIO based on the value (ON/OFF)
        esp_rmaker_param_update_and_report(param, val);  // Update the RainMaker parameter with the new value
    }
    return ESP_OK;  // Return success
}

void app_main()
{
    /* Initialize driver (including LED GPIO) */
    app_driver_init();  // Initialize the hardware components like LED and sensors

    /* Initialize NVS (Non-Volatile Storage) */
    esp_err_t err = nvs_flash_init();  // Initialize the NVS system (for Wi-Fi credentials and other settings)
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {  // Check if NVS needs erasing
        ESP_ERROR_CHECK(nvs_flash_erase());  // Erase NVS data if required
        err = nvs_flash_init();  // Re-initialize NVS after erasure
    }
    ESP_ERROR_CHECK(err);  // Check for errors in NVS initialization

    /* Initialize Wi-Fi */
    app_network_init();  // Initialize the Wi-Fi connection

    /* Initialize RainMaker */
    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,  // Disable time synchronization (optional based on your requirement)
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "DHT11 Sensor");
    if (!node) {
        ESP_LOGE(TAG, "Could not initialize node. Aborting!!!");  // Log error if node initialization fails
        vTaskDelay(5000 / portTICK_PERIOD_MS);  // Wait before aborting to allow debug output
        abort();  // Abort the program if RainMaker node initialization fails
    }

    /* Create Temperature Sensor Device */
    temp_sensor_device = esp_rmaker_temp_sensor_device_create("Temperature Sensor", NULL, app_get_current_temperature());
    esp_rmaker_node_add_device(node, temp_sensor_device);  // Add the temperature sensor device to the RainMaker node

    /* Create Humidity Sensor Device */
    humidity_sensor_device = esp_rmaker_device_create("Humidity Sensor", NULL, NULL);
    esp_rmaker_param_t *humidity_param = esp_rmaker_param_create(
        "Humidity", NULL, esp_rmaker_float(app_get_current_humidity()), PROP_FLAG_READ);  // Create a parameter for humidity
    esp_rmaker_device_add_param(humidity_sensor_device, humidity_param);  // Add humidity parameter to the device
    esp_rmaker_device_assign_primary_param(humidity_sensor_device, humidity_param);  // Assign primary parameter
    esp_rmaker_node_add_device(node, humidity_sensor_device);  // Add humidity sensor to RainMaker node

    /* Create Switch Device for LED */
    switch_device = esp_rmaker_switch_device_create("LED Switch", NULL, false);  // Create a switch device for LED
    esp_rmaker_device_add_cb(switch_device, app_switch_write_cb, NULL);  // Add the callback to handle state changes
    esp_rmaker_node_add_device(node, switch_device);  // Add switch device to RainMaker node
    esp_rmaker_param_t *power_param = esp_rmaker_param_create(
        ESP_RMAKER_DEF_POWER_NAME, NULL, esp_rmaker_bool(false), 
        PROP_FLAG_READ | PROP_FLAG_WRITE);  // Create a power parameter for controlling the LED
    esp_rmaker_device_add_param(switch_device, power_param);  // Add power parameter to the switch device
    esp_rmaker_device_assign_primary_param(switch_device, power_param);  // Assign primary parameter to switch device
    ESP_LOGI(TAG, "LED Switch device created");  // Log the creation of the switch device

    /* Enable OTA (Over-The-Air) updates */
    esp_rmaker_ota_enable_default();  // Enable default OTA functionality for firmware updates

    /* Enable Insights (Diagnostics and Analytics) */
    app_insights_enable();  // Enable insights for monitoring device status and performance

    /* Start RainMaker */
    esp_rmaker_start();  // Start the RainMaker service, enabling device control and monitoring

    /* Start Wi-Fi */
    err = app_network_start(POP_TYPE_RANDOM);  // Start Wi-Fi connection
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not start Wi-Fi. Aborting!!!");  // Log error if Wi-Fi fails to start
        vTaskDelay(5000 / portTICK_PERIOD_MS);  // Wait before aborting to allow debug output
        abort();  // Abort the program if Wi-Fi initialization fails
    }
}
