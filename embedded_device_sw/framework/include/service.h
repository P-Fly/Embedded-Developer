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

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <stddef.h>
#include "cmsis_os2.h"
#include "object.h"
#include "err.h"
#include "framework_conf.h"
#include "services_ids.h"
#include "message.h"

struct _service_t;
typedef struct _service_t service_t;

/**
 * @brief   Message structure on service definitions.
 */
typedef struct {
	const service_t *	src;
	const service_t *	dst;
	message_type_t		type;
	message_t		msg;
} service_message_t;

/**
 * @brief   Service handle definitions.
 */
typedef struct _service_t {
	const object *		owner;
	const char *		name;
	osThreadId_t		thread_id;
	osMessageQueueId_t	queue_id;
	int (*init)(const service_t *svc, void *priv);
	int (*deinit)(const service_t *svc, void *priv);
	void (*handle_message)(const message_t *message, message_t *rsp_message,
			       void *priv);
	void *			priv;
} service_t;

/**
 * @brief   Service configuration structure.
 */
typedef struct {
	osThreadAttr_t		thread_attr;
	osMessageQueueAttr_t	queue_attr;
} service_config_t;

/**
 * @brief   Service interface definitions.
 */
typedef struct {
	int (*init)(const object *obj, const service_config_t *config);
	int (*deinit)(const object *obj);
	int (*send_message)(const object *		obj,
			    const service_message_t *	service_message);
	void (*handle_message)(const object *		obj,
			       const service_message_t *service_message);
} service_intf_t;

extern const service_config_t service_config_default;
extern const service_intf_t service_intf_default;

extern int service_probe(const object *obj);
extern int service_shutdown(const object *obj);
extern const service_t *service_get_binding(const char *const name);
extern const object *service_get_owner(const service_t *svc);
extern const char *service_get_name(const service_t *svc);
extern osThreadId_t service_get_thread_id(const service_t *svc);
extern osMessageQueueId_t service_get_queue_id(const service_t *svc);
extern void *service_get_private_data(const service_t *svc);
extern int service_send_evt(const service_t *dst, const message_t *message);
extern int service_send_req(const service_t *	dst,
			    const service_t *	src,
			    const message_t *	message);
extern int service_send_rsp(const service_t *	dst,
			    const service_t *	src,
			    const message_t *	message);
extern int service_broadcast_evt(const message_t *message);

#define DECLARE_SERVICE(service_name, \
			service_label, \
			priv_data, \
			init_fn, \
			deinit_fn, \
			handle_message_fn) \
	__define_service(service_name, \
			 service_label, \
			 priv_data, \
			 &service_intf_default, \
			 &service_config_default, \
			 init_fn, \
			 deinit_fn, \
			 handle_message_fn)

#define __define_service(service_name, \
			 service_label, \
			 priv_data, \
			 intf, \
			 config, \
			 init_fn, \
			 deinit_fn, \
			 handle_message_fn) \
	static service_t __service_def_ ## service_label \
	__attribute__((used, section("module_service"))) = { \
		.owner		= NULL, \
		.name		= (service_name), \
		.thread_id	= NULL, \
		.queue_id	= NULL, \
		.init		= (init_fn), \
		.deinit		= (deinit_fn), \
		.handle_message = (handle_message_fn), \
		.priv		= (priv_data) }; \
	module_service(service_name, \
		       service_label, \
		       service_probe, \
		       service_shutdown, \
		       ((void *)intf), \
		       (&__service_def_ ## service_label), \
		       ((void *)config))

#endif /* __SERVICE_H__ */
