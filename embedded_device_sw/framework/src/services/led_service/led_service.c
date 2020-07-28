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

#if defined(CONFIG_LED_SERVICE_ENABLE)

typedef struct {

} led_service_priv_t;

static int led_service_init(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;

	(void)memset(priv_data, 0, sizeof(led_service_priv_t));

	return 0;
}

static int led_service_deinit(const service_t *svc, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;

	(void)priv_data;

	return 0;
}

static void led_service_handle_message(const message_t *message, message_t *rsp_message, void *priv)
{
	led_service_priv_t *priv_data = (led_service_priv_t *)priv;

	(void)priv_data;

	switch (message->id) {
	case MSG_ID_LED_SERVICE_QUICK_FLASH_EVT:
		break;

	case MSG_ID_LED_SERVICE_SLOW_FLASH_EVT:
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
