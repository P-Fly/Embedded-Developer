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

#include <string.h>
#include "object.h"
#include "err.h"
#include "ring_buff.h"
#include "stm32wbxx_hal.h"
#include "uart.h"

#if defined(CONFIG_UART1_ENABLE)

/**
 * @brief   Uart handle definition.
 */
typedef struct {
#ifdef CONFIG_UART1_TX_RING_BUFF_SIZE
	ring_buff_t		tx;
#endif
#ifdef CONFIG_UART1_RX_RING_BUFF_SIZE
	ring_buff_t		rx;
#endif
	UART_HandleTypeDef	uart;
	void *			private_data;
} stm32wbxx_uart_handle_t;

/**
 * @brief   Install one hardware configuration from a configuration space.
 *
 * @param   obj Pointer to the uart object handle.
 * @param   config Pointer to the configuration space.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_uart_configure(const object *		obj,
				    const uart_config_t *	config)
{
	stm32wbxx_uart_handle_t *handle =
		(stm32wbxx_uart_handle_t *)obj->object_data;

	handle->uart.Init.BaudRate = config->baudrate;

	/* Set Word Length */
	switch (UARD_CONFIG_WORD_LENGTH_MASK(config->configs)) {
	case UART_CONFIG_WORD_LENGTH_7B:
		handle->uart.Init.WordLength = UART_WORDLENGTH_7B;
		break;
	case UART_CONFIG_WORD_LENGTH_8B:
		handle->uart.Init.WordLength = UART_WORDLENGTH_8B;
		break;
	case UART_CONFIG_WORD_LENGTH_9B:
		handle->uart.Init.WordLength = UART_WORDLENGTH_9B;
		break;
	default:
		return -EINVAL;
	}

	/* Set Stop Bits */
	switch (UARD_CONFIG_STOP_BITS_MASK(config->configs)) {
	case UART_CONFIG_STOP_BITS_0_5:
		handle->uart.Init.StopBits = UART_STOPBITS_0_5;
		break;
	case UART_CONFIG_STOP_BITS_1:
		handle->uart.Init.StopBits = UART_STOPBITS_1;
		break;
	case UART_CONFIG_STOP_BITS_1_5:
		handle->uart.Init.StopBits = UART_STOPBITS_1_5;
		break;
	case UART_CONFIG_STOP_BITS_2:
		handle->uart.Init.StopBits = UART_STOPBITS_2;
		break;
	default:
		return -EINVAL;
	}

	/* Set Parity mode */
	switch (UARD_CONFIG_PARITY_MASK(config->configs)) {
	case UART_CONFIG_PARITY_NONE:
		handle->uart.Init.Parity = UART_PARITY_NONE;
		break;
	case UART_CONFIG_PARITY_EVEN:
		handle->uart.Init.Parity = UART_PARITY_EVEN;
		break;
	case UART_CONFIG_PARITY_ODD:
		handle->uart.Init.Parity = UART_PARITY_ODD;
		break;
	default:
		return -EINVAL;
	}

	/* Set Transfer Mode */
	switch (UARD_CONFIG_MODE_MASK(config->configs)) {
	case UART_CONFIG_MODE_TX:
		handle->uart.Init.Mode = UART_MODE_TX;
		break;
	case UART_CONFIG_MODE_RX:
		handle->uart.Init.Mode = UART_MODE_RX;
		break;
	case UART_CONFIG_MODE_TX_RX:
		handle->uart.Init.Mode = UART_MODE_TX_RX;
		break;
	default:
		return -EINVAL;
	}

	/* Use default config */
	handle->uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	handle->uart.Init.OverSampling = UART_OVERSAMPLING_16;
	handle->uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	handle->uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	handle->uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(&handle->uart) != HAL_OK)
		return -EIO;

	if (HAL_UARTEx_SetTxFifoThreshold(&handle->uart,
					  UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
		return -EIO;

	if (HAL_UARTEx_SetRxFifoThreshold(&handle->uart,
					  UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
		return -EIO;

	if (HAL_UARTEx_DisableFifoMode(&handle->uart) != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief   Write data to uart.
 *
 * @param   obj Pointer to the uart object handle.
 * @param   tx_buf Pointer to data buffer to write.
 * @param   tx_len Data length to write.
 *
 * @retval  The number of data bytes write to the slave on success,
 *          negative error code otherwise.
 */
static int stm32wbxx_uart_write(const object *obj,
				const void *tx_buf, int tx_len)
{
	stm32wbxx_uart_handle_t *handle =
		(stm32wbxx_uart_handle_t *)obj->object_data;
	char *buff = (char *)tx_buf;
	int i;
	int ret;

	if (!handle)
		return -EINVAL;

	if (!tx_buf)
		return -EINVAL;

	if (tx_len < 0)
		return -EINVAL;

#ifdef CONFIG_UART1_TX_RING_BUFF_SIZE
	for (i = 0; i < tx_len; i++) {
		ret = ring_buffer_write(&handle->tx, buff[i]);
		if (ret)
			break;
	}

	/* Enable the UART Transmit data register empty Interrupt */
	__HAL_UART_ENABLE_IT(&handle->uart, UART_IT_TXE);
#else
	/* TBD: Not support*/
#ERROR ("Not support");
#endif

	return i;
}

/**
 * @brief   Read data from uart.
 *
 * @param   obj Pointer to the uart object handle.
 * @param   rx_buf Pointer to data buffer to read.
 * @param   rx_len Data length to read.
 *
 * @retval  The number of data bytes read from the slave on success,
 *          negative error code otherwise.
 */
static int stm32wbxx_uart_read(const object *obj,
			       void *rx_buf, int rx_len)
{
	stm32wbxx_uart_handle_t *handle =
		(stm32wbxx_uart_handle_t *)obj->object_data;
	char *buff = (char *)rx_buf;
	int i;
	int ret;

	if (!handle)
		return -EINVAL;

	if (!rx_buf)
		return -EINVAL;

	if (rx_len < 0)
		return -EINVAL;

#ifdef CONFIG_UART1_RX_RING_BUFF_SIZE
	for (i = 0; i < rx_len; i++) {
		ret = ring_buffer_read(&handle->rx, &buff[i]);
		if (ret)
			break;
	}
#else
	/* TBD: Not support*/
#ERROR ("Not support");
#endif

	return i;
}

/**
 * @brief   Handles uart interrupt request.
 *
 * @param   handle Pointer to the uart driver handle.
 *
 * @retval  None.
 */
static void stm32wbxx_uart_irq_handler(stm32wbxx_uart_handle_t *handle)
{
	int ret;
	char value;

#ifdef CONFIG_UART1_TX_RING_BUFF_SIZE
	/* UART in mode Transmitter */
	if (__HAL_UART_GET_IT_SOURCE(&handle->uart, UART_IT_TXE)) {
		if (__HAL_UART_GET_FLAG(&handle->uart, UART_FLAG_TXE)) {
			ret = ring_buffer_read(&handle->tx, &value);
			if (ret)
				/* Disable the UART Transmit data register empty Interrupt */
				__HAL_UART_DISABLE_IT(&handle->uart,
						      UART_IT_TXE);
			else
				/* Flushes the UART DR register */
				handle->uart.Instance->TDR = value;
		}
	}
#endif

#ifdef CONFIG_UART1_RX_RING_BUFF_SIZE
	/* UART in mode Receiver */
	if (__HAL_UART_GET_IT_SOURCE(&handle->uart, UART_IT_RXNE)) {
		if (__HAL_UART_GET_FLAG(&handle->uart, UART_FLAG_RXNE)) {
			value = handle->uart.Instance->TDR;

			(void)ring_buffer_write(&handle->rx, value);
		}
	}
#endif
}
#endif

#ifdef CONFIG_UART1_ENABLE

#ifdef CONFIG_UART1_TX_RING_BUFF_SIZE
static char uart1_tx_ring_buff[CONFIG_UART1_TX_RING_BUFF_SIZE];
#endif

#ifdef CONFIG_UART1_RX_RING_BUFF_SIZE
static char uart1_rx_ring_buff[CONFIG_UART1_RX_RING_BUFF_SIZE];
#endif

static uart_intf_t uart1_intf =
{
	.configure	= stm32wbxx_uart_configure,
	.read		= stm32wbxx_uart_read,
	.write		= stm32wbxx_uart_write,
};

static stm32wbxx_uart_handle_t uart1_handle;

static uart_config_t uart1_config =
{
	.baudrate	= CONFIG_UART1_HW_BAUDRATE,
	.configs	= CONFIG_UART1_HW_CONFIGS,
};

/**
 * @brief   This function handles USART1 Global Interrupt.
 *
 * @retval  None.
 */
void USART1_IRQHandler(void)
{
	stm32wbxx_uart_irq_handler(&uart1_handle);
}

/**
 * @brief   Initialize uart MSP.
 *
 * @param   uart Pointer to the uart driver handle.
 *
 * @retval  None.
 */
static void stm32wbxx_uart1_msp_init(UART_HandleTypeDef *uart)
{
	GPIO_InitTypeDef gpio;

	(void)memset(&gpio, 0, sizeof(gpio));

	/**
	 * USART1 GPIO Configuration
	 * PB6 ------> USART1_TX
	 * PB7 ------> USART1_RX
	 */
	gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	gpio.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOB, &gpio);

	/* TBD: USART1 interrupt Init */
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
 * @brief   Probe the uart driver.
 *
 * @param   obj Pointer to the uart object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_uart1_probe(const object *obj)
{
	stm32wbxx_uart_handle_t *handle =
		(stm32wbxx_uart_handle_t *)obj->object_data;
	int ret;

	(void)memset(handle, 0, sizeof(stm32wbxx_uart_handle_t));

	handle->uart.Instance = USART1;

	if (HAL_UART_RegisterCallback(&handle->uart, HAL_UART_MSPINIT_CB_ID,
				      stm32wbxx_uart1_msp_init) != HAL_OK)
		return -EIO;

	ret = uart_configure(obj, obj->object_config);
	if (ret)
		return ret;

#ifdef CONFIG_UART1_TX_RING_BUFF_SIZE
	ret =
		ring_buffer_init(&handle->tx,
				 uart1_tx_ring_buff,
				 sizeof(uart1_tx_ring_buff));
	if (ret)
		return ret;
#endif

#ifdef CONFIG_UART1_RX_RING_BUFF_SIZE
	ret =
		ring_buffer_init(&handle->rx,
				 uart1_rx_ring_buff,
				 sizeof(uart1_rx_ring_buff));
	if (ret)
		return ret;
#endif

	return 0;
}

/**
 * @brief   Remove the uart driver.
 *
 * @param   obj Pointer to the uart object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_uart1_shutdown(const object *obj)
{
	return 0;
}

module_early_driver(CONFIG_UART1_NAME,
		    stm32wbxx_uart1_probe,
		    stm32wbxx_uart1_shutdown,
		    &uart1_intf, &uart1_handle, &uart1_config);

#endif
