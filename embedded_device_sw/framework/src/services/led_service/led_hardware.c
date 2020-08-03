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
#include "service.h"
#include "led_hardware.h"
#include "log.h"

#if defined(CONFIG_LED_SERVICE_ENABLE)

static const led_hardware_t led_hardware[] = CONFIG_LED_HW_CONFIGS;

/**
 * @brief   Search for the hardware by index.
 *
 * @param   index The index number.
 *
 * @retval  Hardware handle for reference or NULL in case of error.
 */
const led_hardware_t *led_hardware_search_by_index(unsigned int index)
{
	if (index >= sizeof(led_hardware) / sizeof(led_hardware[0]))
		return NULL;

	return &led_hardware[index];
}

#endif
