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

#ifndef __TUNIT_H__
#define __TUNIT_H__

#include <string.h>
#include "CUnit.h"
#include "tunit_conf.h"

#define _STRINGIFY(x) #x
#define STRINGIFY(s) _STRINGIFY(s)

struct _tunit_suit;
typedef struct _tunit_suit tunit_suit;
struct _tunit_case;
typedef struct _tunit_case tunit_case;

/**
 * @brief   Test Suit interfaces.
 */
typedef int (*tunit_suit_initialize_fn)(void);
typedef int (*tunit_suit_cleanup_fn)(void);

/**
 * @brief   Test Suit structure.
 */
typedef struct _tunit_suit {
	char *				suit_name;
	tunit_suit_initialize_fn	initialize;
	tunit_suit_cleanup_fn		cleanup;
} tunit_suit;

#define __define_tunit_suit(_suit_name, _initialize, _cleanup) \
	static tunit_suit __tunit_suit_def_ ## _suit_name \
	__attribute__((used, section("tunit_suit"))) = { \
		.suit_name	= STRINGIFY(_suit_name), \
		.initialize	= (_initialize), \
		.cleanup	= (_cleanup) }

#define define_tunit_suit(name, initialize, cleanup) \
	__define_tunit_suit(name, initialize, cleanup)

/**
 * @brief   Test Case interfaces.
 */
typedef void (*tunit_case_fn)(void);

/**
 * @brief   Test Case structure.
 */
typedef struct _tunit_case {
	char *		suit_name;
	char *		case_name;
	tunit_case_fn	case_func;
} tunit_case;

#define __define_tunit_case(_suit_name, _case_name, _case_func) \
	static tunit_case __tunit_case_def_ ## _suit_name ## _ ## _case_func \
	__attribute__((used, section("tunit_case"))) = { \
		.suit_name	= STRINGIFY(_suit_name), \
		.case_name	= (_case_name), \
		.case_func	= (_case_func) }

#define define_tunit_case(_suit_name, _case_name, _case_func) \
	__define_tunit_case(_suit_name, _case_name, _case_func)

#define TUNIT_TEST(value) CU_TEST(value)
#define TUNIT_TEST_FATAL(value) CU_TEST_FATAL(value)

#endif /* __TUNIT_H__ */
