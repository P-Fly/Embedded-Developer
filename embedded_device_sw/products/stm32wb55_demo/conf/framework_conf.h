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

#ifndef __FRAMEWORK_CONF_H__
#define __FRAMEWORK_CONF_H__

#define CONFIG_MSG_SEND_BLOCK_TIMEOUT_MS 20

#define CONFIG_INIT_THREAD_NAME "init thread"
#define CONFIG_INIT_THREAD_STACK_SIZE 2048
#define CONFIG_INIT_THREAD_PRIORITY osPriorityRealtime

#define CONFIG_SERVICE_DEFAULT_THREAD_NAME "default service thread"
#define CONFIG_SERVICE_DEFAULT_THREAD_STACK_SIZE 2048
#define CONFIG_SERVICE_DEFAULT_THREAD_PRIORITY osPriorityNormal

#define CONFIG_SERVICE_DEFAULT_QUEUE_NAME "default service queue"
#define CONFIG_SERVICE_DEFAULT_QUEUE_LENGTH 10

#define CONFIG_UI_SERVICE_ENABLE
#if defined(CONFIG_UI_SERVICE_ENABLE)
#define CONFIG_UI_SERVICE_NAME "ui service"
#define CONFIG_UI_SERVICE_LABEL ui_service
#endif

#define CONFIG_LED_SERVICE_ENABLE
#if defined(CONFIG_LED_SERVICE_ENABLE)
#define CONFIG_LED_SERVICE_NAME "led service"
#define CONFIG_LED_SERVICE_LABEL led_service

#define CONFIG_LED_INSTANCE_NUM 1
#define CONFIG_LED_TIMER_NAME "default led service timer"
#define CONFIG_LED_ID_CONFIGS \
	{ \
		{ \
			"user_led", \
			0, \
		}, \
	}
#define CONFIG_LED_HW_CONFIGS \
	{ \
		{ \
			CONFIG_GPIOF_NAME, \
			DRV_GPIO_PIN_3, \
			GPIO_CONFIG_MODE_OUTPUT_PP, \
			DRV_GPIO_PIN_SET, \
			DRV_GPIO_PIN_CLR, \
		}, \
	}
#endif

#endif /* __FRAMEWORK_CONF_H__ */
