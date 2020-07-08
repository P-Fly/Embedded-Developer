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
#include <assert.h>
#include "FreeRTOS.h"
#include "task.h"
#include "log.h"

/**
 * @brief   Function to malloc failed hook.
 */
void vApplicationMallocFailedHook(void)
{
	char *name = pcTaskGetName(xTaskGetCurrentTaskHandle());

	pr_error("Malloc failed at task <%s>.", name);

	assert(0);
}

/**
 * @brief   Function to stack overflow hook.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
	pr_error("Stack overflow at task <%s>.", pcTaskName);

	assert(0);
}
