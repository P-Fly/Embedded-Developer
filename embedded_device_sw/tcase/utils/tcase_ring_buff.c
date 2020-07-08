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
#include "ring_buff.h"
#include "tunit.h"

#ifdef CONFIG_TUNIT_RING_BUFF_SUIT_NAME
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

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_ring_buff(void)
{
	ring_buff_t ring;
	char buffer[10];
	char value;
	int i;
	int ret;

	ret =
		ring_buffer_init(&ring, buffer,
				 sizeof(buffer) / sizeof(buffer[0]));
	TUNIT_TEST(ret == 0);
	TUNIT_TEST(is_empty(&ring));

	for (i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++) {
		ret = ring_buffer_write(&ring, i);
		TUNIT_TEST(ret == 0);
	}
	TUNIT_TEST(is_full(&ring));

	ret = ring_buffer_write(&ring, i);
	TUNIT_TEST(ret == -EFULL);

	for (i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++) {
		ret = ring_buffer_read(&ring, &value);
		TUNIT_TEST(ret == 0);
		TUNIT_TEST(value == i);
	}
	TUNIT_TEST(is_empty(&ring));

	ret = ring_buffer_read(&ring, &value);
	TUNIT_TEST(ret == -EEMPTY);
}

define_tunit_suit(CONFIG_TUNIT_RING_BUFF_SUIT_NAME,
		  CONFIG_TUNIT_RING_BUFF_SUIT_LABEL,
		  tcase_suit_initialize,
		  tcase_suit_cleanup);
define_tunit_case(CONFIG_TUNIT_RING_BUFF_SUIT_NAME,
		  CONFIG_TUNIT_RING_BUFF_SUIT_LABEL,
		  "Ring buff init/read/write test",
		  tcace_ring_buff);

#endif /* CONFIG_TUNIT_RING_BUFF_SUIT_NAME */
