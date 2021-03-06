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
#include "drv_crc.h"
#include "tunit.h"

#ifdef CONFIG_TUNIT_CRC_SUIT_NAME
/**
 * @brief   Suite initialization function.
 *
 * @param   None.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int tcase_suit_initialize(void)
{
	return 0;
}

/**
 * @brief   Suite cleanup function.
 *
 * @param   None.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int tcase_suit_cleanup(void)
{
	return 0;
}

static const unsigned int raw_data_32B[] =
{
	0x00001021, 0x20423063, 0x408450a5, 0x60c670e7, 0x9129a14a, 0xb16bc18c,
	0xd1ade1ce, 0xf1ef1231, 0x32732252, 0x52b54294, 0x72f762d6, 0x93398318,
	0xa35ad3bd, 0xc39cf3ff, 0xe3de2462, 0x34430420, 0x64e674c7, 0x44a45485,
	0xa56ab54b, 0x85289509, 0xf5cfc5ac, 0xd58d3653, 0x26721611, 0x063076d7,
	0x569546b4, 0xb75ba77a, 0x97198738, 0xf7dfe7fe, 0xc7bc48c4, 0x58e56886,
	0x78a70840, 0x18612802, 0xc9ccd9ed, 0xe98ef9af, 0x89489969, 0xa90ab92b,
	0x4ad47ab7, 0x6a961a71, 0x0a503a33, 0x2a12dbfd, 0xfbbfeb9e, 0x9b798b58,
	0xbb3bab1a, 0x6ca67c87, 0x5cc52c22, 0x3c030c60, 0x1c41edae, 0xfd8fcdec,
	0xad2abd0b, 0x8d689d49, 0x7e976eb6, 0x5ed54ef4, 0x2e321e51, 0x0e70ff9f,
	0xefbedfdd, 0xcffcbf1b, 0x9f598f78, 0x918881a9, 0xb1caa1eb, 0xd10cc12d,
	0xe16f1080, 0x00a130c2, 0x20e35004, 0x40257046, 0x83b99398, 0xa3fbb3da,
	0xc33dd31c, 0xe37ff35e, 0x129022f3, 0x32d24235, 0x52146277, 0x7256b5ea,
	0x95a88589, 0xf56ee54f, 0xd52cc50d, 0x34e224c3, 0x04817466, 0x64475424,
	0x4405a7db, 0xb7fa8799, 0xe75ff77e, 0xc71dd73c, 0x26d336f2, 0x069116b0,
	0x76764615, 0x5634d94c, 0xc96df90e, 0xe92f99c8, 0xb98aa9ab, 0x58444865,
	0x78066827, 0x18c008e1, 0x28a3cb7d, 0xdb5ceb3f, 0xfb1e8bf9, 0x9bd8abbb,
	0x4a755a54, 0x6a377a16, 0x0af11ad0, 0x2ab33a92, 0xed0fdd6c, 0xcd4dbdaa,
	0xad8b9de8, 0x8dc97c26, 0x5c644c45, 0x3ca22c83, 0x1ce00cc1, 0xef1fff3e,
	0xdf7caf9b, 0xbfba8fd9, 0x9ff86e17, 0x7e364e55, 0x2e933eb2, 0x0ed11ef0
};

static const unsigned int expected_crc_32B = 0x379E9F06;

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_crc_calculate(void)
{
	const object *obj;
	crc_config_t config;
	unsigned int crc;
	int ret;

	obj = object_get_binding(CONFIG_CRC_NAME);
	TUNIT_TEST(obj != NULL);

	config.initial_value = CRC_CONFIG_INITIAL_VALUE_DEFAULT;
	config.polynomial = CRC_CONFIG_POLYNOMIAL_DEFAULT;
	config.configs = CRC_CONFIG_POLYNOMIAL_LENGTH_32B |
			 CRC_CONFIG_INPUT_32B;
	ret = crc_configure(obj, &config);
	TUNIT_TEST(ret == 0);

	ret =
		crc_calculate(obj,
			      raw_data_32B,
			      sizeof(raw_data_32B) / sizeof(raw_data_32B[0]),
			      &crc);
	TUNIT_TEST(ret == 0);
	TUNIT_TEST(crc == expected_crc_32B);
}

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_crc_accumulate(void)
{
	const object *obj;
	crc_config_t config;
	unsigned int crc;
	int ret;
	const unsigned int *pointer;
	int len;

	obj = object_get_binding(CONFIG_CRC_NAME);
	TUNIT_TEST(obj != NULL);

	config.initial_value = CRC_CONFIG_INITIAL_VALUE_DEFAULT;
	config.polynomial = CRC_CONFIG_POLYNOMIAL_DEFAULT;
	config.configs = CRC_CONFIG_POLYNOMIAL_LENGTH_32B |
			 CRC_CONFIG_INPUT_32B;
	ret = crc_configure(obj, &config);
	TUNIT_TEST(ret == 0);

	pointer = raw_data_32B;
	len = sizeof(raw_data_32B) / sizeof(raw_data_32B[0]) / 2;
	ret = crc_calculate(obj, pointer, len, &crc);
	TUNIT_TEST(ret == 0);

	pointer = raw_data_32B + len;
	len = sizeof(raw_data_32B) / sizeof(raw_data_32B[0]) - len;
	ret = crc_accumulate(obj, pointer, len, &crc);
	TUNIT_TEST(ret == 0);

	TUNIT_TEST(crc == expected_crc_32B);
}

define_tunit_suit(CONFIG_TUNIT_CRC_SUIT_NAME,
		  CONFIG_TUNIT_CRC_SUIT_LABEL,
		  tcase_suit_initialize,
		  tcase_suit_cleanup);
define_tunit_case(CONFIG_TUNIT_CRC_SUIT_NAME,
		  CONFIG_TUNIT_CRC_SUIT_LABEL,
		  "CRC calculate test",
		  tcace_crc_calculate);
define_tunit_case(CONFIG_TUNIT_CRC_SUIT_NAME,
		  CONFIG_TUNIT_CRC_SUIT_LABEL,
		  "CRC accumulate test",
		  tcace_crc_accumulate);

#endif /* CONFIG_TUNIT_CRC_SUIT_NAME */
