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

#include "stm32wbxx_hal.h"

static void hardware_print_info(void);

#define CONFIG_CPU_NAME   "STM32WBXX"
#define CONFIG_BOARD_NAME "NUCLEO-WB55"

void hardware_system_clock_config(void)
{
	RCC_OscInitTypeDef osc_config;
	RCC_ClkInitTypeDef clk_config;

	osc_config.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	osc_config.MSIState = RCC_MSI_ON;
	osc_config.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	osc_config.MSIClockRange = RCC_MSIRANGE_6;
	osc_config.PLL.PLLState = RCC_PLL_ON;
	osc_config.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	osc_config.PLL.PLLM = RCC_PLLM_DIV1;
	osc_config.PLL.PLLN = 32;
	osc_config.PLL.PLLP = RCC_PLLP_DIV5;
	osc_config.PLL.PLLR = RCC_PLLR_DIV2;
	osc_config.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&osc_config) != HAL_OK)
		while(1);

	clk_config.ClockType = RCC_CLOCKTYPE_HCLK4 |
				      RCC_CLOCKTYPE_HCLK2 | RCC_CLOCKTYPE_HCLK |
				      RCC_CLOCKTYPE_SYSCLK |
				      RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_config.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clk_config.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clk_config.APB1CLKDivider = RCC_HCLK_DIV1;
	clk_config.APB2CLKDivider = RCC_HCLK_DIV1;
	clk_config.AHBCLK2Divider = RCC_SYSCLK_DIV2;
	clk_config.AHBCLK4Divider = RCC_SYSCLK_DIV1;

	if (HAL_RCC_ClockConfig(&clk_config, FLASH_LATENCY_3) != HAL_OK)
		while(1);
}

/**
 * @brief   Startup hardware early.
 *
 * @retval  None.
 *
 * @note    This process is called before the OS start.
 */
void hardware_early_startup(void)
{
	HAL_Init();

	hardware_system_clock_config();
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

	pr_info("System Clock: %d Hz", SystemCoreClock);

	pr_info("*************************************************************");
}
