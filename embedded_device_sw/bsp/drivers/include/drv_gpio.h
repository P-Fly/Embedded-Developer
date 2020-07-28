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

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include <stddef.h>
#include "object.h"
#include "err.h"
#include "bsp_conf.h"

/**
 * @brief   GPIO pin level definitions.
 */
typedef enum
{
	DRV_GPIO_PIN_CLR,
	DRV_GPIO_PIN_SET,
} gpio_pin_level_t;

/**
 * @brief   GPIO pin ID definitions.
 */
typedef enum
{
	DRV_GPIO_PIN_0,
	DRV_GPIO_PIN_1,
	DRV_GPIO_PIN_2,
	DRV_GPIO_PIN_3,
	DRV_GPIO_PIN_4,
	DRV_GPIO_PIN_5,
	DRV_GPIO_PIN_6,
	DRV_GPIO_PIN_7,
	DRV_GPIO_PIN_8,
	DRV_GPIO_PIN_9,
	DRV_GPIO_PIN_10,
	DRV_GPIO_PIN_11,
	DRV_GPIO_PIN_12,
	DRV_GPIO_PIN_13,
	DRV_GPIO_PIN_14,
	DRV_GPIO_PIN_15,

	DRV_GPIO_PIN_MAX_NUM,
} gpio_pin_id_t;

/* GPIO Mode */
#define GPIO_CONFIG_MODE_INPUT				(0 << 0)
#define GPIO_CONFIG_MODE_OUTPUT_PP			(1 << 0)
#define GPIO_CONFIG_MODE_OUTPUT_OD			(2 << 0)
#define GPIO_CONFIG_MODE_AF_PP				(3 << 0)
#define GPIO_CONFIG_MODE_AF_OD				(4 << 0)
#define GPIO_CONFIG_MODE_ANALOG				(5 << 0)
#define GPIO_CONFIG_MODE_MASK(config)		((config) & (0x07 << 0))

/* GPIO Speed */
#define GPIO_CONFIG_SPEED_FREQ_LOW        	(0 << 3)
#define GPIO_CONFIG_SPEED_FREQ_MEDIUM     	(1 << 3)
#define GPIO_CONFIG_SPEED_FREQ_HIGH       	(2 << 3)
#define GPIO_CONFIG_SPEED_FREQ_VERY_HIGH  	(3 << 3)
#define GPIO_CONFIG_SPEED_MASK(config)		((config) & (0x03 << 3))

/* GPIO Pull Mode */
#define GPIO_CONFIG_NOPULL					(0 << 5)
#define GPIO_CONFIG_PULLUP					(1 << 5)
#define GPIO_CONFIG_PULLDOWN				(2 << 5)
#define GPIO_CONFIG_PULL_MASK(config)		((config) & (0x03 << 5))

/**
 * @brief   GPIO configuration structure.
 *
 * configs is a bit field with the following parts:
 *      Mode            [ 0 : 2 ]
 *      Speed Mode      [ 3 : 4 ]
 *      Pull Mode       [ 5 : 6 ]
 *      RESERVED        [ 7 : 31 ] - Undefined or device-specific usage.
 */
typedef struct {
	unsigned int	configs;
} gpio_config_t;

/**
 * @brief   GPIO interface definitions.
 */
typedef struct {
	int (*configure)(const object *obj, gpio_pin_id_t pin, const gpio_config_t *config);
	int (*write)(const object *obj, gpio_pin_id_t pin, gpio_pin_level_t level);
	int (*toggle)(const object *obj, gpio_pin_id_t pin);
	int (*read)(const object *obj, gpio_pin_id_t pin, gpio_pin_level_t *level);
} gpio_intf_t;

/**
 * @brief   Install one hardware configuration from a configuration space.
 *
 * @param   obj Pointer to the GPIO object handle.
 * @param   pin Pin number where data is config.
 * @param   config Pointer to the configuration space.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static inline int gpio_configure(const object *obj, gpio_pin_id_t pin, const gpio_config_t *config)
{
	gpio_intf_t *intf;

	if (!obj)
		return -EINVAL;

	intf = (gpio_intf_t *)obj->object_intf;
	if ((intf == NULL)
	    || (intf->configure == NULL))
		return -ENOSUPPORT;

	return intf->configure(obj, pin, config);
}

/**
 * @brief   Write data to pin.
 *
 * @param   obj Pointer to the GPIO object handle.
 * @param   pin Pin number where data is write.
 * @param   level Value set on the pin.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static inline int gpio_write(const object *obj, gpio_pin_id_t pin, gpio_pin_level_t level)
{
	gpio_intf_t *intf;

	if (!obj)
		return -EINVAL;

	intf = (gpio_intf_t *)obj->object_intf;
	if ((intf == NULL)
	    || (intf->write == NULL))
		return -ENOSUPPORT;

	return intf->write(obj, pin, level);
}

/**
 * @brief   Toggle data to pin.
 *
 * @param   obj Pointer to the GPIO object handle.
 * @param   pin Pin number where data is toggle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static inline int gpio_toggle(const object *obj, gpio_pin_id_t pin)
{
	gpio_intf_t *intf;

	if (!obj)
		return -EINVAL;

	intf = (gpio_intf_t *)obj->object_intf;
	if ((intf == NULL)
	    || (intf->toggle == NULL))
		return -ENOSUPPORT;

	return intf->toggle(obj, pin);
}

/**
 * @brief   Read data to pin.
 *
 * @param   obj Pointer to the GPIO object handle.
 * @param   pin Pin number where data is read.
 * @param   level Value get on the pin.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static inline int gpio_read(const object *obj, gpio_pin_id_t pin, gpio_pin_level_t *level)
{
	gpio_intf_t *intf;

	if (!obj)
		return -EINVAL;

	intf = (gpio_intf_t *)obj->object_intf;
	if ((intf == NULL)
	    || (intf->read == NULL))
		return -ENOSUPPORT;

	return intf->read(obj, pin, level);
}

#endif /* __DRV_GPIO_H__ */
