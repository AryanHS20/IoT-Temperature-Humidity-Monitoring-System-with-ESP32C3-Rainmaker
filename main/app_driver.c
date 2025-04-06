#include <freertos/FreeRTOS.h>  // FreeRTOS core functionality for task scheduling and synchronization
#include <freertos/timers.h>  // Timer API for periodic tasks (used for sensor reading intervals)
#include <sdkconfig.h>  // ESP32 SDK configuration settings (defines hardware, software parameters)
#include <esp_rmaker_core.h>  // ESP RainMaker core API to create and manage IoT devices and services
#include <esp_rmaker_standard_types.h>  // Standard RainMaker types for various IoT parameters
#include <esp_rmaker_standard_params.h>  // Standard RainMaker parameters (e.g., temperature, humidity) for device integration
#include <driver/gpio.h>  // GPIO driver for configuring and controlling ESP32 GPIO pins
#include "app_reset.h"  // Custom header file for app reset functionality (e.g., WiFi and factory reset handling)
#include "app_priv.h"  // Private app-related header file, likely for internal app functionality
#include "dht.h"  // DHT sensor library to interface with DHT11 sensor for temperature and humidity readings
#include "esp_log.h"  // ESP32 logging library for debugging and information output to console

#define LED_GPIO GPIO_NUM_10  // GPIO pin for controlling the LED (used as an alert indicator)
#define BUTTON_GPIO 5  // GPIO pin for the reset button
#define SWITCH_GPIO GPIO_NUM_4  // GPIO pin for switch control (optional in your case)
#define DHT_PIN GPIO_NUM_2  // GPIO pin for DHT sensor data (temperature and humidity)
#define DHT_TYPE DHT_TYPE_DHT11  // DHT11 sensor type (temperature and humidity sensor)
#define WIFI_RESET_BUTTON_TIMEOUT 3  // Timeout period for WiFi reset button in seconds
#define FACTORY_RESET_BUTTON_TIMEOUT 11  // Timeout period for factory reset button in seconds

// Declare a timer handle for periodic sensor readings
static TimerHandle_t sensor_timer;
static float g_temperature = DEFAULT_TEMPERATURE;  // Default temperature value
static float g_humidity = DEFAULT_HUMIDITY;  // Default humidity value

// Function to update the sensor readings and send data to RainMaker
static void app_sensor_update(TimerHandle_t handle)
{
    // Read the sensor data (temperature and humidity)
    if (dht_read_float_data(DHT_TYPE, DHT_PIN, &g_humidity, &g_temperature) == ESP_OK) {
        ESP_LOGI("DHT", "Temperature: %.1f°C, Humidity: %.1f%%", g_temperature, g_humidity);
        
        // Update the temperature parameter in RainMaker
        esp_rmaker_param_update_and_report(
            esp_rmaker_device_get_param_by_type(temp_sensor_device, ESP_RMAKER_PARAM_TEMPERATURE),
            esp_rmaker_float(g_temperature));

        // Update the humidity parameter in RainMaker
        esp_rmaker_param_t *humidity_param = esp_rmaker_device_get_param_by_name(humidity_sensor_device, "Humidity");
        if (humidity_param) {
            esp_rmaker_param_update_and_report(humidity_param, esp_rmaker_float(g_humidity));
        } else {
            ESP_LOGE("RainMaker", "Failed to get Humidity parameter!");  // Log error if humidity parameter not found
        }

        // Raise alert if the temperature exceeds 30°C
        if (g_temperature > 30) {
            char alert_msg[ESP_RMAKER_MAX_ALERT_LEN];
            snprintf(alert_msg, sizeof(alert_msg), "🔥 High Temperature Alert: %.1f°C!", g_temperature);
            ESP_LOGW("ALERT", "%s", alert_msg);  // Log the alert message
            esp_rmaker_raise_alert(alert_msg);  // Trigger the alert in RainMaker
        }
    } else {
        ESP_LOGE("DHT", "Failed to read from DHT sensor!");  // Log error if sensor reading fails
    }
}

// Getter function for the current temperature
float app_get_current_temperature() { return g_temperature; }

// Getter function for the current humidity
float app_get_current_humidity() { return g_humidity; }

// Function to initialize the sensor and set up the periodic update timer
esp_err_t app_sensor_init(void)
{
    // Create a timer for periodic sensor updates (using FreeRTOS timer API)
    sensor_timer = xTimerCreate("app_sensor_update_tm", pdMS_TO_TICKS(REPORTING_PERIOD * 1000),
                                pdTRUE, NULL, app_sensor_update);
    if (sensor_timer) {
        xTimerStart(sensor_timer, 0);  // Start the timer
        return ESP_OK;
    }
    return ESP_FAIL;  // Return error if the timer creation fails
}

// Function to initialize the hardware peripherals (LED, button, etc.)
void app_driver_init()
{
    // Initialize the GPIO pin for the LED
    gpio_reset_pin(LED_GPIO);  // Reset the pin configuration
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);  // Set the LED pin as output
    gpio_set_level(LED_GPIO, 0);  // Start with the LED turned off (0 is off, 1 is on)

    // Initialize the sensor
    app_sensor_init();

    // Register the reset buttons (WiFi and factory reset buttons)
    app_reset_button_register(app_reset_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LOW),
                              WIFI_RESET_BUTTON_TIMEOUT, FACTORY_RESET_BUTTON_TIMEOUT);
}
