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

struct _tunit_suit_t;
typedef struct _tunit_suit_t tunit_suit_t;
struct _tunit_case_t;
typedef struct _tunit_case_t tunit_case_t;

/**
 * @brief   Test Suit interfaces.
 */
typedef int (*tunit_suit_initialize_fn)(void);
typedef int (*tunit_suit_cleanup_fn)(void);

/**
 * @brief   Test Suit structure.
 */
typedef struct _tunit_suit_t {
	char *				suit_name;
	tunit_suit_initialize_fn	initialize;
	tunit_suit_cleanup_fn		cleanup;
} tunit_suit_t;

#define __define_tunit_suit(_suit_name, _suit_label, _initialize, _cleanup) \
	static tunit_suit_t __tunit_suit_def_ ## _suit_label \
	__attribute__((used, section("tunit_suit"))) = { \
		.suit_name	= (_suit_name), \
		.initialize	= (_initialize), \
		.cleanup	= (_cleanup) }

#define define_tunit_suit(name, label, initialize, cleanup) \
	__define_tunit_suit(name, label, initialize, cleanup)

/**
 * @brief   Test Case interfaces.
 */
typedef void (*tunit_case_fn)(void);

/**
 * @brief   Test Case structure.
 */
typedef struct _tunit_case_t {
	char *		suit_name;
	char *		case_name;
	tunit_case_fn	case_func;
} tunit_case_t;

#define __define_tunit_case(_suit_name, _suit_label, _case_name, _case_func) \
	static tunit_case_t __tunit_case_def_ ## _suit_label ## _ ## _case_func \
	__attribute__((used, section("tunit_case"))) = { \
		.suit_name	= (_suit_name), \
		.case_name	= (_case_name), \
		.case_func	= (_case_func) }

#define define_tunit_case(_suit_name, _suit_label, _case_name, _case_func) \
	__define_tunit_case(_suit_name, _suit_label, _case_name, _case_func)

#define TUNIT_PASS(msg) \
	CU_PASS(msg)
#define TUNIT_ASSERT(value) \
	CU_ASSERT(value)
#define TUNIT_ASSERT_FATAL(value) \
	CU_ASSERT_FATAL(value)
#define TUNIT_TEST(value) \
	CU_TEST(value)
#define TUNIT_TEST_FATAL(value) \
	CU_TEST_FATAL(value)
#define TUNIT_FAIL(msg) \
	CU_FAIL(msg)
#define TUNIT_FAIL_FATAL(msg) \
	CU_FAIL_FATAL(msg)
#define TUNIT_ASSERT_TRUE(value) \
	CU_ASSERT_TRUE(value)
#define TUNIT_ASSERT_TRUE_FATAL(value) \
	CU_ASSERT_TRUE_FATAL(value)
#define TUNIT_ASSERT_FALSE(value) \
	CU_ASSERT_FALSE(value)
#define TUNIT_ASSERT_FALSE_FATAL(value) \
	CU_ASSERT_FALSE_FATAL(value)
#define TUNIT_ASSERT_EQUAL(actual, expected) \
	CU_ASSERT_EQUAL(actual, expected)
#define TUNIT_ASSERT_EQUAL_FATAL(actual, expected) \
	CU_ASSERT_EQUAL_FATAL(actual, expected)
#define TUNIT_ASSERT_NOT_EQUAL(actual, expected) \
	CU_ASSERT_NOT_EQUAL(actual, expected)
#define TUNIT_ASSERT_NOT_EQUAL_FATAL(actual, expected) \
	CU_ASSERT_NOT_EQUAL_FATAL(actual, expected)
#define TUNIT_ASSERT_PTR_EQUAL(actual, expected) \
	CU_ASSERT_PTR_EQUAL(actual, expected)
#define TUNIT_ASSERT_PTR_EQUAL_FATAL(actual, expected) \
	CU_ASSERT_PTR_EQUAL_FATAL(actual, expected)
#define TUNIT_ASSERT_PTR_NOT_EQUAL(actual, expected) \
	CU_ASSERT_PTR_NOT_EQUAL(actual, expected)
#define TUNIT_ASSERT_PTR_NOT_EQUAL_FATAL(actual, expected) \
	CU_ASSERT_PTR_NOT_EQUAL_FATAL(actual, expected)
#define TUNIT_ASSERT_PTR_NULL(value) \
	CU_ASSERT_PTR_NULL(value)
#define TUNIT_ASSERT_PTR_NULL_FATAL(value) \
	CU_ASSERT_PTR_NULL_FATAL(value)
#define TUNIT_ASSERT_PTR_NOT_NULL(value) \
	CU_ASSERT_PTR_NOT_NULL(value)
#define TUNIT_ASSERT_PTR_NOT_NULL_FATAL(value) \
	CU_ASSERT_PTR_NOT_NULL_FATAL(value)
#define TUNIT_ASSERT_STRING_EQUAL(actual, expected) \
	CU_ASSERT_STRING_EQUAL(actual, expected)
#define TUNIT_ASSERT_STRING_EQUAL_FATAL(actual, expected) \
	CU_ASSERT_STRING_EQUAL_FATAL(actual, expected)
#define TUNIT_ASSERT_STRING_NOT_EQUAL(actual, expected) \
	CU_ASSERT_STRING_NOT_EQUAL(actual, expected)
#define TUNIT_ASSERT_STRING_NOT_EQUAL_FATAL(actual, expected) \
	CU_ASSERT_STRING_NOT_EQUAL_FATAL(actual, expected)
#define TUNIT_ASSERT_NSTRING_EQUAL(actual, expected, count) \
	CU_ASSERT_NSTRING_EQUAL(actual, expected, count)
#define TUNIT_ASSERT_NSTRING_EQUAL_FATAL(actual, expected, count) \
	CU_ASSERT_NSTRING_EQUAL_FATAL(actual, expected, count)
#define TUNIT_ASSERT_NSTRING_NOT_EQUAL(actual, expected, count) \
	CU_ASSERT_NSTRING_NOT_EQUAL(actual, expected, count)
#define TUNIT_ASSERT_NSTRING_NOT_EQUAL_FATAL(actual, expected, count) \
	CU_ASSERT_NSTRING_NOT_EQUAL_FATAL(actual, expected, count)
#define TUNIT_ASSERT_DOUBLE_EQUAL(actual, expected, granularity) \
	CU_ASSERT_DOUBLE_EQUAL(actual, expected, granularity)
#define TUNIT_ASSERT_DOUBLE_EQUAL_FATAL(actual, expected, granularity) \
	CU_ASSERT_DOUBLE_EQUAL_FATAL(actual, expected, granularity)
#define TUNIT_ASSERT_DOUBLE_NOT_EQUAL(actual, expected, granularity) \
	CU_ASSERT_DOUBLE_NOT_EQUAL(actual, expected, granularity)
#define TUNIT_ASSERT_DOUBLE_NOT_EQUAL_FATAL(actual, expected, granularity) \
	CU_ASSERT_DOUBLE_NOT_EQUAL_FATAL(actual, expected, granularity)

#endif /* __TUNIT_H__ */
