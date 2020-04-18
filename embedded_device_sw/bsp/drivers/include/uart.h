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

#ifndef __UART_H__
#define __UART_H__

#include <stddef.h>
#include "object.h"
#include "err.h"

/* Word Length */
#define UART_CONFIG_WORD_LENGTH_7B              (0 << 0)
#define UART_CONFIG_WORD_LENGTH_8B              (1 << 0)
#define UART_CONFIG_WORD_LENGTH_9B              (2 << 0)
#define UARD_CONFIG_WORD_LENGTH_MASK(config)    ((config) & (0x03 << 0))

/* Stop Bits */
#define UART_CONFIG_STOP_BITS_0_5               (0 << 2)
#define UART_CONFIG_STOP_BITS_1                 (1 << 2)
#define UART_CONFIG_STOP_BITS_1_5               (2 << 2)
#define UART_CONFIG_STOP_BITS_2                 (3 << 2)
#define UARD_CONFIG_STOP_BITS_MASK(config)      ((config) & (0x03 << 2))

/* Parity mode */
#define UART_CONFIG_PARITY_NONE                 (0 << 4)
#define UART_CONFIG_PARITY_EVEN                 (1 << 4)
#define UART_CONFIG_PARITY_ODD                  (2 << 4)
#define UARD_CONFIG_PARITY_MASK(config)         ((config) & (0x03 << 4))

/* Transfer Mode */
#define UART_CONFIG_MODE_TX                     (0 << 6)
#define UART_CONFIG_MODE_RX                     (1 << 6)
#define UART_CONFIG_MODE_TX_RX                  (2 << 6)
#define UARD_CONFIG_MODE_MASK(config)           ((config) & (0x03 << 6))

/**
 * @brief   Uart configuration structure.
 *
 * configs is a bit field with the following parts:
 *      Word Length     [ 0 : 1 ]
 *      Stop Bits       [ 2 : 3 ]
 *      Parity Mode     [ 4 : 5 ]
 *      Transfer Mode   [ 6 : 7 ]
 *      RESERVED        [ 8 : 31 ] - Undefined or device-specific usage.
 */
typedef struct
{
    unsigned int baudrate;
    unsigned int configs;
} uart_config_t;

/**
 * @brief   Uart interface definitions
 */
typedef struct
{
    int (*configure)(const object* obj, const uart_config_t* config);
    int (*write)(const object* obj, const void* tx_buf, int tx_len);
    int (*read)(const object* obj, void* rx_buf, int rx_len);
} uart_intf_t;

/**
 * @brief   Install one hardware configuration from a configuration space.
 *
 * @param   obj Pointer to the uart object handle.
 * @param   config Pointer to the configuration space.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static inline int uart_configure(const object* obj, const uart_config_t* config)
{
    uart_intf_t* intf;

    if (!obj)
        return -EINVAL;

    intf = (uart_intf_t*)obj->object_intf;
    if ((intf == NULL)
        || (intf->configure == NULL))
        return -ENOSUPPORT;

    return intf->configure(obj, config);
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
static inline int uart_write(const object* obj, const void* tx_buf, int tx_len)
{
    uart_intf_t* intf;

    if (!obj)
        return -EINVAL;

    intf = (uart_intf_t*)obj->object_intf;
    if ((intf == NULL)
        || (intf->write == NULL))
        return -ENOSUPPORT;

    return intf->write(obj, tx_buf, tx_len);
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
static inline int uart_read(const object* obj, void* rx_buf, int rx_len)
{
    uart_intf_t* intf;

    if (!obj)
        return -EINVAL;

    intf = (uart_intf_t*)obj->object_intf;
    if ((intf == NULL)
        || (intf->read == NULL))
        return -ENOSUPPORT;

    return intf->read(obj, rx_buf, rx_len);
}

#endif /* __UART_H__ */
