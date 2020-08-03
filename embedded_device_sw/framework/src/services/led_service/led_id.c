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

#include <stddef.h>
#include "service.h"
#include "led_id.h"
#include "log.h"

#if defined(CONFIG_LED_SERVICE_ENABLE)

static const led_id_t led_id[] = CONFIG_LED_ID_CONFIGS;

/**
 * @brief   Search for the led_id by index.
 *
 * @param   index The index number.
 *
 * @retval  led_id handle for reference or NULL in case of error.
 */
const led_id_t* led_id_search_by_index(unsigned int index)
{
	if (index >= sizeof(led_id) / sizeof(led_id[0]))
		return NULL;

	return &led_id[index];
}

#endif
