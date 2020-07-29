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
#include "service.h"
#include "log.h"
#include "led_pattern.h"
#include "led_hardware.h"
#include "drv_gpio.h"

#if defined(CONFIG_LED_SERVICE_ENABLE)

typedef struct
{
	const led_hardware_t* config;
	const led_pattern_t* pattern;
	const object *port;
	osTimerId_t timer;
} led_instance_t;

typedef struct {
	led_instance_t instance[CONFIG_LED_INSTANCE_NUM];
} led_service_priv_t;

static int led_service_init(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	unsigned int i;

	(void)memset(priv_data, 0, sizeof(led_service_priv_t));

	for (i = 0; i < CONFIG_LED_INSTANCE_NUM; i++)
	{
		priv_data->instance[i].config = led_hardware_search_index(i);
		/*osTimerNew (osTimerFunc_t func, osTimerOnce, NULL, const osTimerAttr_t *attr)*/
	}

	return 0;
}

static int led_service_deinit(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;

	(void)priv_data;

	return 0;
}

static void led_service_handle_message(const message_t *message,
				       message_t *	rsp_message,
				       void *		priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;

	(void)priv_data;

	switch (message->id) {
	case MSG_ID_LED_SERVICE_FLASH_ONCE_EVT:
		break;

	case MSG_ID_LED_SERVICE_FLASH_TWICE_EVT:
		break;

	case MSG_ID_LED_SERVICE_REPEAT_QUICK_FLASH_EVT:
		break;

	case MSG_ID_LED_SERVICE_REPEAT_SLOW_FLASH_EVT:
		break;

	default:
		break;
	}
}

static led_service_priv_t led_service_priv;

DECLARE_SERVICE(CONFIG_LED_SERVICE_NAME,
		CONFIG_LED_SERVICE_LABEL,
		&led_service_priv,
		led_service_init,
		led_service_deinit,
		led_service_handle_message);

#endif
