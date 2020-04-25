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

#ifndef __CRC_H__
#define __CRC_H__

#include <stddef.h>
#include "object.h"
#include "err.h"
#include "bsp_conf.h"

/* Initial Value */
#define CRC_CONFIG_INITIAL_VALUE_DEFAULT            (0xFFFFFFFF)

/* Polynomial */
#define CRC_CONFIG_POLYNOMIAL_DEFAULT               (0)

/* Polynomial Length */
#define CRC_CONFIG_POLYNOMIAL_LENGTH_32B            (0 << 0)
#define CRC_CONFIG_POLYNOMIAL_LENGTH_16B            (1 << 0)
#define CRC_CONFIG_POLYNOMIAL_LENGTH_8B             (2 << 0)
#define CRC_CONFIG_POLYNOMIAL_LENGTH_7B             (3 << 0)
#define CRC_CONFIG_POLYNOMIAL_LENGTH_MASK(config)   ((config) & (0x03 << 0))

/* Input Format */
#define CRC_CONFIG_INPUT_32B                        (0 << 2)
#define CRC_CONFIG_INPUT_16B                        (1 << 2)
#define CRC_CONFIG_INPUT_8B                         (2 << 2)
#define CRC_CONFIG_INPUT_MASK(config)               ((config) & (0x03 << 2))

/**
 * @brief   CRC configuration structure.
 *
 * configs is a bit field with the following parts:
 *      Polynomial Length   [ 0 : 1 ]
 *      Input Format        [ 2 : 3 ]
 *      RESERVED            [ 4 : 31 ] - Undefined or device-specific usage.
 */
typedef struct
{
    unsigned int initial_value;
    unsigned int polynomial;
    unsigned int configs;
} crc_config_t;

/**
 * @brief   CRC interface definitions.
 */
typedef struct
{
    int (*configure)(const object* obj, const crc_config_t* config);
    int (*calculate)(const object* obj,
        const void* buf, int len, unsigned int* crc);
    int (*accumulate)(const object* obj,
        const void* buf, int len, unsigned int* crc);
} crc_intf_t;

/**
 * @brief   Install one hardware configuration from a configuration space.
 *
 * @param   obj Pointer to the CRC object handle.
 * @param   config Pointer to the configuration space.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static inline int crc_configure(const object* obj, const crc_config_t* config)
{
    crc_intf_t* intf;

    if (!obj)
        return -EINVAL;

    intf = (crc_intf_t*)obj->object_intf;
    if ((intf == NULL)
        || (intf->configure == NULL))
        return -ENOSUPPORT;

    return intf->configure(obj, config);
}

/**
 * @brief   Compute the CRC value of an data buffer.
 *
 * @param   obj Pointer to the CRC object handle.
 * @param   buf Pointer to data buffer to compute.
 * @param   len Data length to compute.
 * @param   crc CRC (returned value LSBs for CRC shorter than 32 bits).
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int crc_calculate(const object* obj,
    const void* buf, int len, unsigned int* crc)
{
    crc_intf_t* intf;

    if (!obj)
        return -EINVAL;

    intf = (crc_intf_t*)obj->object_intf;
    if ((intf == NULL)
        || (intf->calculate == NULL))
        return -ENOSUPPORT;

    return intf->calculate(obj, buf, len, crc);
}

/**
 * @brief   Compute the CRC value of an data buffer, 
 *          starting with the previously computed CRC as initialization value.
 *
 * @param   obj Pointer to the CRC object handle.
 * @param   buf Pointer to data buffer to compute.
 * @param   len Data length to compute.
 * @param   crc CRC (returned value LSBs for CRC shorter than 32 bits).
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int crc_accumulate(const object* obj,
    const void* buf, int len, unsigned int* crc)
{
    crc_intf_t* intf;

    if (!obj)
        return -EINVAL;

    intf = (crc_intf_t*)obj->object_intf;
    if ((intf == NULL)
        || (intf->accumulate == NULL))
        return -ENOSUPPORT;

    return intf->accumulate(obj, buf, len, crc);
}

#endif /* __CRC_H__ */
