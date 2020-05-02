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
#include "cmsis_os.h"
#include "object.h"
#include "err.h"
#include "CUnit.h"
#include "Basic.h"
#include "tunit.h"

#ifdef CONFIG_TUNIT_ENABLE

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
static void tcase_testcase(void)
{
    TUNIT_TEST(CU_TRUE);
}

define_tunit_suit(CONFIG_TUNIT_TUNIT_SUIT_NAME,
    tcase_suit_initialize, tcase_suit_cleanup);
define_tunit_case(CONFIG_TUNIT_TUNIT_SUIT_NAME,
    tcase_testcase, tcase_testcase);

extern tunit_suit tunit_suit$$Base[];
extern tunit_suit tunit_suit$$Limit[];
extern tunit_case tunit_case$$Base[];
extern tunit_case tunit_case$$Limit[];

/**
 * @brief   Register new case sets and add to the specified suite.
 *
 * @param   csuit Test suite to which to add new case.
 * @param   suit_name Name for the new test suite.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int tunit_register_case_sets(const CU_pSuite csuit, const char* suit_name)
{
    tunit_case* tcase;
    CU_pTest ret;

    for (tcase = tunit_case$$Base; tcase < tunit_case$$Limit; tcase++)
    {
        if (tcase && !strcmp(suit_name, tcase->suit_name))
        {
            ret = CU_add_test(csuit, tcase->case_name, tcase->case_func);
            if (!ret)
                return -EINVAL;
        }
    }

    return 0;
}

/**
 * @brief   Register all suites and case sets.
 *
 * @param   None.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int tunit_register_all_suit_and_case(void)
{
    CU_pSuite csuit;
    tunit_suit* tsuit;
    int ret;

    for (tsuit = tunit_suit$$Base; tsuit < tunit_suit$$Limit; tsuit++)
    {
        csuit = CU_add_suite(tsuit->suit_name, tsuit->initialize, tsuit->cleanup);
        if (!csuit)
            return -EINVAL;

        ret = tunit_register_case_sets(csuit, tsuit->suit_name);
        if (ret)
            return -EINVAL;
    }

    return 0;
}

static void tunit_thread(void const* argument);
osThreadDef(tunit, tunit_thread, osPriorityNormal, 0, 2048);

/**
 * @brief   A unit testing application, depend on CUnit module.
 *
 * @param   argument None.
 *
 * @retval  None.
 */
static void tunit_thread(void const* argument)
{
    (void)argument;

    CU_initialize_registry();

    tunit_register_all_suit_and_case();

    CU_basic_set_mode(CU_BRM_VERBOSE);

    CU_basic_run_tests();

    CU_cleanup_registry();

    for (;;)
    {
        osThreadSuspend(NULL);
    }
}

/**
 * @brief   tunit handle definition.
 */
typedef struct
{
    osThreadId thread_id;
} tunit_handle_t;

static tunit_handle_t tunit_handle;

/**
 * @brief   Probe the tunit module.
 *
 * @param   obj Pointer to the tunit module handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int tunit_probe(const object* obj)
{
    tunit_handle_t* handle = (tunit_handle_t*)obj->object_data;

    (void)memset(handle, 0, sizeof(tunit_handle_t));

    handle->thread_id = osThreadCreate(osThread(tunit), NULL);

    return 0;
}

/**
 * @brief   Remove the tunit module.
 *
 * @param   obj Pointer to the tunit module handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int tunit_shutdown(const object* obj)
{
    tunit_handle_t* handle = (tunit_handle_t*)obj->object_data;

    if (handle->thread_id)
        osThreadTerminate(handle->thread_id);

    return 0;
}

module_application(CONFIG_TUNIT_NAME,
    tunit_probe,
    tunit_shutdown,
    NULL, &tunit_handle, NULL);

#endif /* CONFIG_TUNIT_ENABLE */
