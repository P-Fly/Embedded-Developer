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

#define MSG_TYPE_EVT_BASE               0x40000000
#define MSG_TYPE_REQ_BASE               0x80000000
#define MSG_TYPE_RSP_BASE               0xC0000000

#define MSG_ID_ADC_SERVICE_BASE         0x00010000
#define MSG_ID_LED_SERVICE_BASE         0x00020000
#define MSG_ID_BUTTON_SERVICE_BASE      0x00030000
#define MSG_ID_TUNIT_SERVICE_BASE    	0x00FF0000

#endif /* __SERVICES_IDS_H__ */
