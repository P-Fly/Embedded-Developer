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

#ifndef __LED_SERVICES_H__
#define __LED_SERVICES_H__

/**
 * @brief   Led pattern ID definitions.
 */
typedef enum {
	LED_PATTERN_FLASH_ONCE,
	LED_PATTERN_FLASH_TWICE,
	LED_PATTERN_QUICK_FLASH,
	LED_PATTERN_SLOW_FLASH,
} led_pattern_id_t;

#endif /* __LED_SERVICES_H__ */
