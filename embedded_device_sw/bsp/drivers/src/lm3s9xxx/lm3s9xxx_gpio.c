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
#include "drv_gpio.h"
#include "drv_clock.h"

#define TARGET_IS_TEMPEST_RC1
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#if defined(CONFIG_GPIO_ENABLE)

/**
 * @brief   GPIO handle definition.
 */
typedef struct {
	const object *clock;
	clock_subsys_t sys;
	unsigned long port;
} lm3s9xxx_gpio_handle_t;

/**
 * @brief   GPIO pin internal ID definitions.
 */
typedef struct
{
	gpio_pin_id_t pin;
	unsigned char pad_id;
} lm3s9xxx_gpio_internal_id_t;

static const lm3s9xxx_gpio_internal_id_t gpio_internal_id[] =
{
	{ 0, GPIO_PIN_0 },
	{ 1, GPIO_PIN_1 },
	{ 2, GPIO_PIN_2 },
	{ 3, GPIO_PIN_3 },
	{ 4, GPIO_PIN_4 },
	{ 5, GPIO_PIN_5 },
	{ 6, GPIO_PIN_6 },
	{ 7, GPIO_PIN_7 },
};

static int lm3s9xxx_gpio_pad_id_search(gpio_pin_id_t pin, unsigned char *pad_id)
{
    int i;

    for (i = 0; i < sizeof(gpio_internal_id) / sizeof(gpio_internal_id[0]); i++)
        if (gpio_internal_id[i].pin == pin) {
			*pad_id = gpio_internal_id[i].pad_id;
            return i;
        }

    return -ENODEV;
}

/**
 * @brief   Install one hardware configuration from a configuration space.
 *
 * @param   obj Pointer to the GPIO object handle.
 * @param   pin Pin number where data is config.
 * @param   config Pointer to the configuration space.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_gpio_configure(const object *obj, gpio_pin_id_t pin, const gpio_config_t *config)
{
	lm3s9xxx_gpio_handle_t *handle =
			(lm3s9xxx_gpio_handle_t *)obj->object_data;
	unsigned long pad_type = 0;
	unsigned long pad_strength;
	unsigned long pad_dir;
	unsigned char pad_id;
	int ret;

	if (!handle)
		return -EINVAL;

	if (!config)
		return -EINVAL;

	ret = lm3s9xxx_gpio_pad_id_search(pin, &pad_id);
	if (ret < 0)
		return ret;

	switch (GPIO_CONFIG_MODE_MASK(config->configs)) {
		case GPIO_CONFIG_MODE_INPUT:
			pad_dir = GPIO_DIR_MODE_IN;
			pad_type |= 0x08;
			break;
		case GPIO_CONFIG_MODE_OUTPUT_PP:
			pad_dir = GPIO_DIR_MODE_OUT;
			pad_type |= 0x08;
			break;
		case GPIO_CONFIG_MODE_OUTPUT_OD:
			pad_dir = GPIO_DIR_MODE_OUT;
			pad_type |= 0x09;
			break;
		case GPIO_CONFIG_MODE_ANALOG:
			pad_dir = GPIO_DIR_MODE_IN;
			pad_type = GPIO_PIN_TYPE_ANALOG;
			break;
		default:
			return -EINVAL;
		}

	switch (GPIO_CONFIG_PULL_MASK(config->configs)) {
		case GPIO_CONFIG_PULLUP:
			pad_type |= 0x02;
			break;
		case GPIO_CONFIG_PULLDOWN:
			pad_type |= 0x04;
			break;
		default:
			return -EINVAL;
		}

	switch (GPIO_CONFIG_SPEED_MASK(config->configs)) {
		case GPIO_CONFIG_SPEED_FREQ_LOW:
			pad_strength = GPIO_STRENGTH_2MA;
			break;
		case GPIO_CONFIG_SPEED_FREQ_MEDIUM:
			pad_strength = GPIO_STRENGTH_4MA;
			break;
		case GPIO_CONFIG_SPEED_FREQ_HIGH:
			pad_strength = GPIO_STRENGTH_8MA;
			break;
		case GPIO_CONFIG_SPEED_FREQ_VERY_HIGH:
			pad_strength = GPIO_STRENGTH_8MA_SC;
			break;
		default:
			return -EINVAL;
		}

	MAP_GPIODirModeSet(handle->port, pad_id, pad_dir);

	MAP_GPIOPadConfigSet(handle->port, pad_id, pad_strength, pad_type);

	return 0;
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
static int lm3s9xxx_gpio_write(const object *obj, gpio_pin_id_t pin, gpio_pin_level_t level)
{
	lm3s9xxx_gpio_handle_t *handle =
			(lm3s9xxx_gpio_handle_t *)obj->object_data;
	unsigned char pad_id;
	int ret;

	if (!handle)
		return -EINVAL;

	ret = lm3s9xxx_gpio_pad_id_search(pin, &pad_id);
	if (ret < 0)
		return ret;

	return 0;
}

/**
 * @brief   Toggle data to pin.
 *
 * @param   obj Pointer to the GPIO object handle.
 * @param   pin Pin number where data is toggle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_gpio_toggle(const object *obj, gpio_pin_id_t pin)
{
	lm3s9xxx_gpio_handle_t *handle =
			(lm3s9xxx_gpio_handle_t *)obj->object_data;
	unsigned char pad_id;
	int ret;

	if (!handle)
		return -EINVAL;

	ret = lm3s9xxx_gpio_pad_id_search(pin, &pad_id);
	if (ret < 0)
		return ret;
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
static int lm3s9xxx_gpio_read(const object *obj, gpio_pin_id_t pin, gpio_pin_level_t *level)
{
	lm3s9xxx_gpio_handle_t *handle =
			(lm3s9xxx_gpio_handle_t *)obj->object_data;
	unsigned char pad_id;
	int ret;

	if (!handle)
		return -EINVAL;

	if (!level)
		return -EINVAL;

	ret = lm3s9xxx_gpio_pad_id_search(pin, &pad_id);
	if (ret < 0)
		return ret;

	*level = MAP_GPIOPinRead(handle->port, pad_id);

	return 0;
}

static gpio_intf_t gpio_intf =
{
	.configure = lm3s9xxx_gpio_read,
	.write = lm3s9xxx_gpio_write,
	.toggle = lm3s9xxx_gpio_toggle,
	.read = lm3s9xxx_gpio_read,
};

/**
 * @brief   Probe the gpio driver.
 *
 * @param   obj Pointer to the gpio object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_gpio_probe(const object *obj)
{
	lm3s9xxx_gpio_handle_t *handle =
		(lm3s9xxx_gpio_handle_t *)obj->object_data;
	int ret;

	handle->clock = object_get_binding(CONFIG_CLOCK_NAME);
	if (!obj)
		return -ENODEV;

	ret = clock_on(handle->clock, handle->sys, 0);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief   Remove the gpio driver.
 *
 * @param   obj Pointer to the gpio object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_gpio_shutdown(const object *obj)
{
	lm3s9xxx_gpio_handle_t *handle =
			(lm3s9xxx_gpio_handle_t *)obj->object_data;
	int ret;

	ret = clock_off(handle->clock, handle->sys, 0);
	if (ret)
		return ret;

	return 0;
}
#endif

#ifdef CONFIG_GPIOF_NAME
static lm3s9xxx_gpio_handle_t gpiof_handle =
{
	.clock = NULL;
	.sys = DRV_CLK_PORTF;
	.port = GPIO_PORTF_BASE;
};

module_driver(CONFIG_GPIOF_NAME,
		    CONFIG_GPIOF_LABEL,
		    lm3s9xxx_gpio_probe,
		    lm3s9xxx_gpio_shutdown,
		    &gpio_intf, &gpiof_handle, NULL);
#endif
