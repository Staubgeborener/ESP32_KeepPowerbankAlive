#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define BLINK_GPIO 21 // LED-Pin für Fehlermeldung definieren
unsigned long previousMillis;

void blink_task(void* pvParameter)
{
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        unsigned long currentMillis = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (currentMillis - previousMillis >= 5000) {
            previousMillis = currentMillis;
            gpio_set_level(BLINK_GPIO, 1);
            unsigned long currentMillis = xTaskGetTickCount() * portTICK_PERIOD_MS;
            if (currentMillis - previousMillis >= 50) {
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
            gpio_set_level(BLINK_GPIO, 0);
        }
    }
}

void app_main()
{
    xTaskCreate(&blink_task, "blink_task", 512, NULL, 5, NULL);
}
