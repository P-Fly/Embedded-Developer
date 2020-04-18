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

#include "cmsis_os.h"
#include "object.h"
#include "err.h"

/**
 * @brief   Startup hardware early.
 *
 * @retval  None.
 *
 * @note    This process is called before the OS start.
 */
__weak void hardware_early_startup(void)
{
    /**
     * This function should not be modified, when the callback is needed,
     * please override it on product layer.
     */
}

/**
 * @brief   Startup hardware late.
 *
 * @retval  None.
 *
 * @note    This process is called after initialization thread is completed.
 */
__weak void hardware_late_startup(void)
{
    /**
     * This function should not be modified, when the callback is needed,
     * please override it on product layer.
     */
}

static void init_thread(void const* argument);
osThreadDef(init, init_thread, osPriorityRealtime, 0, 2048);

/**
 * @brief   Initialize thread, include all drivers, services, applications etc..
 *
 * @param   argument None.
 *
 * @retval  None.
 */
static void init_thread(void const* argument)
{
    (void)argument;

    (void)object_init();

    hardware_late_startup();

    osThreadSuspend(NULL);
}

int main(int argc, char* argv[])
{
    hardware_early_startup();

    osThreadCreate(osThread(init), NULL);

    osKernelStart();

    return 0;
}
