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

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stddef.h>
#include "cmsis_os2.h"
#include "object.h"
#include "err.h"
#include "framework_conf.h"

/**
 * @brief	Message types definition.
 */
typedef enum {
	MSG_TYPE_REQ,
	MSG_TYPE_RSP,
	MSG_TYPE_EVT,
	MSG_TYPE_INT
} message_type_t;

/**
 * @brief	Message structure definitions.
 */
typedef struct {
	unsigned int	id;
	unsigned int	param0;
	unsigned int	param1;
	void *		ptr;
} message_t;

#endif /* __MESSAGE_H__ */
