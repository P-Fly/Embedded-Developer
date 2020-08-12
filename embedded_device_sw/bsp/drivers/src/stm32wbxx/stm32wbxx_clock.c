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

#include <stdbool.h>
#include <string.h>
#include "drv_clock.h"

#include "stm32wbxx_hal.h"

#if defined(CONFIG_CLOCK_ENABLE)

/**
 * @brief   Standard on/off interface for subsys clock.
 */
typedef int (*subsys_onoff_t)(clock_subsys_t sys, unsigned int id, bool onoff);

/**
 * @brief   Enable/Disable the clock of a sub-system controlled by the GPIO.
 *
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 * @param   onoff 1 - Enable the clock
 *                0 - Disable the clock
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_gpio_onoff(clock_subsys_t sys, unsigned int id, bool onoff)
{
	switch (sys) {
	case DRV_CLK_PORTA:
		if (onoff)
			__HAL_RCC_GPIOA_CLK_ENABLE();
		else
			__HAL_RCC_GPIOA_CLK_DISABLE();
		break;
	case DRV_CLK_PORTB:
		if (onoff)
			__HAL_RCC_GPIOB_CLK_ENABLE();
		else
			__HAL_RCC_GPIOB_CLK_DISABLE();
		break;
	case DRV_CLK_PORTC:
		if (onoff)
			__HAL_RCC_GPIOC_CLK_ENABLE();
		else
			__HAL_RCC_GPIOC_CLK_DISABLE();
		break;
	case DRV_CLK_PORTD:
		if (onoff)
			__HAL_RCC_GPIOD_CLK_ENABLE();
		else
			__HAL_RCC_GPIOD_CLK_DISABLE();
		break;
	case DRV_CLK_PORTE:
		if (onoff)
			__HAL_RCC_GPIOE_CLK_ENABLE();
		else
			__HAL_RCC_GPIOE_CLK_DISABLE();
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief   Enable/Disable the clock of a sub-system controlled by the UART.
 *
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 * @param   onoff 1 - Enable the clock
 *                0 - Disable the clock
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_uart_onoff(clock_subsys_t sys, unsigned int id, bool onoff)
{
	if (onoff)
		__HAL_RCC_USART1_CLK_ENABLE();
	else
		__HAL_RCC_USART1_CLK_DISABLE();

	return 0;
}

/**
 * @brief   Enable/Disable the clock of a sub-system controlled by the CRC.
 *
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 * @param   onoff 1 - Enable the clock
 *                0 - Disable the clock
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_crc_onoff(clock_subsys_t sys, unsigned int id, bool onoff)
{
	if (onoff)
		__HAL_RCC_CRC_CLK_ENABLE();
	else
		__HAL_RCC_CRC_CLK_DISABLE();

	return 0;
}

/**
 * @brief   Clock sub-system definition.
 */
typedef struct {
	clock_subsys_t sys;
	unsigned int   id;
	subsys_onoff_t subsys_onoff;
} stm32wbxx_clock_subsys_onoff_t;

static const stm32wbxx_clock_subsys_onoff_t clock_subsys_onoff[] =
{
	{ DRV_CLK_PORTA, 0,  stm32wbxx_clock_gpio_onoff },
	{ DRV_CLK_PORTB, 0,  stm32wbxx_clock_gpio_onoff },
	{ DRV_CLK_PORTC, 0,  stm32wbxx_clock_gpio_onoff },
	{ DRV_CLK_PORTD, 0,  stm32wbxx_clock_gpio_onoff },
	{ DRV_CLK_PORTE, 0,  stm32wbxx_clock_gpio_onoff },
	{ DRV_CLK_UART,	 0,  stm32wbxx_clock_uart_onoff },
	{ DRV_CLK_CRC,	 0,  stm32wbxx_clock_crc_onoff  },
};

#define DEF_CLOCK_SUBSYS_ONOFF_NUM (sizeof(clock_subsys_onoff) / \
				    sizeof(clock_subsys_onoff[0]))

static int stm32wbxx_clock_subsys_onoff_search(
	clock_subsys_t sys,
	unsigned int id,
	subsys_onoff_t *subsys_onoff)
{
	int i;

	for (i = 0; i < DEF_CLOCK_SUBSYS_ONOFF_NUM; i++)
		if (clock_subsys_onoff[i].sys == sys &&
		    clock_subsys_onoff[i].id == id) {
			*subsys_onoff = clock_subsys_onoff[i].subsys_onoff;
			return i;
		}

	return -ENODEV;
}

/**
 * @brief   Clock handle definition.
 */
typedef struct {
	/*
	 * Each clock of the sub-system maintains its
	 * own status in the nesting variable.
	 */
	unsigned int clock_subsys_onoff_nesting[DEF_CLOCK_SUBSYS_ONOFF_NUM];
} stm32wbxx_clock_handle_t;

/**
 * @brief   Enable the clock of a sub-system controlled by the device.
 *
 * @param   obj Pointer to the clock object handle.
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_on(const object *	obj,
			     clock_subsys_t	sys,
			     unsigned int	id)
{
	stm32wbxx_clock_handle_t *handle =
		(stm32wbxx_clock_handle_t *)obj->object_data;
	subsys_onoff_t subsys_onoff;
	int index;
	int ret;

	if (!handle)
		return -EINVAL;

	index = stm32wbxx_clock_subsys_onoff_search(sys, id, &subsys_onoff);
	if (index < 0)
		return index;

	if (handle->clock_subsys_onoff_nesting[index] + 1 == 0)
		return -EBUSY;

	if (handle->clock_subsys_onoff_nesting[index] == 0) {
		ret = subsys_onoff(sys, id, 1);
		if (ret)
			return -EIO;
	}

	handle->clock_subsys_onoff_nesting[index]++;

	return 0;
}

/**
 * @brief   Disable the clock of a sub-system controlled by the device.
 *
 * @param   obj Pointer to the clcok object handle.
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_off(const object *	obj,
			      clock_subsys_t	sys,
			      unsigned int	id)
{
	stm32wbxx_clock_handle_t *handle =
		(stm32wbxx_clock_handle_t *)obj->object_data;
	subsys_onoff_t subsys_onoff;
	int index;
	int ret;

	if (!handle)
		return -EINVAL;

	index = stm32wbxx_clock_subsys_onoff_search(sys, id, &subsys_onoff);
	if (index < 0)
		return index;

	if (handle->clock_subsys_onoff_nesting[index] == 0)
		return -EIO;

	handle->clock_subsys_onoff_nesting[index]--;

	if (handle->clock_subsys_onoff_nesting[index] == 0) {
		ret = subsys_onoff(sys, id, 0);
		if (ret)
			return -EIO;
	}

	return 0;
}

/**
 * @brief   Set the clock rate of given sub-system.
 *
 * @param   obj Pointer to the clock object handle.
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 * @param   rate Subsystem clock rate.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_set_rate(const object *obj,
				   clock_subsys_t	sys,
				   unsigned int		id,
				   unsigned long	rate)
{
	stm32wbxx_clock_handle_t *handle =
		(stm32wbxx_clock_handle_t *)obj->object_data;

	if (!handle)
		return -EINVAL;

	return 0;
}

/**
 * @brief   Get the clock rate of given sub-system.
 *
 * @param   obj Pointer to the clock object handle.
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 * @param   rate Pointer to the subsystem clock rate.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_get_rate(const object *obj,
				   clock_subsys_t	sys,
				   unsigned int		id,
				   unsigned long *	rate)
{
	stm32wbxx_clock_handle_t *handle =
		(stm32wbxx_clock_handle_t *)obj->object_data;

	if (!handle)
		return -EINVAL;

	if (!rate)
		return -EINVAL;

	return 0;
}

static clock_intf_t clock_intf =
{
	.on			= stm32wbxx_clock_on,
	.off		= stm32wbxx_clock_off,
	.set_rate	= stm32wbxx_clock_set_rate,
	.get_rate	= stm32wbxx_clock_get_rate,
};

static stm32wbxx_clock_handle_t clock_handle;

/**
 * @brief   Probe the clock driver.
 *
 * @param   obj Pointer to the clock object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_probe(const object *obj)
{
	stm32wbxx_clock_handle_t *handle =
		(stm32wbxx_clock_handle_t *)obj->object_data;

	(void)memset(handle, 0, sizeof(stm32wbxx_clock_handle_t));

	return 0;
}

/**
 * @brief   Remove the clock driver.
 *
 * @param   obj Pointer to the clock object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_clock_shutdown(const object *obj)
{
	stm32wbxx_clock_handle_t *handle =
		(stm32wbxx_clock_handle_t *)obj->object_data;

	(void)handle;

	return 0;
}

module_core(CONFIG_CLOCK_NAME,
	    CONFIG_CLOCK_LABEL,
	    stm32wbxx_clock_probe,
	    stm32wbxx_clock_shutdown,
	    &clock_intf, &clock_handle, NULL);

#endif
