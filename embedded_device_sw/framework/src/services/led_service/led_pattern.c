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
#include "log.h"
#include "led_pattern.h"

#if defined(CONFIG_LED_SERVICE_ENABLE)

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
		.id = LED_PATTERN_FLASH_ONCE,
		.cycle = led_pattern_cycle_flash_once,
		.cycle_total = DEF_CYCLE_NUM_FLASH_ONCE,
		.periodic = 0,
	},
	{
		.id = LED_PATTERN_FLASH_TWICE,
		.cycle = led_pattern_cycle_flash_twice,
		.cycle_total = DEF_CYCLE_NUM_FLASH_TWICE,
		.periodic = 0,
	},
	{
		.id = LED_PATTERN_QUICK_FLASH,
		.cycle = led_pattern_cycle_quick_flash,
		.cycle_total = DEF_CYCLE_NUM_QUICK_FLASH,
		.periodic = 1,
	},
	{
		.id = LED_PATTERN_SLOW_FLASH,
		.cycle = led_pattern_cycle_slow_flash,
		.cycle_total = DEF_CYCLE_NUM_SLOW_FLASH,
		.periodic = 1,
	},
};

const led_pattern_t* led_pattern_search_id(led_pattern_id_t id)
{
	int i;

	for (i = 0; i < sizeof(led_pattern) / sizeof(led_pattern[0]); i++)
		if (led_pattern[i].id == id)
			return &led_pattern[i];

	return NULL;
}

#endif
