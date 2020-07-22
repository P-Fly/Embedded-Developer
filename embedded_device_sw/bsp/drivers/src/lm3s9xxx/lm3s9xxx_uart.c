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
#include "ring_buff.h"
#include "drv_uart.h"

#define TARGET_IS_TEMPEST_RC1
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

#if defined(CONFIG_UART0_ENABLE)

/**
 * @brief   Uart handle definition.
 */
typedef struct {
#ifdef CONFIG_UART0_TX_RING_BUFF_SIZE
	ring_buff_t   tx;
#endif
#ifdef CONFIG_UART0_RX_RING_BUFF_SIZE
	ring_buff_t   rx;
#endif
	unsigned long uart_base;
} lm3s9xxx_uart_handle_t;

/**
 * @brief   Install one hardware configuration from a configuration space.
 *
 * @param   obj Pointer to the uart object handle.
 * @param   config Pointer to the configuration space.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_uart_configure(const object *		obj,
				    const uart_config_t *	config)
{
	lm3s9xxx_uart_handle_t *handle =
		(lm3s9xxx_uart_handle_t *)obj->object_data;
    unsigned long baudrate;
    unsigned long word_length;
    unsigned long stop_bits;
    unsigned long parity;

	baudrate = config->baudrate;

	/* Set Word Length */
	switch (UARD_CONFIG_WORD_LENGTH_MASK(config->configs)) {
    case UART_CONFIG_WORD_LENGTH_5B:
		word_length = UART_CONFIG_WLEN_5;
		break;
    case UART_CONFIG_WORD_LENGTH_6B:
		word_length = UART_CONFIG_WLEN_6;
		break;
	case UART_CONFIG_WORD_LENGTH_7B:
		word_length = UART_CONFIG_WLEN_7;
		break;
	case UART_CONFIG_WORD_LENGTH_8B:
		word_length = UART_CONFIG_WLEN_8;
		break;
	default:
		return -EINVAL;
	}

	/* Set Stop Bits */
	switch (UARD_CONFIG_STOP_BITS_MASK(config->configs)) {
	case UART_CONFIG_STOP_BITS_1:
		stop_bits = UART_CONFIG_STOP_ONE;
		break;
	case UART_CONFIG_STOP_BITS_2:
		stop_bits = UART_CONFIG_STOP_TWO;
		break;
	default:
		return -EINVAL;
	}

	/* Set Parity mode */
	switch (UARD_CONFIG_PARITY_MASK(config->configs)) {
	case UART_CONFIG_PARITY_NONE:
		parity = UART_CONFIG_PAR_NONE;
		break;
	case UART_CONFIG_PARITY_EVEN:
		parity = UART_CONFIG_PAR_EVEN;
        break;
	case UART_CONFIG_PARITY_ODD:
		parity = UART_CONFIG_PAR_ODD;
		break;
	default:
		return -EINVAL;
	}

    /* Configure the UART for 115,200, 8-N-1 operation */
    //MAP_UARTConfigSetExpClk(handle->uart_base, MAP_SysCtlClockGet(), baudrate, (word_length | stop_bits | parity));
    MAP_UARTConfigSetExpClk(UART0_BASE, MAP_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    /* Configure FIFO Level Select */
    MAP_UARTFIFOLevelSet(handle->uart_base, UART_FIFO_TX1_8, UART_FIFO_RX7_8);

    /* Enable the UART interrupt */
    MAP_IntEnable(INT_UART0);
    MAP_UARTIntEnable(handle->uart_base, UART_INT_RX | UART_INT_RT | UART_INT_TX);

    /* Enable the UART */
    MAP_UARTEnable(handle->uart_base);

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
static int lm3s9xxx_uart_write(const object *obj,
				const void *tx_buf, int tx_len)
{
	lm3s9xxx_uart_handle_t *handle =
		(lm3s9xxx_uart_handle_t *)obj->object_data;
	char *buff = (char *)tx_buf;
	int i;
	int ret;
    char ch;

	if (!handle)
		return -EINVAL;

	if (!tx_buf)
		return -EINVAL;

	if (tx_len < 0)
		return -EINVAL;

#ifdef CONFIG_UART0_TX_RING_BUFF_SIZE
	for (i = 0; i < tx_len; i++) {
        MAP_UARTCharPut(handle->uart_base, buff[i]);
		//ret = ring_buffer_write(&handle->tx, buff[i]);
		//if (ret)
			//break;
	}

//    MAP_IntDisable(handle->uart_base);

    //while (MAP_UARTSpaceAvail(handle->uart_base))
   //{
    //    ret = ring_buffer_read(&handle->rx, &ch);

    //    if (ret)
   //     {
   //         break;
   //     }

   //     MAP_UARTCharPutNonBlocking(handle->uart_base, ch);
   // }

//    MAP_IntEnable(handle->uart_base);

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
static int lm3s9xxx_uart_read(const object *obj,
			       void *rx_buf, int rx_len)
{
	lm3s9xxx_uart_handle_t *handle =
		(lm3s9xxx_uart_handle_t *)obj->object_data;
	char *buff = (char *)rx_buf;
	int i;
	int ret;

	if (!handle)
		return -EINVAL;

	if (!rx_buf)
		return -EINVAL;

	if (rx_len < 0)
		return -EINVAL;

#ifdef CONFIG_UART0_RX_RING_BUFF_SIZE
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
static void lm3s9xxx_uart_irq_handler(lm3s9xxx_uart_handle_t *handle)
{
	int ret;
	long value;
    char ch;
    unsigned long status;

    /* Get the interrrupt status */
    status = MAP_UARTIntStatus(handle->uart_base, true);

    /* Clear the asserted interrupts */
    MAP_UARTIntClear(handle->uart_base, status);

#ifdef CONFIG_UART0_TX_RING_BUFF_SIZE
    /* Handle TX interrupt */
    if (status & UART_INT_TX)
    {
        do
        {
            ret = ring_buffer_read(&handle->rx, &ch);

            if (ret)
            {
                break;
            }

            MAP_UARTCharPutNonBlocking(handle->uart_base, ch);
        }
        while (MAP_UARTSpaceAvail(handle->uart_base));
    }
#endif

#ifdef CONFIG_UART0_RX_RING_BUFF_SIZE
    /* Handle RX interrupt */
    if (status & (UART_INT_RX | UART_INT_RT))
    {
        do
        {
            value = MAP_UARTCharGetNonBlocking(handle->uart_base);

            if (-1 == value)
            {
                break;
            }

            ch = (char)value;

            (void)ring_buffer_write(&handle->rx, ch);
        }
        while (true);
    }
#endif
}
#endif

#ifdef CONFIG_UART0_ENABLE

#ifdef CONFIG_UART0_TX_RING_BUFF_SIZE
static char uart0_tx_ring_buff[CONFIG_UART0_TX_RING_BUFF_SIZE];
#endif

#ifdef CONFIG_UART0_RX_RING_BUFF_SIZE
static char uart0_rx_ring_buff[CONFIG_UART0_RX_RING_BUFF_SIZE];
#endif

static uart_intf_t uart0_intf =
{
	.configure	= lm3s9xxx_uart_configure,
	.read		= lm3s9xxx_uart_read,
	.write		= lm3s9xxx_uart_write,
};

static lm3s9xxx_uart_handle_t uart0_handle;

static uart_config_t uart0_config =
{
	.baudrate	= CONFIG_UART0_HW_BAUDRATE,
	.configs	= CONFIG_UART0_HW_CONFIGS,
};

/**
 * @brief   This function handles USART1 Global Interrupt.
 *
 * @retval  None.
 */
void USART0_IRQHandler(void)
{
	lm3s9xxx_uart_irq_handler(&uart0_handle);
}

/**
 * @brief   Probe the uart driver.
 *
 * @param   obj Pointer to the uart object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_uart0_probe(const object *obj)
{
	lm3s9xxx_uart_handle_t *handle =
		(lm3s9xxx_uart_handle_t *)obj->object_data;
	int ret;

	(void)memset(handle, 0, sizeof(lm3s9xxx_uart_handle_t));

    handle->uart_base = UART0_BASE;

    /* Enable the UART0 and GPIO Port A */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Configure the pin muxing for UART0 functions on port A0 and A1 */
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);

    /* Set GPIO A0 and A1 as UART pins */
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    MAP_UARTConfigSetExpClk(UART0_BASE, MAP_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    /* Configure FIFO Level Select */
    MAP_UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX7_8);

    /* Enable the UART interrupt */
    //MAP_IntEnable(INT_UART0);
    //MAP_UARTIntEnable(handle->uart_base, UART_INT_RX | UART_INT_RT | UART_INT_TX);

    /* Enable the UART */
    MAP_UARTEnable(UART0_BASE);

	//ret = uart_configure(obj, obj->object_config);
	//if (ret)
	//	return ret;

#ifdef CONFIG_UART0_TX_RING_BUFF_SIZE
	ret =
		ring_buffer_init(&handle->tx,
				 uart0_tx_ring_buff,
				 sizeof(uart0_tx_ring_buff));
	if (ret)
		return ret;
#endif

#ifdef CONFIG_UART0_RX_RING_BUFF_SIZE
	ret =
		ring_buffer_init(&handle->rx,
				 uart0_rx_ring_buff,
				 sizeof(uart0_rx_ring_buff));
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
static int lm3s9xxx_uart0_shutdown(const object *obj)
{
	return 0;
}

module_early_driver(CONFIG_UART0_NAME,
		    CONFIG_UART0_LABEL,
		    lm3s9xxx_uart0_probe,
		    lm3s9xxx_uart0_shutdown,
		    &uart0_intf, &uart0_handle, &uart0_config);

#endif
