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

#ifndef __LED_HARDWARE_H__
#define __LED_HARDWARE_H__

#include "drv_gpio.h"

/**
 * @brief   Led hardware definitions.
 */
typedef struct {
	const char *const	port;
	gpio_pin_id_t		pin;
	gpio_config_t		config;
	gpio_pin_level_t	on;
	gpio_pin_level_t	off;
} led_hardware_t;

extern const led_hardware_t *led_hardware_search_by_index(unsigned int index);

#endif /* __LED_HARDWARE_H__ */
