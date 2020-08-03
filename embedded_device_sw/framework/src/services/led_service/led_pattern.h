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

#ifndef __LED_PATTERN_H__
#define __LED_PATTERN_H__

#include <stdbool.h>

/**
 * @brief   Led pattern cycle definitions.
 */
typedef struct {
    unsigned int onoff;
    unsigned int time_ms;
} led_pattern_cycle_t;

/**
 * @brief   Led pattern definitions.
 */
typedef struct {
	led_pattern_id_t pattern_id;
	const led_pattern_cycle_t* cycle;
	unsigned int cycle_total;
	bool periodic;
} led_pattern_t;

extern const led_pattern_t* led_pattern_search_id(led_pattern_id_t id);

#endif /* __LED_PATTERN_H__ */
