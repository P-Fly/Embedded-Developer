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

#ifndef __TUNIT_CONF_H__
#define __TUNIT_CONF_H__

#define CONFIG_TUNIT_ENABLE
#if defined(CONFIG_TUNIT_ENABLE)
#define CONFIG_TUNIT_NAME "tunit application"
#define CONFIG_TUNIT_LABEL tunit_application

#define CONFIG_TUNIT_THREAD_NAME "tunit thread"
#define CONFIG_TUNIT_THREAD_STACK_SIZE 2048
#define CONFIG_TUNIT_THREAD_PRIORITY osPriorityNormal

#define CONFIG_TUNIT_TUNIT_SUIT_NAME "tunit test suite"
#define CONFIG_TUNIT_TUNIT_SUIT_LABEL tunit_suite
#define CONFIG_TUNIT_RING_BUFF_SUIT_NAME "ring buff test suite"
#define CONFIG_TUNIT_RING_BUFF_SUIT_LABEL ring_buff_suite
#define CONFIG_TUNIT_CRC_SUIT_NAME "crc test suite"
#define CONFIG_TUNIT_CRC_SUIT_LABEL crc_suite
#define CONFIG_TUNIT_SERVICE_SUIT_NAME "service test suite"
#define CONFIG_TUNIT_SERVICE_SUIT_LABEL service_suite
#if defined(CONFIG_TUNIT_SERVICE_SUIT_NAME)
#define CONFIG_TUNIT_SERVICE_FOO_NAME "foo service"
#define CONFIG_TUNIT_SERVICE_FOO_LABEL foo_service
#define CONFIG_TUNIT_SERVICE_BAR_NAME "bar service"
#define CONFIG_TUNIT_SERVICE_BAR_LABEL bar_service
#define CONFIG_TUNIT_SERVICE_BAZ_NAME "baz service"
#define CONFIG_TUNIT_SERVICE_BAZ_LABEL baz_service
#endif
#endif

#endif /* __TUNIT_CONF_H__ */
