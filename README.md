# ESP32_KeepPowerbankAlive


This guide shows, how to keep a powerbank alive while using it with a microcontroller. In this case, we use the Espressif ESP32.

## Basics

While using a powerbank with a mcu, most powerbanks while simply shut down after a certain period. That is because of the minimum load which a powerbank requires.
Is current load below the minimum load, the powerbank will shutdown after a few seconds.
In most cases, the load of the mcu is some mA below this required minimum load, so you can't use a mcu while keeping a powerbank alive.
What we do is to do push the load of the powerbank every X seconds for a specific amount. We use this push as some kind of 'wake up sigal'.

What we need to know: minimal load of powerbank and load of mcu

In this guide we use:
* ESP32 from Espressif Systems as MCU
* VOLTCRAFT PB-17 Li-Ion 10400 mAh Powerbank
* NPN Transistor (in this case BC547)
* some resistors and a LED

## Circuit
![Circuit](https://github.com/Staubgeborener/ESP32_KeepPowerbankAlive/blob/master/esp32-wroom_powerbank_circuit.png "Circuit")

The circuit is rather simple. Connect the collector of the npn transistor to the 5 V pin of the esp32, the ermitter to ground and the base to any free gpio. The led and R3 is not necessary, but it's nice to visualize the wake up signal.
The VOLTCRAFT Powerbank needs a minimal load of 100 mA (just look into the datasheet or ask the manufacturer). An ESP32 needs without doing anything around 0.4 A. That means, we only need further 0.6 A to keep the powerbank alive. 
In this scenario we will get 0.12 A. That's truly higher as 0.1 A, but better too much than too little.

What we want to calculate is I. At R1 (base) we have: 

![equation R1](http://latex.codecogs.com/gif.latex?I%20%3D%20%5Cfrac%7BU%7D%7BR%7D%20%3D%20%5Cfrac%7B%283%2C3%20V%20-%200%2C7%20V%29%7D%7B1000%20%5COmega%20%7D%20%3D%202.6%20mA)  

2.6 mA of 100 mA seems low. And it is. But keep in mind: The maximum current of a esp32 gpio pin is [12 mA](https://esp32.com/viewtopic.php?p=9557&sid=85a6f03df066f869d075708be51dbcb8#p9557). 

Next we've got R2 (collector):

![equation R2](http://latex.codecogs.com/gif.latex?I%20%3D%20%5Cfrac%7BU%7D%7BR%7D%20%3D%20%5Cfrac%7B5%20V%7D%7B59%20%5COmega%7D%20%3D%2084.75%20mA)

Sounds better. So in total:

![equation total](http://latex.codecogs.com/gif.latex?40%20mA%20&plus;%202.6%20mA%20&plus;%2084.75%20mA%20%3D%20127%20mA%20%3D%200.127%20A)

Everytime, we trigger the gpio, he generate a pulse of 0.12 A. That's enough for the Voltcraft.

## Program code

The code is even simpler.

```C
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define BLINK_GPIO 21   //define gpio
unsigned long previousMillis;

void blink_task(void* pvParameter)
{
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        unsigned long currentMillis = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (currentMillis - previousMillis >= 9000) {
            previousMillis = currentMillis;
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(50 / portTICK_PERIOD_MS);
            gpio_set_level(BLINK_GPIO, 0);
        }
    }
}

void app_main()
{
    xTaskCreate(&blink_task, "blink_task", 512, NULL, 5, NULL);
}
```

This code is written with the [Espressif IoT Development Framework (ESP-IDF)](https://github.com/espressif/esp-idf). We customized the [blink example](https://github.com/espressif/esp-idf/tree/master/examples/get-started/blink) a little bit. 
Basically we trigger the IO21 every 9 seconds. You have to find out the time, when your powerbank turns off. In my case, the powerbank shut downs after 10 seconds. So 9 seconds is a nice value. Customize your time in line 17:

```c
if (currentMillis - previousMillis >= 9000) {
```

IO21 will be triggered for 50 ms. I recommend to calculate the difference between the current time and the time since the mcu is booted instead of a simple delay. A delay will block the rest of your code. This means, you can also calculate this value in line 20:

```c
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
```
              
but i think, 50 ms is such a low value so you can also use also a delay. But do what you want.

The same script for the [Arduino IDE](https://github.com/arduino/Arduino):

```c
#define BLINK_GPIO 21 // define gpio
unsigned long previousMillis;

void loop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 5000) {
        previousMillis = currentMillis;
        digitalWrite(BLINK_GPIO, HIGH); 
        delay(50);
        digitalWrite(BLINK_GPIO, LOW);
    }
}

void setup()
{
    pinMode(BLINK_GPIO, OUTPUT);
}
```

## Installation / Flashing
For ESP-IDF download the files: [LINK](https://github.com/Staubgeborener/ESP32_KeepPowerbankAlive/tree/master/esp-idf) and compile with ``make flash``

For Arduino-IDE download the file and compile with IDE: [LINK](https://github.com/Staubgeborener/ESP32_KeepPowerbankAlive/tree/master/arduino-ide)

## License
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

The program code is licensed under GNU General Public License 3. Take a look at the [license file](https://github.com/Staubgeborener/Scapy_RandomPaketSender/blob/master/LICENSE) for more informations.
