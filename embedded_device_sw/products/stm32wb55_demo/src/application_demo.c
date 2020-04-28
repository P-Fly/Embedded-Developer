/**
 * Embedded Device Software
 * Copyright (C) 2020 Peter.Peng
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "object.h"
#include "err.h"
#include "stm32wbxx_hal.h"      /* TBD: */
#include "uart.h"
#include "crc.h"

#define LEDn                                    3
#define LED1_PIN                                GPIO_PIN_5
#define LED1_GPIO_PORT                          GPIOB
#define LED2_PIN                                GPIO_PIN_0
#define LED2_GPIO_PORT                          GPIOB
#define LED3_PIN                                GPIO_PIN_1
#define LED3_GPIO_PORT                          GPIOB

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)         __HAL_RCC_GPIOB_CLK_ENABLE() /* All Led on same port */
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)        __HAL_RCC_GPIOB_CLK_ENABLE() /* All Led on same port */

typedef enum
{
    LED1 = 0,
    LED2 = 1,
    LED3 = 2,
    /* Color led aliases */
    LED_BLUE   = LED1,
    LED_GREEN  = LED2,
    LED_RED    = LED3
} Led_TypeDef;

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN};

void bsp_led_init(Led_TypeDef Led)
{
    GPIO_InitTypeDef  gpioinitstruct = {0};

    /* Enable the GPIO_LED Clock */
    LEDx_GPIO_CLK_ENABLE(Led);

    /* Configure the GPIO_LED pin */
    gpioinitstruct.Pin = GPIO_PIN[Led];
    gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpioinitstruct.Pull = GPIO_NOPULL;
    gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIO_PORT[Led], &gpioinitstruct);

    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}

void bsp_led_on(Led_TypeDef Led)
{
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
}

void bsp_led_off(Led_TypeDef Led)
{
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}

int app_demo_init(void)
{
    bsp_led_init(LED_BLUE);
    bsp_led_init(LED_GREEN);
    bsp_led_init(LED_RED);

    bsp_led_on(LED_BLUE);
    bsp_led_on(LED_GREEN);
    bsp_led_on(LED_RED);

    return 0;
}

void app_demo_1_thread(void const * argument);
void app_demo_2_thread(void const * argument);

osThreadDef(app_demo_1, app_demo_1_thread, osPriorityNormal, 0, 256);
osThreadDef(app_demo_2, app_demo_2_thread, osPriorityNormal, 0, 256);

void app_demo_1_thread(void const * argument)
{
    object* uart;

    (void) argument;

    uart = object_get_binding(CONFIG_UART1_NAME);

    osDelay(1000);

    for (;;)
    {
        bsp_led_on(LED_BLUE);

        uart_write(uart, "Blue On\r\n", strlen("Blue On\r\n"));
        printf("Trace OK\r\n");

        osDelay(1000);

        bsp_led_off(LED_BLUE);

        uart_write(uart, "Blue Off\r\n", strlen("Blue Off\r\n"));
        printf("Trace OK\r\n");

        osDelay(1000);
    }
}

void app_demo_2_thread(void const * argument)
{
    object* uart;

    (void) argument;

    uart = object_get_binding(CONFIG_UART1_NAME);

    osDelay(2000);

    for (;;)
    {
        bsp_led_on(LED_GREEN);

        uart_write(uart, "Green On\r\n", strlen("Green On\r\n"));

        osDelay(1000);

        bsp_led_off(LED_GREEN);

        uart_write(uart, "Green Off\r\n", strlen("Green Off\r\n"));

        osDelay(1000);
    }
}

int app_demo_probe(const object* obj)
{
    app_demo_init();

    osThreadCreate(osThread(app_demo_1), NULL);
    osThreadCreate(osThread(app_demo_2), NULL);

    return 0;
}

module_application(Demo, app_demo_probe, NULL, NULL, NULL, NULL);

