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
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "drv_gpio.h"
#include "drv_clock.h"
#include "log.h"

#include "stm32wbxx_hal.h"

#if defined(CONFIG_GPIO_ENABLE)

/**
 * @brief   GPIO handle definition.
 */
typedef struct {
	const object * clock;
	clock_subsys_t sys;
	GPIO_TypeDef * port;
} stm32wbxx_gpio_handle_t;

/**
 * @brief   GPIO pin internal ID definitions.
 */
typedef struct {
	gpio_pin_id_t	pin;
	unsigned short  pad_id;
} stm32wbxx_gpio_internal_id_t;

static const stm32wbxx_gpio_internal_id_t gpio_internal_id[] =
{
	{ DRV_GPIO_PIN_0,  GPIO_PIN_0 },
	{ DRV_GPIO_PIN_1,  GPIO_PIN_1 },
	{ DRV_GPIO_PIN_2,  GPIO_PIN_2 },
	{ DRV_GPIO_PIN_3,  GPIO_PIN_3 },
	{ DRV_GPIO_PIN_4,  GPIO_PIN_4 },
	{ DRV_GPIO_PIN_5,  GPIO_PIN_5 },
	{ DRV_GPIO_PIN_6,  GPIO_PIN_6 },
	{ DRV_GPIO_PIN_7,  GPIO_PIN_7 },
	{ DRV_GPIO_PIN_8,  GPIO_PIN_8 },
	{ DRV_GPIO_PIN_9,  GPIO_PIN_9 },
	{ DRV_GPIO_PIN_10, GPIO_PIN_10 },
	{ DRV_GPIO_PIN_11, GPIO_PIN_11 },
	{ DRV_GPIO_PIN_12, GPIO_PIN_12 },
	{ DRV_GPIO_PIN_13, GPIO_PIN_13 },
	{ DRV_GPIO_PIN_14, GPIO_PIN_14 },
	{ DRV_GPIO_PIN_15, GPIO_PIN_15 },
};

static int stm32wbxx_gpio_pad_id_search(gpio_pin_id_t pin, unsigned short *pad_id)
{
	int i;

	for (i = 0;
	     i < sizeof(gpio_internal_id) / sizeof(gpio_internal_id[0]);
	     i++)
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
static int stm32wbxx_gpio_configure(const object *	obj,
				   gpio_pin_id_t	pin,
				   const gpio_config_t *config)
{
	stm32wbxx_gpio_handle_t *handle =
		(stm32wbxx_gpio_handle_t *)obj->object_data;
	GPIO_InitTypeDef gpio_config;
	unsigned short pad_id;
	int ret;

	if (!handle)
		return -EINVAL;

	if (!config)
		return -EINVAL;

	ret = stm32wbxx_gpio_pad_id_search(pin, &pad_id);
	if (ret < 0)
		return ret;

	gpio_config.Pin = pad_id;

	switch (GPIO_CONFIG_MODE_MASK(config->configs)) {
	case GPIO_CONFIG_MODE_INPUT:
		gpio_config.Mode = GPIO_MODE_INPUT;
		break;
	case GPIO_CONFIG_MODE_OUTPUT_PP:
		gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
		break;
	case GPIO_CONFIG_MODE_OUTPUT_OD:
		gpio_config.Mode = GPIO_MODE_OUTPUT_OD;
		break;
	case GPIO_CONFIG_MODE_ANALOG:
		gpio_config.Mode = GPIO_MODE_ANALOG;
		break;
	default:
		return -EINVAL;
	}

	switch (GPIO_CONFIG_PULL_MASK(config->configs)) {
	case GPIO_CONFIG_NOPULL:
		gpio_config.Pull = GPIO_NOPULL;
		break;
	case GPIO_CONFIG_PULLUP:
		gpio_config.Pull = GPIO_PULLUP;
		break;
	case GPIO_CONFIG_PULLDOWN:
		gpio_config.Pull = GPIO_PULLDOWN;
		break;
	default:
		return -EINVAL;
	}

	switch (GPIO_CONFIG_SPEED_MASK(config->configs)) {
	case GPIO_CONFIG_SPEED_FREQ_LOW:
		gpio_config.Speed = GPIO_SPEED_FREQ_LOW;
		break;
	case GPIO_CONFIG_SPEED_FREQ_MEDIUM:
		gpio_config.Speed = GPIO_SPEED_FREQ_MEDIUM;
		break;
	case GPIO_CONFIG_SPEED_FREQ_HIGH:
		gpio_config.Speed = GPIO_SPEED_FREQ_HIGH;
		break;
	case GPIO_CONFIG_SPEED_FREQ_VERY_HIGH:
		gpio_config.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		break;
	default:
		return -EINVAL;
	}

	HAL_GPIO_Init(handle->port, &gpio_config);

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
static int stm32wbxx_gpio_write(const object *	obj,
			       gpio_pin_id_t	pin,
			       gpio_pin_level_t level)
{
	stm32wbxx_gpio_handle_t *handle =
		(stm32wbxx_gpio_handle_t *)obj->object_data;
	unsigned short pad_id;
	GPIO_PinState value;
	int ret;

	if (!handle)
		return -EINVAL;

	ret = stm32wbxx_gpio_pad_id_search(pin, &pad_id);
	if (ret < 0)
		return ret;

	if (level == DRV_GPIO_PIN_SET)
		value = GPIO_PIN_SET;
	else
		value = GPIO_PIN_RESET;

	HAL_GPIO_WritePin(handle->port, pad_id, value);

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
static int stm32wbxx_gpio_toggle(const object *obj, gpio_pin_id_t pin)
{
	stm32wbxx_gpio_handle_t *handle =
		(stm32wbxx_gpio_handle_t *)obj->object_data;
	unsigned short pad_id;
	int ret;

	if (!handle)
		return -EINVAL;

	ret = stm32wbxx_gpio_pad_id_search(pin, &pad_id);
	if (ret < 0)
		return ret;

	HAL_GPIO_TogglePin(handle->port, pad_id);

	return 0;
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
static int stm32wbxx_gpio_read(const object *	obj,
			      gpio_pin_id_t	pin,
			      gpio_pin_level_t *level)
{
	stm32wbxx_gpio_handle_t *handle =
		(stm32wbxx_gpio_handle_t *)obj->object_data;
	unsigned short pad_id;
	GPIO_PinState value;
	int ret;

	if (!handle)
		return -EINVAL;

	if (!level)
		return -EINVAL;

	ret = stm32wbxx_gpio_pad_id_search(pin, &pad_id);
	if (ret < 0)
		return ret;

	value = HAL_GPIO_ReadPin(handle->port, pad_id);

	*level = value == GPIO_PIN_SET ? DRV_GPIO_PIN_SET : DRV_GPIO_PIN_CLR;

	return 0;
}

static gpio_intf_t gpio_intf =
{
	.configure	= stm32wbxx_gpio_configure,
	.write		= stm32wbxx_gpio_write,
	.toggle		= stm32wbxx_gpio_toggle,
	.read		= stm32wbxx_gpio_read,
};

/**
 * @brief   Probe the gpio driver.
 *
 * @param   obj Pointer to the gpio object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_gpio_probe(const object *obj)
{
	stm32wbxx_gpio_handle_t *handle =
		(stm32wbxx_gpio_handle_t *)obj->object_data;
	int ret;

	handle->clock = object_get_binding(CONFIG_CLOCK_NAME);
	if (!handle->clock) {
		pr_error("Drivce <%s> binding object <%s> failed.",
			 obj->name,
			 CONFIG_CLOCK_NAME);
		return -ENODEV;
	} else {
		pr_info("Drivce <%s> binding object <%s> succeed.",
			obj->name,
			CONFIG_CLOCK_NAME);
	}

	ret = clock_on(handle->clock, handle->sys, 0);
	if (ret)
		return ret;

	pr_info("Object <%s> probe succeed.", obj->name);

	return 0;
}

/**
 * @brief   Remove the gpio driver.
 *
 * @param   obj Pointer to the gpio object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_gpio_shutdown(const object *obj)
{
	stm32wbxx_gpio_handle_t *handle =
		(stm32wbxx_gpio_handle_t *)obj->object_data;
	int ret;

	ret = clock_off(handle->clock, handle->sys, 0);
	if (ret)
		return ret;

	pr_info("Object <%s> shutdown succeed.", obj->name);

	return 0;
}
#endif

#ifdef CONFIG_GPIOA_NAME
static stm32wbxx_gpio_handle_t gpioa_handle =
{
	.clock	= NULL,
	.sys	= DRV_CLK_PORTA,
	.port	= GPIOA,
};

module_driver(CONFIG_GPIOA_NAME,
	      CONFIG_GPIOA_LABEL,
	      stm32wbxx_gpio_probe,
	      stm32wbxx_gpio_shutdown,
	      &gpio_intf, &gpioa_handle, NULL);
#endif

#ifdef CONFIG_GPIOB_NAME
static stm32wbxx_gpio_handle_t gpiob_handle =
{
	.clock	= NULL,
	.sys	= DRV_CLK_PORTB,
	.port	= GPIOB,
};

module_driver(CONFIG_GPIOB_NAME,
	      CONFIG_GPIOB_LABEL,
	      stm32wbxx_gpio_probe,
	      stm32wbxx_gpio_shutdown,
	      &gpio_intf, &gpiob_handle, NULL);
#endif

#ifdef CONFIG_GPIOC_NAME
static stm32wbxx_gpio_handle_t gpioc_handle =
{
	.clock	= NULL,
	.sys	= DRV_CLK_PORTC,
	.port	= GPIOC,
};

module_driver(CONFIG_GPIOC_NAME,
	      CONFIG_GPIOC_LABEL,
	      stm32wbxx_gpio_probe,
	      stm32wbxx_gpio_shutdown,
	      &gpio_intf, &gpioc_handle, NULL);
#endif

#ifdef CONFIG_GPIOD_NAME
static stm32wbxx_gpio_handle_t gpiod_handle =
{
	.clock	= NULL,
	.sys	= DRV_CLK_PORTD,
	.port	= GPIOD,
};

module_driver(CONFIG_GPIOD_NAME,
	      CONFIG_GPIOD_LABEL,
	      stm32wbxx_gpio_probe,
	      stm32wbxx_gpio_shutdown,
	      &gpio_intf, &gpiod_handle, NULL);
#endif

#ifdef CONFIG_GPIOE_NAME
static stm32wbxx_gpio_handle_t gpioe_handle =
{
	.clock	= NULL,
	.sys	= DRV_CLK_PORTE,
	.port	= GPIOE,
};

module_driver(CONFIG_GPIOE_NAME,
	      CONFIG_GPIOE_LABEL,
	      stm32wbxx_gpio_probe,
	      stm32wbxx_gpio_shutdown,
	      &gpio_intf, &gpioe_handle, NULL);
#endif
