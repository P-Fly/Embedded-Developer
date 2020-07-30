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
#include "led_id.h"
#include "led_hardware.h"
#include "led_pattern.h"
#include "drv_gpio.h"

#if defined(CONFIG_LED_SERVICE_ENABLE)

typedef struct
{
	const led_id_t* id;
	const led_hardware_t* hardware;
	const led_pattern_t* pattern;
	osTimerId_t timer;
} led_service_instance_t;

typedef struct {
	led_service_instance_t instance[CONFIG_LED_INSTANCE_NUM];
} led_service_priv_t;

/**
 * @brief   Attributes structure for led service timer.
 */
const osTimerAttr_t led_service_timer_attr = {
	.name = CONFIG_LED_TIMER_NAME,
	.attr_bits = 0,
	.cb_mem = NULL,
	.cb_size = 0,
};

void led_service_timer_callback(void *argument)
{
}

static int led_service_init(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	unsigned int i;
	osStatus_t stat;

	(void)memset(priv_data, 0, sizeof(led_service_priv_t));

	for (i = 0; i < CONFIG_LED_INSTANCE_NUM; i++) {
		priv_data->instance[i].id = led_id_search_index(i);
		priv_data->instance[i].hardware = led_hardware_search_index(i);
		priv_data->instance[i].pattern = NULL;
		priv_data->instance[i].timer = osTimerNew(led_service_timer_callback, osTimerOnce, &priv_data->instance[i], &led_service_timer_attr);
		if (stat != osOK) {
			pr_error("Service <%s> create timer <%s> failed.",
				svc->name,
				osTimerGetName(priv_data->instance[i].timer));
			return -EINVAL;
		} else {
			pr_info("Service <%s> create timer <%s> succeed.",
				svc->name,
				osTimerGetName(priv_data->instance[i].timer));
		}
	}

	return 0;
}

static int led_service_deinit(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	unsigned int i;
	osStatus_t stat;

	for (i = 0; i < CONFIG_LED_INSTANCE_NUM; i++) {
		priv_data->instance[i].id = NULL;
		priv_data->instance[i].hardware = NULL;
		priv_data->instance[i].pattern = NULL;
		stat = osTimerDelete(priv_data->instance[i].timer);
		if (stat != osOK) {
			pr_error("Service <%s> delete timer <%s> failed.",
				svc->name,
				osTimerGetName(priv_data->instance[i].timer));
			return -EINVAL;
		} else {
			pr_info("Service <%s> delete timer <%s> succeed.",
				svc->name,
				osTimerGetName(priv_data->instance[i].timer));
		}
	}

	return 0;
}

static void led_service_handle_message(const message_t *message,
				       message_t *	rsp_message,
				       void *		priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	unsigned int index;
	osStatus_t stat;

	(void)priv_data;



	switch (message->id) {
	case MSG_ID_LED_START:
		osTimerStart();
		break;

	case MSG_ID_LED_STOP:
		osTimerStop();
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
