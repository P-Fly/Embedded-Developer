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
#include "cmsis_os2.h"
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

/**
 * @brief   Attributes structure for init thread.
 */
const osThreadAttr_t init_attr = {
	.name		= "init",
	.attr_bits	= osThreadDetached,
	.cb_mem		= NULL,
	.cb_size	= 0,
	.stack_mem	= NULL,
	.stack_size	= 256,
	.priority	= osPriorityRealtime,
};

/**
 * @brief   Initialize thread, include all drivers, services, applications etc.
 *
 * @param   argument None.
 *
 * @retval  None.
 */
static void init_thread(void *argument)
{
	osStatus_t stat;

	(void)argument;

	(void)object_init();

	printf("All object initialized.\r\n");

	hardware_late_startup();

	stat = osThreadSuspend(osThreadGetId());
	if (stat != osOK)
		printf("Suspend %s thread failed, stat = %d.\r\n",
		       osThreadGetName(osThreadGetId()),
		       stat);
}

__attribute__((noreturn))
int main(int argc, char *argv[])
{
	osThreadId_t thread_id;
	osStatus_t stat;

	hardware_early_startup();

	stat = osKernelInitialize();
	if (stat != osOK)
		printf("Kernel initialize failed, stat = %d.\r\n", stat);

	thread_id = osThreadNew(init_thread, NULL, &init_attr);
	if (!thread_id)
		printf("Create %s thread failed.\r\n", init_attr.name);
	else
		printf("Create %s thread succeed.\r\n", init_attr.name);

	stat = osKernelStart();
	if (stat != osOK)
		printf("Kernel start failed, stat = %d.\r\n", stat);

	/* The program is undefined, if the code reaches this point. */
	while (1);
}
