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

#ifndef __BSP_CONF_H__
#define __BSP_CONF_H__

#define CONFIG_CLOCK_ENABLE
#if defined(CONFIG_CLOCK_ENABLE)
#define CONFIG_CLOCK_NAME "lm3s9b96 clock driver"
#define CONFIG_CLOCK_LABEL lm3s9b96_clock_driver
#endif

#define CONFIG_GPIO_ENABLE
#if defined(CONFIG_GPIO_ENABLE)
#define CONFIG_GPIOA_NAME "lm3s9b96 gpio a driver"
#define CONFIG_GPIOA_LABEL lm3s9b96_gpio_a_driver

#define CONFIG_GPIOB_NAME "lm3s9b96 gpio b driver"
#define CONFIG_GPIOB_LABEL lm3s9b96_gpio_b_driver

#define CONFIG_GPIOC_NAME "lm3s9b96 gpio c driver"
#define CONFIG_GPIOC_LABEL lm3s9b96_gpio_c_driver

#define CONFIG_GPIOD_NAME "lm3s9b96 gpio d driver"
#define CONFIG_GPIOD_LABEL lm3s9b96_gpio_d_driver

#define CONFIG_GPIOE_NAME "lm3s9b96 gpio e driver"
#define CONFIG_GPIOE_LABEL lm3s9b96_gpio_e_driver

#define CONFIG_GPIOF_NAME "lm3s9b96 gpio f driver"
#define CONFIG_GPIOF_LABEL lm3s9b96_gpio_f_driver

#define CONFIG_GPIOG_NAME "lm3s9b96 gpio g driver"
#define CONFIG_GPIOG_LABEL lm3s9b96_gpio_g_driver

#define CONFIG_GPIOH_NAME "lm3s9b96 gpio h driver"
#define CONFIG_GPIOH_LABEL lm3s9b96_gpio_h_driver

#define CONFIG_GPIOJ_NAME "lm3s9b96 gpio j driver"
#define CONFIG_GPIOJ_LABEL lm3s9b96_gpio_j_driver
#endif

#define CONFIG_UART0_ENABLE
#if defined(CONFIG_UART0_ENABLE)
#define CONFIG_UART0_NAME "lm3s9b96 uart0 driver"
#define CONFIG_UART0_LABEL lm3s9b96_uart0_driver
#define CONFIG_UART0_TX_RING_BUFF_SIZE (2 * 1024)
#define CONFIG_UART0_RX_RING_BUFF_SIZE (2 * 1024)
#define CONFIG_UART0_HW_BAUDRATE 115200
#define CONFIG_UART0_HW_CONFIGS UART_CONFIG_WORD_LENGTH_8B | \
	UART_CONFIG_STOP_BITS_1 | UART_CONFIG_PARITY_NONE | \
	UART_CONFIG_MODE_TX_RX
#endif

#endif /* __BSP_CONF_H__ */
