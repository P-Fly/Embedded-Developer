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

#ifndef __UTILS_CONF_H__
#define __UTILS_CONF_H__

#define CONFIG_DBG_TRACE_ENABLE
#if defined(CONFIG_DBG_TRACE_ENABLE)
#define CONFIG_DBG_TRACE_NAME "dbg trace module"
#define CONFIG_DBG_TRACE_LABEL dbg_trace_module
#define CONFIG_DBG_TRACE_PORT_NAME CONFIG_UART1_NAME
#define CONFIG_DBG_TRACE_MAX_LEN 256
#endif

#define CONFIG_TRACE_ENABLE
#if defined(CONFIG_TRACE_ENABLE)
#define CONFIG_TRACE_NAME "trace module"
#define CONFIG_TRACE_LABEL trace_module
#define CONFIG_TRACE_PORT_NAME CONFIG_UART1_NAME
#endif

#define CONFIG_ASSERT_ENABLE

#endif /* __UTILS_CONF_H__ */
