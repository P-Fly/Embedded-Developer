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

typedef struct {
	const object* gpio;
	osTimerId_t timer;
	unsigned int cycle_idx;
} led_service_runtime_t;

typedef struct {
	const led_id_t* led_id;
	const led_hardware_t* hardware;
	const led_pattern_t* pattern;
	led_service_runtime_t runtime;
} led_service_instance_t;

typedef struct {
	led_service_instance_t instance[CONFIG_LED_INSTANCE_NUM];
} led_service_priv_t;

led_service_instance_t* led_service_instance_search_id(led_service_priv_t *priv_data, unsigned int id)
{
	int i;

	for (i = 0; i < CONFIG_LED_INSTANCE_NUM; i++)
		if (priv_data->instance[i].led_id->id == id)
			return &priv_data->instance[i];

	return NULL;
}

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
	led_service_instance_t* instance = (led_service_instance_t*)argument;
	osStatus_t stat;
	int ret;

	instance->runtime.cycle_idx++;

	if (instance->runtime.cycle_idx >= instance->pattern->cycle_total)
		if (!instance->pattern->periodic)
			;
		else
			instance->runtime.cycle_idx = 0;

	if (instance->pattern->cycle[instance->runtime.cycle_idx].onoff)
	{
		ret = gpio_write(instance->runtime.gpio, instance->hardware->pin, instance->hardware->on);
		pr_info("gpio_write ret %d.", ret);
	}
	else
	{
		ret = gpio_write(instance->runtime.gpio, instance->hardware->pin, instance->hardware->off);
		pr_info("gpio_write ret %d.", ret);
	}

	stat = osTimerStart(instance->runtime.timer, instance->pattern->cycle[instance->runtime.cycle_idx].time_ms * osKernelGetTickFreq() / 1000);
	pr_info("osTimerStart %p %d = %d.", instance->runtime.timer, instance->pattern->cycle[instance->runtime.cycle_idx].time_ms, stat);
}

static int led_service_init(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	int ret;
	unsigned int i;

	(void)memset(priv_data, 0, sizeof(led_service_priv_t));

	for (i = 0; i < CONFIG_LED_INSTANCE_NUM; i++) {
		priv_data->instance[i].led_id = led_id_search_index(i);
		priv_data->instance[i].hardware = led_hardware_search_index(i);
		priv_data->instance[i].pattern = NULL;
		priv_data->instance[i].runtime.cycle_idx = 0;
		priv_data->instance[i].runtime.timer = osTimerNew(led_service_timer_callback, osTimerOnce, &priv_data->instance[i], &led_service_timer_attr);
		if (!priv_data->instance[i].runtime.timer) {
			pr_error("Service <%s> create timer <%s> failed.",
				svc->name,
				osTimerGetName(priv_data->instance[i].runtime.timer));
			return -EINVAL;
		} else
			pr_info("Service <%s> create timer <%s> succeed.",
				svc->name,
				osTimerGetName(priv_data->instance[i].runtime.timer));

		priv_data->instance[i].runtime.gpio = object_get_binding(priv_data->instance[i].hardware->port);
		if (!priv_data->instance[i].runtime.gpio)
		{
			pr_error("Service <%s> binding object <%s> failed.",
				svc->name,
				priv_data->instance[i].hardware->port);
			return -ENODEV;
		} else
			pr_info("Service <%s> binding object <%s> succeed.",
				svc->name,
				priv_data->instance[i].hardware->port);

		ret = gpio_configure(priv_data->instance[i].runtime.gpio, priv_data->instance[i].hardware->pin, &priv_data->instance[i].hardware->config);
		pr_info("gpio_configure ret %d.", ret);

	}

	return 0;
}

static int led_service_deinit(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	unsigned int i;
	osStatus_t stat;

	for (i = 0; i < CONFIG_LED_INSTANCE_NUM; i++) {
		priv_data->instance[i].led_id = NULL;
		priv_data->instance[i].hardware = NULL;
		priv_data->instance[i].pattern = NULL;
		stat = osTimerDelete(priv_data->instance[i].runtime.timer);
		if (stat != osOK) {
			pr_error("Service <%s> delete timer <%s> failed.",
				svc->name,
				osTimerGetName(priv_data->instance[i].runtime.timer));
			return -EINVAL;
		} else {
			pr_info("Service <%s> delete timer <%s> succeed.",
				svc->name,
				osTimerGetName(priv_data->instance[i].runtime.timer));
		}
	}

	return 0;
}

static void led_service_handle_message(const message_t *message,
				       message_t *	rsp_message,
				       void *		priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	led_service_instance_t* instance;
	unsigned int id;
	led_pattern_id_t pattern_id;
	osStatus_t stat;
	int ret;

	switch (message->id) {
	case MSG_ID_LED_START:
		id = message->param0;
		pattern_id = (led_pattern_id_t)message->param1;

		instance = led_service_instance_search_id(priv_data, id);
		if (!instance)
			break;

		instance->pattern = led_pattern_search_id(pattern_id);
		if (!instance->pattern)
			break;

		stat = osTimerStop(instance->runtime.timer);
		pr_error("osTimerStop %d.", stat);

		instance->runtime.cycle_idx = 0;

		if (instance->pattern->cycle[instance->runtime.cycle_idx].onoff)
		{
			ret = gpio_write(instance->runtime.gpio, instance->hardware->pin, instance->hardware->on);
			pr_info("gpio_write ret %d.", ret);
		}
		else
		{
			ret = gpio_write(instance->runtime.gpio, instance->hardware->pin, instance->hardware->off);\
			pr_info("gpio_write ret %d.", ret);
		}
		stat = osTimerStart(instance->runtime.timer, instance->pattern->cycle[instance->runtime.cycle_idx].time_ms * osKernelGetTickFreq() / 1000);
		pr_error("osTimerStart %p %d = %d.", instance->runtime.timer, instance->pattern->cycle[instance->runtime.cycle_idx].time_ms, stat);

		break;

	case MSG_ID_LED_STOP:
		osTimerStop(instance->runtime.timer);
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
