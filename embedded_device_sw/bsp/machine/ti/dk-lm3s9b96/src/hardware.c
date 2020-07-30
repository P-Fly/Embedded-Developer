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

#include <stdint.h>
#include "cmsis_os2.h"
#include "object.h"
#include "err.h"
#include "log.h"
#include "service.h"
#include "drv_gpio.h"

#define TARGET_IS_TEMPEST_RC1
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

uint32_t SystemCoreClock = 16000000UL;

/**
 * @brief   Startup hardware early.
 *
 * @retval  None.
 *
 * @note    This process is called before the OS start.
 */
void hardware_early_startup(void)
{
	/* Set the clocking to run from the PLL at 16MHz, now the system core clock about 50MHz */
	MAP_SysCtlClockSet(
		SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
		SYSCTL_XTAL_16MHZ);

	SystemCoreClock = 50000000UL;
}

/**
 * @brief   Startup hardware late.
 *
 * @retval  None.
 *
 * @note    This process is called after initialization thread is completed.
 */
void hardware_late_startup(void)
{
	int ret;

	osDelay(500 * osKernelGetTickFreq() / 1000);

	message_t message;
	message.id = MSG_ID_SYS_STARTUP_COMPLETED;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_broadcast_evt(&message);
	if (ret)
		pr_error("Broadcast event 0x%x failed.", message.id);
	else
		pr_info("Broadcast event 0x%x succeed.", message.id);
}
