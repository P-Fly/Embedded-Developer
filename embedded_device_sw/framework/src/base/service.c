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
#include "cmsis_os2.h"
#include "object.h"
#include "err.h"
#include "log.h"
#include "message.h"
#include "service.h"

static int service_init_default(const object *		obj,
				const service_config_t *config);
static int service_deinit_default(const object *obj);
static int service_send_message_default(const object *			obj,
					const service_message_t *	service_message);
static void service_handle_message_default(const object *		obj,
					   const service_message_t *	service_message);
static void service_routine_thread(void *argument);

const service_config_t service_config_default = {
	.thread_attr		=
	{
		.name		= CONFIG_SERVICE_DEFAULT_THREAD_NAME,
		.attr_bits	= osThreadDetached,
		.cb_mem		= NULL,
		.cb_size	= 0,
		.stack_mem	= NULL,
		.stack_size	= CONFIG_SERVICE_DEFAULT_THREAD_STACK_SIZE,
		.priority	= CONFIG_SERVICE_DEFAULT_THREAD_PRIORITY,
	},

	.queue_attr		=
	{
		.name		= CONFIG_SERVICE_DEFAULT_QUEUE_NAME,
		.attr_bits	= 0,
		.cb_mem		= NULL,
		.cb_size	= 0,
		.mq_mem		= NULL,
		.mq_size	= 0,
	}
};

const service_intf_t service_intf_default = {
	.init		= service_init_default,
	.deinit		= service_deinit_default,
	.send_message	= service_send_message_default,
	.handle_message = service_handle_message_default,
};

/**
 * @brief   Initialize the service instance.
 *
 * @param   obj Pointer to the service object handle.
 * @param   config Pointer to the configuration space.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int service_init_default(const object *		obj,
				const service_config_t *config)
{
	service_t *svc = (service_t *)obj->object_data;
	int ret;

	svc->thread_id = osThreadNew(service_routine_thread,
				     (void *)obj,
				     &config->thread_attr);
	if (!svc->thread_id) {
		pr_error("Service <%s> create thread <%s> failed.",
			 svc->name,
			 config->thread_attr.name);
		return -EINVAL;
	} else {
		pr_info("Service <%s> create thread <%s> succeed.",
			svc->name,
			config->thread_attr.name);
	}

	svc->queue_id = osMessageQueueNew(CONFIG_SERVICE_DEFAULT_QUEUE_LENGTH,
					  sizeof(service_message_t),
					  &config->queue_attr);
	if (!svc->queue_id) {
		pr_error("Service <%s> create message queue <%s> failed.",
			 svc->name,
			 config->queue_attr.name);
		return -EINVAL;
	} else {
		pr_info("Service <%s> create message queue <%s> succeed.",
			svc->name,
			config->queue_attr.name);
	}

	if (svc->init) {
		ret = svc->init(svc, svc->priv);
		if (ret)
			return ret;
	}

	pr_info("Service <%s> initialize succeed.", svc->name);

	return 0;
}

/**
 * @brief   Deinitialize the service instance.
 *
 * @param   obj Pointer to the service object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int service_deinit_default(const object *obj)
{
	service_t *svc = (service_t *)obj->object_data;
	osStatus_t stat;

	if (svc->thread_id) {
		stat = osThreadTerminate(svc->thread_id);
		if (stat != osOK)
			pr_error("Service <%s> terminate thread <%s> failed.",
				 svc->name,
				 osThreadGetName(svc->thread_id));
		else
			pr_info("Service <%s> terminate thread <%s> succeed.",
				svc->name,
				osThreadGetName(svc->thread_id));
	}

	if (svc->queue_id) {
		stat = osMessageQueueDelete(svc->queue_id);
		if (stat != osOK)
			pr_error("Service <%s> delete message queue failed.",
				 svc->name);
		else
			pr_info("Service <%s> delete message queue succeed.",
				svc->name);
	}

	if (svc->deinit)
		svc->deinit(svc, svc->priv);

	pr_info("Service <%s> deinitialize succeed.", svc->name);

	return 0;
}

/**
 * @brief   Sends a message to service.
 *
 * @param   obj Pointer to the service object handle.
 * @param   service_message Service message to send.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int service_send_message_default(const object *			obj,
					const service_message_t *	service_message)
{
	service_t *svc = (service_t *)obj->object_data;
	osMessageQueueId_t queue_id = svc->queue_id;
	osStatus_t stat;

	stat =
		osMessageQueuePut(queue_id,
				  service_message,
				  sizeof(service_message_t),
				  CONFIG_MSG_SEND_BLOCK_TIMEOUT_MS * osKernelGetTickFreq() /
				  1000);
	if (stat != osOK)
		return -EPIPE;

	return 0;
}

/**
 * @brief   Handle service message queue.
 *
 * @param   obj Pointer to the service object handle.
 * @param   service_message Pointer to the service message structure.
 *
 * @retval  None.
 */
static void service_handle_message_default(const object *		obj,
					   const service_message_t *	service_message)
{
	service_t *svc = (service_t *)obj->object_data;
	message_t rsp_message;
	int ret;

	if (svc->handle_message)
		svc->handle_message(&service_message->msg,
				    &rsp_message,
				    svc->priv);

	if (service_message->type == MSG_TYPE_REQ) {
		ret = service_send_rsp(service_message->src,
				       service_message->dst,
				       &rsp_message);
		if (ret) {
			pr_error(
				"Service <%s> responds to service <%s> messsage 0x%x failed, ret %d.",
				service_message->dst->name,
				service_message->src->name,
				service_message->msg.id,
				ret);
		}
	}
}

/**
 * @brief   service routine thread, processing message loops.
 *
 * @param   argument Pointer to the service object handle.
 *
 * @retval  None.
 */
static void service_routine_thread(void *argument)
{
	object *obj = (object *)argument;
	service_t *svc = (service_t *)obj->object_data;
	service_intf_t *intf = (service_intf_t *)obj->object_intf;
	service_message_t service_message;
	osStatus_t stat;

	while (1) {
		stat = osMessageQueueGet(svc->queue_id,
					 &service_message,
					 NULL,
					 osWaitForever);
		if (stat != osOK)
			continue;

		if (intf->handle_message)
			intf->handle_message(obj, &service_message);
	}
}

/**
 * @brief   Probe the service object.
 *
 * @param   obj Pointer to the service object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
int service_probe(const object *obj)
{
	service_t *svc = (service_t *)obj->object_data;
	service_config_t *config = (service_config_t *)obj->object_config;
	service_intf_t *intf = (service_intf_t *)obj->object_intf;
	int ret;

	svc->owner = obj;

	if (intf->init) {
		ret = intf->init(obj, config);
		if (ret)
			return ret;
	}

	pr_info("Object <%s> probe succeed.", obj->name);

	return 0;
}

/**
 * @brief   Remove the service object.
 *
 * @param   obj Pointer to the service object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
int service_shutdown(const object *obj)
{
	service_intf_t *intf = (service_intf_t *)obj->object_intf;
	int ret;

	if (intf->deinit) {
		ret = intf->deinit(obj);
		if (ret)
			return ret;
	}

	pr_info("Object <%s> shutdown succeed.", obj->name);

	return 0;
}

extern service_t module_service$$Base[];
extern service_t module_service$$Limit[];

/**
 * @brief   Get the service handle.
 *
 * @param   name Service name.
 *
 * @retval  Service handle for reference or NULL in case of error.
 */
const service_t *service_get_binding(const char *const name)
{
	const service_t *start = module_service$$Base;
	const service_t *end = module_service$$Limit;
	const service_t *svc;

	for (svc = start; svc < end; svc++)
		if (svc && !strcmp(name, svc->name))
			return svc;

	return NULL;
}

/**
 * @brief   Get the owner for service.
 *
 * @param   svc Pointer to the service handle.
 *
 * @retval  Returns the owner.
 */
const object *service_get_owner(const service_t *svc)
{
	return svc->owner;
}

/**
 * @brief   Get the name for service.
 *
 * @param   svc Pointer to the service handle.
 *
 * @retval  Returns the name.
 */
const char *service_get_name(const service_t *svc)
{
	return svc->name;
}

/**
 * @brief   Get the thread ID for service.
 *
 * @param   svc Pointer to the service handle.
 *
 * @retval  Returns the thread ID.
 */
osThreadId_t service_get_thread_id(const service_t *svc)
{
	return svc->thread_id;
}

/**
 * @brief   Get the queue ID for service.
 *
 * @param   svc Pointer to the service handle.
 *
 * @retval  Returns the queue ID.
 */
osMessageQueueId_t service_get_queue_id(const service_t *svc)
{
	return svc->queue_id;
}

/**
 * @brief   Get the private data for service.
 *
 * @param   svc Pointer to the service handle.
 *
 * @retval  Returns the private data.
 */
void *service_get_private_data(const service_t *svc)
{
	return svc->priv;
}

/**
 * @brief   Sends a message to service.
 *
 * @param   obj Pointer to the service object handle.
 * @param   service_message Service message to send.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int service_send_message(const object *			obj,
				const service_message_t *	service_message)
{
	service_intf_t *intf = (service_intf_t *)obj->object_intf;

	if (!obj)
		return -EINVAL;

	if ((intf == NULL)
	    || (intf->send_message == NULL))
		return -ENOSUPPORT;

	return intf->send_message(obj, service_message);
}

/**
 * @brief   Sends a event message to service.
 *
 * @param   dst Pointer to the destination service handle.
 * @param   message Message structure to send.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
int service_send_evt(const service_t *dst, const message_t *message)
{
	service_message_t service_message;
	const object *obj;

	if (!dst)
		return -EINVAL;

	if (!message)
		return -EINVAL;

	obj = service_get_owner(dst);
	if (!obj)
		return -EINVAL;

	service_message.dst = dst;
	service_message.src = NULL;
	service_message.type = MSG_TYPE_EVT;
	memcpy(&service_message.msg, message, sizeof(message_t));

	//pr_info("Send event 0x%x to service <%s>.",
	//	message->id,
	//	dst->name);

	return service_send_message(obj, &service_message);
}

/**
 * @brief   Sends a request message to service.
 *
 * @param   dst Pointer to the destination service handle.
 * @param   src Pointer to the source service handle.
 * @param   message Message structure to send.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
int service_send_req(const service_t *	dst,
		     const service_t *	src,
		     const message_t *	message)
{
	service_message_t service_message;
	const object *obj;

	if (!dst)
		return -EINVAL;

	if (!src)
		return -EINVAL;

	if (!message)
		return -EINVAL;

	obj = service_get_owner(dst);
	if (!obj)
		return -EINVAL;

	service_message.dst = dst;
	service_message.src = src;
	service_message.type = MSG_TYPE_REQ;
	memcpy(&service_message.msg, message, sizeof(message_t));

	//pr_info("Send request 0x%x from service <%s> to service <%s>.",
	//	message->id,
	//	src->name,
	//	dst->name);

	return service_send_message(obj, &service_message);
}

/**
 * @brief	Sends a respond message to service.
 *
 * @param   dst Pointer to the destination service handle.
 * @param   src Pointer to the source service handle.
 * @param   message Message structure to send.
 *
 * @retval	Returns 0 on success, negative error code otherwise.
 */
int service_send_rsp(const service_t *	dst,
		     const service_t *	src,
		     const message_t *	message)
{
	service_message_t service_message;
	const object *obj;

	if (!dst)
		return -EINVAL;

	if (!src)
		return -EINVAL;

	if (!message)
		return -EINVAL;

	obj = service_get_owner(dst);
	if (!obj)
		return -EINVAL;

	service_message.dst = dst;
	service_message.src = src;
	service_message.type = MSG_TYPE_RSP;
	memcpy(&service_message.msg, message, sizeof(message_t));

	//pr_info("Send respond 0x%x from service <%s> to service <%s>.",
	//		message->id,
	//		src->name,
	//		dst->name);

	return service_send_message(obj, &service_message);
}

/**
 * @brief	Broadcast event messages to all services.
 *
 * @param   message Message structure to send.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
int service_broadcast_evt(const message_t *message)
{
	const service_t *start = module_service$$Base;
	const service_t *end = module_service$$Limit;
	const service_t *svc;

	if (!message)
		return -EINVAL;

	//pr_info("Broadcast event 0x%x.", message->id);

	for (svc = start; svc < end; svc++)
		if (svc)
			(void)service_send_evt(svc, message);

	return 0;
}
