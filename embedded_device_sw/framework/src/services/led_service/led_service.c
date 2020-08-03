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
#include "led_id.h"
#include "led_hardware.h"
#include "led_pattern.h"
#include "log.h"
#include "drv_gpio.h"

#if defined(CONFIG_LED_SERVICE_ENABLE)

#define led_error   pr_error
#define led_warning pr_warning
#define led_info    pr_info
#define led_debug   pr_debug

/**
 * @brief   Led runtime structure definitions.
 */
typedef struct {
	const object *	gpio;
	osTimerId_t	timer;
	unsigned int	cycle_idx;
} led_service_runtime_t;

/**
 * @brief   Led instance structure definitions.
 */
typedef struct {
	const led_id_t *	led_id;
	const led_hardware_t *	hardware;
	const led_pattern_t *	pattern;
	led_service_runtime_t	runtime;
} led_service_instance_t;

/**
 * @brief   Private structure for LED service.
 */
typedef struct {
	const service_t *	owner_svc;
	led_service_instance_t	instance[CONFIG_LED_INSTANCE_NUM];
} led_service_priv_t;

/**
 * @brief   Search for the instance by led_id.
 *
 * @param   priv Pointer to the private structure.
 * @param   id The id on led_id structure.
 *
 * @retval  Instance handle for reference or NULL in case of error.
 */
static led_service_instance_t *led_instance_search_by_led_id(
	led_service_priv_t *	priv_data,
	unsigned int		id)
{
	int i;

	if (!priv_data->instance[i].led_id)
		return NULL;

	for (i = 0; i < CONFIG_LED_INSTANCE_NUM; i++)
		if (priv_data->instance[i].led_id->id == id)
			return &priv_data->instance[i];

	return NULL;
}

/**
 * @brief   Attributes structure for led service timer.
 */
const osTimerAttr_t led_service_timer_attr = {
	.name		= CONFIG_LED_TIMER_NAME,
	.attr_bits	= 0,
	.cb_mem		= NULL,
	.cb_size	= 0,
};

/**
 * @brief   Timer callback function.
 */
static void led_service_timer_callback(void *argument)
{
	led_service_instance_t *instance = (led_service_instance_t *)argument;
	gpio_pin_level_t level;
	osStatus_t stat;
	int ret;

	instance->runtime.cycle_idx++;

	if (instance->runtime.cycle_idx >= instance->pattern->cycle_total)
		if (!instance->pattern->periodic) {
			led_debug("End Timer <%s> succeed, led %d <%s>.",
				  osTimerGetName(instance->runtime.timer),
				  instance->led_id->id,
				  instance->led_id->name);
			return;
		} else {
			instance->runtime.cycle_idx = 0;
		}

	if (instance->pattern->cycle[instance->runtime.cycle_idx].onoff)
		level = instance->hardware->on;
	else
		level = instance->hardware->off;

	ret =
		gpio_write(instance->runtime.gpio,
			   instance->hardware->pin,
			   level);
	if (ret)
		led_error(
			"Timer <%s> write gpio <%s> pin %d level %d failed, ret %d.",
			osTimerGetName(instance->runtime.timer),
			instance->runtime.gpio->name,
			instance->hardware->pin,
			level,
			ret);
	else
		led_debug("Timer <%s> write gpio <%s> pin %d level %d succeed.",
			  osTimerGetName(instance->runtime.timer),
			  instance->runtime.gpio->name,
			  instance->hardware->pin,
			  level);

	stat =
		osTimerStart(instance->runtime.timer,
			     instance->pattern->cycle[instance->runtime.
						      cycle_idx].time_ms * osKernelGetTickFreq() /
			     1000);
	if (stat != osOK)
		led_error(
			"Start Timer <%s> period %d failed, led %d <%s>, stat = %d.",
			osTimerGetName(
				instance->runtime.timer),
			instance->pattern->cycle[instance->runtime.cycle_idx].time_ms,
			instance->led_id->id,
			instance->led_id->name,
			stat);
	else
		led_debug("Start Timer <%s> period %d succeed, led %d <%s>",
			  osTimerGetName(
				  instance->runtime.timer),
			  instance->pattern->cycle[instance->runtime.cycle_idx].time_ms,
			  instance->led_id->id,
			  instance->led_id->name);
}

/**
 * @brief   Initialize the LED service.
 *
 * @param   svc Pointer to the svc handle.
 * @param   priv Pointer to the private structure.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int led_service_init(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	int ret;
	unsigned int i;

	(void)memset(priv_data, 0, sizeof(led_service_priv_t));

	priv_data->owner_svc = svc;

	for (i = 0; i < CONFIG_LED_INSTANCE_NUM; i++) {
		priv_data->instance[i].led_id = led_id_search_by_index(i);
		priv_data->instance[i].hardware =
			led_hardware_search_by_index(i);
		priv_data->instance[i].pattern = NULL;
		priv_data->instance[i].runtime.cycle_idx = 0;
		priv_data->instance[i].runtime.timer = osTimerNew(
			led_service_timer_callback,
			osTimerOnce,
			&priv_data->instance[i],
			&led_service_timer_attr);
		if (!priv_data->instance[i].runtime.timer) {
			led_error("Service <%s> create timer <%s> failed.",
				  svc->name,
				  osTimerGetName(priv_data->instance[i].runtime.
						 timer));
			return -EINVAL;
		} else {
			led_info("Service <%s> create timer <%s> succeed.",
				 svc->name,
				 osTimerGetName(priv_data->instance[i].runtime.
						timer));
		}

		priv_data->instance[i].runtime.gpio = object_get_binding(
			priv_data->instance[i].hardware->port);
		if (!priv_data->instance[i].runtime.gpio) {
			led_error("Service <%s> binding object <%s> failed.",
				  svc->name,
				  priv_data->instance[i].hardware->port);
			return -ENODEV;
		} else {
			led_info("Service <%s> binding object <%s> succeed.",
				 svc->name,
				 priv_data->instance[i].hardware->port);
		}

		ret = gpio_configure(priv_data->instance[i].runtime.gpio,
				     priv_data->instance[i].hardware->pin,
				     &priv_data->instance[i].hardware->config);
		if (ret) {
			led_error(
				"Service <%s> configure gpio <%s> pin %d failed, ret %d.",
				svc->name,
				priv_data->instance[i].runtime.gpio->name,
				priv_data->instance[i].hardware->pin,
				ret);
			return ret;
		} else {
			led_info(
				"Service <%s> configure gpio <%s> pin %d succeed.",
				svc->name,
				priv_data->instance[i].runtime.gpio->name,
				priv_data->instance[i].hardware->pin);
		}
	}

	return 0;
}

/**
 * @brief   Deinitialize the LED service.
 *
 * @param   svc Pointer to the svc handle.
 * @param   priv Pointer to the private structure.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
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
			led_error("Service <%s> delete timer <%s> failed.",
				  svc->name,
				  osTimerGetName(priv_data->instance[i].runtime.
						 timer));
			return -EINVAL;
		} else {
			led_info("Service <%s> delete timer <%s> succeed.",
				 svc->name,
				 osTimerGetName(priv_data->instance[i].runtime.
						timer));
		}
	}

	return 0;
}

/**
 * @brief   Handle the LED service message.
 *
 * @param   message Pointer to the received message.
 * @param 	rsp_message Pointer to the respond message.
 * @param   priv Pointer to the private structure.
 *
 * @retval  None.
 */
static void led_service_handle_message(const message_t *message,
				       message_t *	rsp_message,
				       void *		priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;
	led_service_instance_t *instance;
	unsigned int id;
	led_pattern_id_t pattern_id;
	gpio_pin_level_t level;
	osStatus_t stat;
	int ret;

	switch (message->id) {
	case MSG_ID_LED_START:

		id = message->param0;
		pattern_id = (led_pattern_id_t)message->param1;

		led_info("Service <%s> received event 0x%x id %d pattern %d.",
			 priv_data->owner_svc->name,
			 MSG_ID_LED_START,
			 id,
			 pattern_id);

		instance = led_instance_search_by_led_id(priv_data, id);
		if (!instance) {
			led_error("Service <%s> search led_id %d failed.",
				  priv_data->owner_svc->name,
				  id);
			break;
		} else {
			led_debug("Service <%s> search led_id %d succeed.",
				  priv_data->owner_svc->name,
				  id);
		}

		instance->pattern = led_pattern_search_id(pattern_id);
		if (!instance->pattern) {
			led_error("Service <%s> search pattern %d failed.",
				  priv_data->owner_svc->name,
				  pattern_id);
			break;
		} else {
			led_debug("Service <%s> search pattern %d succeed.",
				  priv_data->owner_svc->name,
				  pattern_id);
		}

		if (!instance->hardware) {
			led_error("Service <%s> hardware is NULL.",
				  priv_data->owner_svc->name);
			break;
		}

		(void)osTimerStop(instance->runtime.timer);

		instance->runtime.cycle_idx = 0;

		if (instance->pattern->cycle[instance->runtime.cycle_idx].onoff)
			level = instance->hardware->on;
		else
			level = instance->hardware->off;

		ret = gpio_write(instance->runtime.gpio,
				 instance->hardware->pin,
				 level);
		if (ret) {
			led_error(
				"Service <%s> write gpio <%s> pin %d level %d failed, ret %d.",
				priv_data->owner_svc->name,
				instance->runtime.gpio->name,
				instance->hardware->pin,
				level,
				ret);
			break;
		} else {
			led_debug(
				"Service <%s> write gpio <%s> pin %d level %d succeed.",
				priv_data->owner_svc->name,
				instance->runtime.gpio->name,
				instance->hardware->pin,
				level);
		}

		stat = osTimerStart(instance->runtime.timer,
				    instance->pattern->cycle[instance->runtime.
							     cycle_idx].time_ms * osKernelGetTickFreq() /
				    1000);
		if (stat != osOK) {
			led_error(
				"Start Timer <%s> period %d failed, stat = %d.",
				osTimerGetName(instance->runtime.timer),
				instance->pattern->cycle[instance->runtime.
							 cycle_idx].time_ms,
				stat);
			break;
		} else {
			led_debug("Start Timer <%s> period %d succeed.",
				  osTimerGetName(instance->runtime.timer),
				  instance->pattern->cycle[instance->runtime.
							   cycle_idx].time_ms);
		}

		break;

	case MSG_ID_LED_STOP:

		id = message->param0;

		led_info("Service <%s> received event 0x%x id %d.",
			 priv_data->owner_svc->name,
			 MSG_ID_LED_STOP,
			 id);

		instance = led_instance_search_by_led_id(priv_data, id);
		if (!instance) {
			led_error("Service <%s> search led_id %d failed.",
				  priv_data->owner_svc->name,
				  id);
			break;
		} else {
			led_debug("Service <%s> search led_id %d succeed.",
				  priv_data->owner_svc->name,
				  id);
		}

		stat = osTimerStop(instance->runtime.timer);
		if (stat != osOK) {
			led_error("Stop Timer <%s> failed, stat = %d.",
				  osTimerGetName(instance->runtime.timer),
				  stat);
			break;
		} else {
			led_debug("Stop Timer <%s> succeed.",
				  osTimerGetName(instance->runtime.timer));
		}

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
