#include "cmsis_os.h"
#include "object.h"
#include "err.h"
#include "unit_conf.h"
#include "CUnit.h"

#ifdef CONFIG_UNIT_ENABLE

static void unit_thread(void const* argument);
osThreadDef(unit, unit_thread, osPriorityNormal, 0, 2048);

/**
 * @brief   
 *
 * @param   argument None.
 *
 * @retval  None.
 */
static void unit_thread(void const* argument)
{
    (void)argument;

    CU_initialize_registry();

#if 0
    CU_add_suite();
    CU_add_test();
    CU_console_run_tests();
    CU_cleanup_registry();
#endif

    for (;;)
    {
        osThreadSuspend(NULL);
    }
}

/**
 * @brief   Probe the CRC driver.
 *
 * @param   obj Pointer to the CRC object handle.
 *
 * @retval  None.
 */
static int unit_probe(const object* obj)
{
    osThreadCreate(osThread(unit), NULL);

    return 0;
}

/**
 * @brief   Remove the CRC driver.
 *
 * @param   obj Pointer to the CRC object handle.
 *
 * @retval  None.
 */
static int unit_shutdown(const object* obj)
{
    return 0;
}

module_application(CONFIG_UNIT_NAME,
    unit_probe,
    unit_shutdown,
    NULL, NULL, NULL);

#endif /* CONFIG_UNIT_ENABLE */
