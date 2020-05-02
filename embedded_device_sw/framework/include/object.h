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

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stddef.h>

#define _STRINGIFY(x) #x
#define STRINGIFY(s) _STRINGIFY(s)

struct _object;
typedef struct _object object;

/**
 * @brief   Standard object model interfaces.
 */
typedef int (*probe)(const object* obj);
typedef int (*shutdown)(const object* obj);
typedef int (*suspend)(const object* obj);
typedef int (*resume)(const object* obj);

/**
 * @brief   Standard object model structure.
 */
typedef struct _object
{
    char* name;

    probe probe;            /* Power on */
    shutdown shutdown;      /* Power off */
    suspend suspend;        /* Suspend */
    resume resume;          /* Resume */

    void* object_intf;      /* Object API */
    void* object_data;      /* Runtime instance */
    void* object_config;    /* User config */
} object;

/**
 * Objects are now grouped by functionality into separate
 * subsections. Ordering inside the subsections is determined
 * by link order.
 *
 * The 'id' arg to __define_object() is needed so that multiple objects
 * can point at the same handler without causing duplicate-symbol build errors.
 *
 * Objects are run by placing pointers in object sections that the
 * kernel iterates at runtime. The linker can do dead code / data elimination
 * and remove that completely, so the objects sections have to be marked
 * as 'used' in the linker attribute.
 */
#define __define_object(object_name, probe_fn, shutdown_fn, suspend_fn,     \
                resume_fn, intf, runtime, config, id)                       \
    static object __object_def_##id##_##object_name                         \
    __attribute__((used, section("module_object_" #id))) = {                \
        .name = STRINGIFY(object_name),                                     \
        .probe = (probe_fn),                                                \
        .shutdown = (shutdown_fn),                                          \
        .suspend = (suspend_fn), .resume = (resume_fn),                     \
        .object_intf = (intf), .object_data = (runtime),                    \
        .object_config = (config) }

#define module_pre(name, probe, shutdown, intf, runtime, config)            \
        __define_object(name, probe, shutdown, NULL, NULL,                  \
            intf, runtime, config, 0)
#define module_core(name, probe, shutdown, intf, runtime, config)           \
        __define_object(name, probe, shutdown, NULL, NULL,                  \
            intf, runtime, config, 1)
#define module_early_driver(name, probe, shutdown, intf, runtime, config)   \
        __define_object(name, probe, shutdown, NULL, NULL,                  \
            intf, runtime, config, 2)
#define module_driver(name, probe, shutdown, intf, runtime, config)         \
        __define_object(name, probe, shutdown, NULL, NULL,                  \
            intf, runtime, config, 3)
#define module_service_manager(name, probe, shutdown, intf, runtime, config)\
        __define_object(name, probe, shutdown, NULL, NULL,                  \
            intf, runtime, config, 4)
#define module_service(name, probe, shutdown, intf, runtime, config)        \
        __define_object(name, probe, shutdown, NULL, NULL,                  \
            intf, runtime, config, 5)
#define module_application(name, probe, shutdown, intf, runtime, config)    \
        __define_object(name, probe, shutdown, NULL, NULL,                  \
            intf, runtime, config, 6)

extern int object_init(void);
extern object* _object_get_binding(const char* const name);
#define object_get_binding(name) _object_get_binding(STRINGIFY(name))

#endif /* __OBJECT_H__ */
