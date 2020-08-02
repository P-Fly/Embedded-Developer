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

#ifndef __SERVICES_IDS_H__
#define __SERVICES_IDS_H__

/**
 * @brief   Service message base definitions.
 */
#define MSG_TYPE_EVT_BASE               0x40000000
#define MSG_TYPE_REQ_BASE               0x80000000
#define MSG_TYPE_RSP_BASE               0xC0000000

#define MSG_ID_ADC_SERVICE_BASE         0x00010000
#define MSG_ID_LED_SERVICE_BASE         0x00020000
#define MSG_ID_BUTTON_SERVICE_BASE      0x00030000
#define MSG_ID_TUNIT_SERVICE_BASE       0x00F00000
#define MSG_ID_SYS_SERVICE_BASE         0x00FF0000

/** Message ID for LED service */

/**
 * @brief           Start a LED pattern.
 *
 * @message.id      MSG_ID_LED_START
 * @message.param0  LED ID.
 * @message.param1  Pattern ID.
 * @message.ptr     None.
 */
#define MSG_ID_LED_START (MSG_TYPE_EVT_BASE | \
					MSG_ID_LED_SERVICE_BASE | 0x0001)

/**
 * @brief           Stop a LED pattern.
 *
 * @message.id      MSG_ID_LED_STOP
 * @message.param0  LED ID.
 * @message.param1  None.
 * @message.ptr     None.
 */
#define MSG_ID_LED_STOP (MSG_TYPE_EVT_BASE | \
					MSG_ID_LED_SERVICE_BASE | 0x0002)

/**
 * @brief           Notify LED pattern is completed.
 *
 * @message.id      MSG_ID_LED_PATTERN_COMPLETED
 * @message.param0  LED ID.
 * @message.param1  Pattern ID.
 * @message.ptr     None.
 */
#define MSG_ID_LED_PATTERN_COMPLETED (MSG_TYPE_EVT_BASE | \
					MSG_ID_LED_SERVICE_BASE | 0x0003)

/** Message ID for system service */

/**
 * @brief           Notify system startup is completed.
 *
 * @message.id      MSG_ID_SYS_STARTUP_COMPLETED
 * @message.param0  None.
 * @message.param1  None.
 * @message.ptr     None.
 */
#define MSG_ID_SYS_STARTUP_COMPLETED (MSG_TYPE_EVT_BASE | \
					MSG_ID_SYS_SERVICE_BASE | 0x0001)

#endif /* __SERVICES_IDS_H__ */
