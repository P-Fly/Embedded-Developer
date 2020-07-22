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
