#pragma once  // Ensures that the header file is included only once during compilation

#include <stdint.h>  // Provides standard integer types such as uint32_t, int32_t, etc.
#include <stdbool.h>  // Provides the boolean type (true/false) for flag management
#include <esp_rmaker_core.h>  // Includes the ESP RainMaker core API for device and parameter management

// Define default values for temperature and humidity
#define DEFAULT_TEMPERATURE 0.0  // Default temperature value (in Celsius)
#define DEFAULT_HUMIDITY    0.0  // Default humidity value (percentage)
#define REPORTING_PERIOD    5    // Reporting period for sensor data in seconds

// Declare external variables for the devices that will be used in the app
// These variables are defined in the corresponding app_main.c or app_driver.c file
extern esp_rmaker_device_t *temp_sensor_device;  // Pointer to the temperature sensor device
extern esp_rmaker_device_t *humidity_sensor_device;  // Pointer to the humidity sensor device
extern esp_rmaker_device_t *switch_device;  // Pointer to the switch device that controls the LED

// Function declarations
void app_driver_init(void);  // Initializes the hardware components such as GPIO, sensors, etc.
float app_get_current_temperature();  // Returns the current temperature value from the sensor
float app_get_current_humidity();  // Returns the current humidity value from the sensor
