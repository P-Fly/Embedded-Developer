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

#include "cmsis_version.h"
#include "cmsis_os2.h"
#include "object.h"
#include "err.h"
#include "log.h"
#include "version.h"

#define TARGET_IS_TEMPEST_RC1
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

static const char *hardware_get_reset_reason(void);
static void hardware_print_info(void);

#define CONFIG_CPU_NAME   "LM3S9B96"
#define CONFIG_BOARD_NAME "DK-LM3S9B96"

/* Default system clock */
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

	SystemCoreClock = MAP_SysCtlClockGet();
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
	hardware_print_info();
}

typedef struct {
	unsigned long reset;
	const char *reason;
} hardware_reset_reason_t;

static const hardware_reset_reason_t reset_reason[] =
{
	SYSCTL_RESC_MOSCFAIL,    "MOSC Failure Reset",
	SYSCTL_RESC_WDT1,        "Watchdog Timer 1 Reset",
	SYSCTL_RESC_SW,          "Software Reset",
	SYSCTL_RESC_WDT0,        "Watchdog Timer 0 Reset",
	SYSCTL_RESC_BOR,         "Brown-Out Reset",
	SYSCTL_RESC_POR,         "Power-On Reset",
	SYSCTL_RESC_EXT,         "External Reset",
};

/**
 * @brief   Get hardware reset reason.
 *
 * @param   None.
 *
 * @retval  Return the string of reset reason.
 */
static const char *hardware_get_reset_reason(void)
{
	unsigned long reset;
	int i;

	reset = MAP_SysCtlResetCauseGet();

	MAP_SysCtlResetCauseClear(reset);

	for (i = 0; i < sizeof(reset_reason) / sizeof(reset_reason[0]); i++) {
		if (reset & reset_reason[i].reset) {
			return reset_reason[i].reason;
		}
	}

	return "Unknow Reset";
}

/**
 * @brief   Print some board info.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void hardware_print_info(void)
{
	char version[25];

	pr_info("*************************************************************");

	pr_info("%s - %s (Build %s %s)",
		CONFIG_ISSUE_NAME,
		CONFIG_ISSUE_VERSION,
		CONFIG_ISSUE_DATE,
		CONFIG_ISSUE_TIME);

	pr_info("Reset reason: %s", hardware_get_reset_reason());

	pr_info("CPU: %s", CONFIG_CPU_NAME);

	pr_info("Board: %s", CONFIG_BOARD_NAME);

#ifdef __ARMCC_VERSION
	pr_info("ARMCC Version: 0x%08x", __ARMCC_VERSION);
#endif

	pr_info("CMSIS Version: 0x%08x", __CM_CMSIS_VERSION);

	(void)osKernelGetInfo(NULL, version, sizeof(version));

	pr_info("OS Version: %s", version);

	pr_info("");
	pr_info("");

	pr_info("Flash Size: %d", MAP_SysCtlFlashSizeGet());
	pr_info("SRAM Size: %d", MAP_SysCtlSRAMSizeGet());
	pr_info("System Clock: %d", SystemCoreClock);
	pr_info("PWM Clock: %d", MAP_SysCtlPWMClockGet());
	pr_info("ADC Speed: %d", MAP_SysCtlADCSpeedGet());

	pr_info("*************************************************************");
}
