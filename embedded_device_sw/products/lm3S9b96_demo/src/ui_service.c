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

#if defined(CONFIG_UI_SERVICE_ENABLE)

#define ui_error   pr_error
#define ui_warning pr_warning
#define ui_info    pr_info
#define ui_debug   pr_debug

/**
 * @brief   Private structure for UI service.
 */
typedef struct {
	const service_t *owner_svc;
	const service_t *led_svc;
} ui_service_priv_t;

/**
 * @brief   Initialize the UI service.
 *
 * @param   svc Pointer to the svc handle.
 * @param   priv Pointer to the private structure.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int ui_service_init(const service_t *svc, void *priv)
{
	ui_service_priv_t *priv_data = (ui_service_priv_t *)priv;

	(void)memset(priv_data, 0, sizeof(ui_service_priv_t));

	priv_data->owner_svc = svc;

	priv_data->led_svc = service_get_binding(CONFIG_LED_SERVICE_NAME);
	if (!priv_data->led_svc) {
		ui_error("Service <%s> binding object <%s> failed.",
			svc->name,
			CONFIG_LED_SERVICE_NAME);
		return -ENODEV;
	} else
		ui_info("Service <%s> binding object <%s> succeed.",
			svc->name,
			CONFIG_LED_SERVICE_NAME);

	return 0;
}

/**
 * @brief   Deinitialize the UI service.
 *
 * @param   svc Pointer to the svc handle.
 * @param   priv Pointer to the private structure.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int ui_service_deinit(const service_t *svc, void *priv)
{
	ui_service_priv_t *priv_data = (ui_service_priv_t *)priv;

	(void)priv_data;

	return 0;
}

/**
 * @brief   Handle the UI service message.
 *
 * @param   message Pointer to the received message.
 * @param 	rsp_message Pointer to the respond message.
 * @param   priv Pointer to the private structure.
 *
 * @retval  None.
 */
static void ui_service_handle_message(const message_t *message,
				       message_t *	rsp_message,
				       void *		priv)
{
	ui_service_priv_t *priv_data = (ui_service_priv_t *)priv;
	message_t send_message;
	int ret;

	switch (message->id) {
	case MSG_ID_SYS_STARTUP_COMPLETED:

		send_message.id = MSG_ID_LED_START;
		send_message.param0 = 0;
		send_message.param1 = LED_PATTERN_QUICK_FLASH;
		send_message.ptr = NULL;

		ret = service_send_evt(priv_data->led_svc, &send_message);
		if (ret)
			ui_error("Send event 0x%x to <%s> failed.", send_message.id, priv_data->led_svc->name);
		else
			ui_info("Send event 0x%x to <%s> succeed.", send_message.id, priv_data->led_svc->name);

		send_message.id = MSG_ID_LED_START;
		send_message.param0 = 1;
		send_message.param1 = LED_PATTERN_SLOW_FLASH;
		send_message.ptr = NULL;

		ret = service_send_evt(priv_data->led_svc, &send_message);
		if (ret)
			ui_error("Send event 0x%x to <%s> failed.", send_message.id, priv_data->led_svc->name);
		else
			ui_info("Send event 0x%x to <%s> succeed.", send_message.id, priv_data->led_svc->name);

		break;

	default:
		break;
	}
}

static ui_service_priv_t ui_service_priv;

DECLARE_SERVICE(CONFIG_UI_SERVICE_NAME,
		CONFIG_UI_SERVICE_LABEL,
		&ui_service_priv,
		ui_service_init,
		ui_service_deinit,
		ui_service_handle_message);

#endif
