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

#include <stdio.h>
#include <string.h>
#include "object.h"
#include "err.h"
#include "uart.h"
#include "utils_conf.h"

#ifdef CONFIG_TRACE_ENABLE

/**
 * @brief   Trace handle definition.
 */
typedef struct
{
    object* port;
} trace_handle_t;

trace_handle_t trace_handle;

/**
 * These are different compilers for ARM processors,
 * these macros are used to hide compiler-dependent stuff in code
 * that's compilable by several compilers.
 */
#if defined(__CC_ARM)

int fputc(int ch, FILE* f)
{
    char tempch = ch;
    int ret;

    (void)f;

    /**
     * Critical protection is not required,
     * due to this handle is not modified at runtime.
     */
    if (!trace_handle.port)
        return -EIO;

    ret = uart_write(trace_handle.port, &tempch, 1);
    if (ret != 1)
        return -EIO;

    return ch;
}

#elif defined(__GNUC__)

/**
 * To implement the STDIO functions you need to create
 * the _read and _write functions.
 */
#error "Must be override the _write and _read to redirect printf."

#elif defined(ICCARM)

/**
 * To implement the STDIO functions you need to create
 * the _read and _write functions.
 */
#error "Must be override the _write and _read to redirect printf."

#else

#error "Must be override the _write and _read to redirect printf."

#endif

/**
 * @brief   Probe the trace module.
 *
 * @param   obj Pointer to the trace object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int trace_probe(const object* obj)
{
    trace_handle_t* handle = (trace_handle_t*)obj->object_data;

    (void)memset(handle, 0, sizeof(trace_handle_t));

    handle->port = object_get_binding(CONFIG_TRACE_PORT_NAME);
    if (!handle->port)
        return -ENXIO;

    return 0;
}

/**
 * @brief   Remove the trace module.
 *
 * @param   obj Pointer to the trace object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int trace_shutdown(const object* obj)
{
    (void)obj;

    return 0;
}

module_early_driver(CONFIG_TRACE_NAME,
    trace_probe,
    trace_shutdown,
    NULL, &trace_handle, NULL);

#endif
