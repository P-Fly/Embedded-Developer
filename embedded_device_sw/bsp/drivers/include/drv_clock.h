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

#ifndef __DRV_CLOCK_H__
#define __DRV_CLOCK_H__

#include <stddef.h>
#include "object.h"
#include "err.h"
#include "bsp_conf.h"

/**
 * @brief   A type to identify a clock controller sub-system.
 */
typedef enum {
	DRV_CLK_MASTER,
	DRV_CLK_PORTA,
	DRV_CLK_PORTB,
	DRV_CLK_PORTC,
	DRV_CLK_PORTD,
	DRV_CLK_PORTE,
	DRV_CLK_PORTF,
	DRV_CLK_PORTG,
	DRV_CLK_PORTH,
	DRV_CLK_PORTI,
	DRV_CLK_PORTJ,
	DRV_CLK_UART,
	DRV_CLK_ADC,
	DRV_CLK_I2C,
	DRV_CLK_SPI,
	DRV_CLK_PWM,
	DRV_CLK_CAN,
	DRV_CLK_I2S,
	DRV_CLK_USB,

	DRV_CLK_MAX_NUM,
} clock_subsys_t;

/**
 * @brief   Clock interface definitions.
 */
typedef struct {
	int (*on)(const object *obj, clock_subsys_t sys, unsigned int id);
	int (*off)(const object *obj, clock_subsys_t sys, unsigned int id);
	int (*set_rate)(const object *obj, clock_subsys_t sys, unsigned int id,
			unsigned long rate);
	int (*get_rate)(const object *obj, clock_subsys_t sys, unsigned int id,
			unsigned long *rate);
} clock_intf_t;

/**
 * @brief   Enable the clock of a sub-system controlled by the device.
 *
 * @param   obj Pointer to the clock object handle.
 * @param   sys Pointer to an clock data representing the sub-system.
 * @param   id Device id.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static inline int clock_on(const object *	obj,
			   clock_subsys_t	sys,
			   unsigned int		id)
{
	clock_intf_t *intf;

	if (!obj)
		return -EINVAL;

	intf = (clock_intf_t *)obj->object_intf;
	if ((intf == NULL)
	    || (intf->on == NULL))
		return -ENOSUPPORT;

	return intf->on(obj, sys, id);
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
static inline int clock_off(const object *	obj,
			    clock_subsys_t	sys,
			    unsigned int	id)
{
	clock_intf_t *intf;

	if (!obj)
		return -EINVAL;

	intf = (clock_intf_t *)obj->object_intf;
	if ((intf == NULL)
	    || (intf->off == NULL))
		return -ENOSUPPORT;

	return intf->off(obj, sys, id);
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
static inline int clock_set_rate(const object * obj,
				 clock_subsys_t sys,
				 unsigned int	id,
				 unsigned long	rate)
{
	clock_intf_t *intf;

	if (!obj)
		return -EINVAL;

	intf = (clock_intf_t *)obj->object_intf;
	if ((intf == NULL)
	    || (intf->set_rate == NULL))
		return -ENOSUPPORT;

	return intf->set_rate(obj, sys, id, rate);
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
static inline int clock_get_rate(const object * obj,
				 clock_subsys_t sys,
				 unsigned int	id,
				 unsigned long *rate)
{
	clock_intf_t *intf;

	if (!obj)
		return -EINVAL;

	intf = (clock_intf_t *)obj->object_intf;
	if ((intf == NULL)
	    || (intf->get_rate == NULL))
		return -ENOSUPPORT;

	return intf->get_rate(obj, sys, id, rate);
}

#endif /* __DRV_CLOCK_H__ */
