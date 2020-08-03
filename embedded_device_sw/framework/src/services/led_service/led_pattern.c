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
#include "led_pattern.h"
#include "log.h"

#if defined(CONFIG_LED_SERVICE_ENABLE)

/**
 *                           +---+
 *                           |   |
 *                           |   |
 * LED_PATTERN_FLASH_ONCE  +-+---+-------------------------------------->
 *                            300  300
 *
 *                           +---+     +---+
 *                           |   |     |   |
 *                           |   |     |   |
 * LED_PATTERN_FLASH_TWICE +-+---+-----+---+---------------------------->
 *                            300  300  300  300
 *
 *                           +---+     +---+     +---+
 *                           |   |     |   |     |   |
 *                           |   |     |   |     |   |
 * LED_PATTERN_QUICK_FLASH +-+---+-----+---+-----+---+------------------>
 *                            300  300  300  300  300  300  (repeat)
 *
 *                           +---+            +---+
 *                           |   |            |   |
 *                           |   |            |   |
 * LED_PATTERN_SLOW_FLASH  +-+---+------------+---+--------------------->
 *                            300     2700     300     2700     (repeat)
 *
 */

#define DEF_CYCLE_NUM_FLASH_ONCE 2
static const led_pattern_cycle_t led_pattern_cycle_flash_once[DEF_CYCLE_NUM_FLASH_ONCE] =
{
	{ .onoff = 1, .time_ms = 300 },
	{ .onoff = 0, .time_ms = 300 },
};

#define DEF_CYCLE_NUM_FLASH_TWICE 4
static const led_pattern_cycle_t led_pattern_cycle_flash_twice[DEF_CYCLE_NUM_FLASH_TWICE] =
{
	{ .onoff = 1, .time_ms = 300 },
	{ .onoff = 0, .time_ms = 300 },
	{ .onoff = 1, .time_ms = 300 },
	{ .onoff = 0, .time_ms = 300 },
};

#define DEF_CYCLE_NUM_QUICK_FLASH 2
static const led_pattern_cycle_t led_pattern_cycle_quick_flash[DEF_CYCLE_NUM_QUICK_FLASH] =
{
	{ .onoff = 1, .time_ms = 300 },
	{ .onoff = 0, .time_ms = 300 },
};

#define DEF_CYCLE_NUM_SLOW_FLASH 2
static const led_pattern_cycle_t led_pattern_cycle_slow_flash[DEF_CYCLE_NUM_SLOW_FLASH] =
{
	{ .onoff = 1, .time_ms = 300 },
	{ .onoff = 0, .time_ms = 2700 },
};

static const led_pattern_t led_pattern[] =
{
	{
		.pattern_id = LED_PATTERN_FLASH_ONCE,
		.cycle = led_pattern_cycle_flash_once,
		.cycle_total = DEF_CYCLE_NUM_FLASH_ONCE,
		.periodic = false,
	},
	{
		.pattern_id = LED_PATTERN_FLASH_TWICE,
		.cycle = led_pattern_cycle_flash_twice,
		.cycle_total = DEF_CYCLE_NUM_FLASH_TWICE,
		.periodic = false,
	},
	{
		.pattern_id = LED_PATTERN_QUICK_FLASH,
		.cycle = led_pattern_cycle_quick_flash,
		.cycle_total = DEF_CYCLE_NUM_QUICK_FLASH,
		.periodic = true,
	},
	{
		.pattern_id = LED_PATTERN_SLOW_FLASH,
		.cycle = led_pattern_cycle_slow_flash,
		.cycle_total = DEF_CYCLE_NUM_SLOW_FLASH,
		.periodic = true,
	},
};

/**
 * @brief   Search for the pattern by pattern ID.
 *
 * @param   pattern_id The pattern ID.
 *
 * @retval  Pattern handle for reference or NULL in case of error.
 */
const led_pattern_t* led_pattern_search_id(led_pattern_id_t pattern_id)
{
	int i;

	for (i = 0; i < sizeof(led_pattern) / sizeof(led_pattern[0]); i++)
		if (led_pattern[i].pattern_id == pattern_id)
			return &led_pattern[i];

	return NULL;
}

#endif
