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
#include "drv_clock.h"

#define TARGET_IS_TEMPEST_RC1
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#if defined(CONFIG_CLOCK_ENABLE)

/**
 * @brief   Clock sub-system definition.
 */
typedef struct {
	clock_subsys_t	sys;
	unsigned int	id;
	unsigned long	peripheral_base;
} lm3s9xxx_clock_subsys_onoff_t;

static const lm3s9xxx_clock_subsys_onoff_t clock_subsys_onoff[] =
{
	{ DRV_CLK_PORTA, 0,  SYSCTL_PERIPH_GPIOA },
	{ DRV_CLK_PORTB, 0,  SYSCTL_PERIPH_GPIOB },
	{ DRV_CLK_PORTC, 0,  SYSCTL_PERIPH_GPIOC },
	{ DRV_CLK_PORTD, 0,  SYSCTL_PERIPH_GPIOD },
	{ DRV_CLK_PORTE, 0,  SYSCTL_PERIPH_GPIOE },
	{ DRV_CLK_PORTF, 0,  SYSCTL_PERIPH_GPIOF },
	{ DRV_CLK_PORTG, 0,  SYSCTL_PERIPH_GPIOG },
	{ DRV_CLK_PORTH, 0,  SYSCTL_PERIPH_GPIOH },
	{ DRV_CLK_PORTJ, 0,  SYSCTL_PERIPH_GPIOJ },
	{ DRV_CLK_UART,	 0,  SYSCTL_PERIPH_UART0 },
};

#define DEF_CLOCK_SUBSYS_ONOFF_NUM (sizeof(clock_subsys_onoff) / \
				    sizeof(clock_subsys_onoff[0]))

static int lm3s9xxx_clock_subsys_onoff_search(clock_subsys_t	sys,
					      unsigned int	id,
					      unsigned long *	peripheral_base)
{
	int i;

	for (i = 0; i < DEF_CLOCK_SUBSYS_ONOFF_NUM; i++)
		if (clock_subsys_onoff[i].sys == sys &&
		    clock_subsys_onoff[i].id == id) {
			*peripheral_base =
				clock_subsys_onoff[i].peripheral_base;
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
} lm3s9xxx_clock_handle_t;

/**
 * @brief   Enable the clock of a sub-system controlled by the device.
 *
 * @param   obj Pointer to the clock object handle.
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_clock_on(const object *	obj,
			     clock_subsys_t	sys,
			     unsigned int	id)
{
	lm3s9xxx_clock_handle_t *handle =
		(lm3s9xxx_clock_handle_t *)obj->object_data;
	unsigned long peripheral_base;
	int index;

	if (!handle)
		return -EINVAL;

	index = lm3s9xxx_clock_subsys_onoff_search(sys, id, &peripheral_base);
	if (index < 0)
		return index;

	if (handle->clock_subsys_onoff_nesting[index] + 1 == 0)
		return -EBUSY;

	if (handle->clock_subsys_onoff_nesting[index] == 0)
		MAP_SysCtlPeripheralEnable(peripheral_base);

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
static int lm3s9xxx_clock_off(const object *	obj,
			      clock_subsys_t	sys,
			      unsigned int	id)
{
	lm3s9xxx_clock_handle_t *handle =
		(lm3s9xxx_clock_handle_t *)obj->object_data;
	unsigned long peripheral_base;
	int index;

	if (!handle)
		return -EINVAL;

	index = lm3s9xxx_clock_subsys_onoff_search(sys, id, &peripheral_base);
	if (index < 0)
		return index;

	if (handle->clock_subsys_onoff_nesting[index] == 0)
		return -EIO;

	handle->clock_subsys_onoff_nesting[index]--;

	if (handle->clock_subsys_onoff_nesting[index] == 0)
		MAP_SysCtlPeripheralDisable(peripheral_base);

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
static int lm3s9xxx_clock_set_rate(const object *	obj,
				   clock_subsys_t	sys,
				   unsigned int		id,
				   unsigned long	rate)
{
	lm3s9xxx_clock_handle_t *handle =
		(lm3s9xxx_clock_handle_t *)obj->object_data;

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
static int lm3s9xxx_clock_get_rate(const object *	obj,
				   clock_subsys_t	sys,
				   unsigned int		id,
				   unsigned long *	rate)
{
	lm3s9xxx_clock_handle_t *handle =
		(lm3s9xxx_clock_handle_t *)obj->object_data;

	if (!handle)
		return -EINVAL;

	if (!rate)
		return -EINVAL;

	return 0;
}

static clock_intf_t clock_intf =
{
	.on		= lm3s9xxx_clock_on,
	.off		= lm3s9xxx_clock_off,
	.set_rate	= lm3s9xxx_clock_set_rate,
	.get_rate	= lm3s9xxx_clock_get_rate,
};

static lm3s9xxx_clock_handle_t clock_handle;

/**
 * @brief   Probe the clock driver.
 *
 * @param   obj Pointer to the clock object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_clock_probe(const object *obj)
{
	lm3s9xxx_clock_handle_t *handle =
		(lm3s9xxx_clock_handle_t *)obj->object_data;

	(void)memset(handle, 0, sizeof(lm3s9xxx_clock_handle_t));

	return 0;
}

/**
 * @brief   Remove the clock driver.
 *
 * @param   obj Pointer to the clock object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int lm3s9xxx_clock_shutdown(const object *obj)
{
	lm3s9xxx_clock_handle_t *handle =
		(lm3s9xxx_clock_handle_t *)obj->object_data;

	(void)handle;

	return 0;
}

module_core(CONFIG_CLOCK_NAME,
	    CONFIG_CLOCK_LABEL,
	    lm3s9xxx_clock_probe,
	    lm3s9xxx_clock_shutdown,
	    &clock_intf, &clock_handle, NULL);

#endif
