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
#include "tunit.h"

#ifdef CONFIG_TUNIT_SERVICE_SUIT_NAME

#define DEF_MAX_MSG_BUFF_NUM 100

#define DEF_MSG_SEND_PARAM_0 0xAA000000
#define DEF_MSG_SEND_PARAM_1 0x55000000
#define DEF_MSG_REQ_PARAM_0 0x00AA0000
#define DEF_MSG_REQ_PARAM_1 0x0000AA00
#define DEF_MSG_RSP_PARAM_0 0x00550000
#define DEF_MSG_RSP_PARAM_1 0x00005500

#define MSG_ID_SERVICE_INIT_EVT         (MSG_TYPE_EVT_BASE | \
					 MSG_ID_TUNIT_SERVICE_BASE | 0x01)
#define MSG_ID_SERVICE_START_EVT_TEST   (MSG_TYPE_EVT_BASE | \
					 MSG_ID_TUNIT_SERVICE_BASE | 0x02)
#define MSG_ID_SERVICE_START_REQ_TEST   (MSG_TYPE_EVT_BASE | \
					 MSG_ID_TUNIT_SERVICE_BASE | 0x03)
#define MSG_ID_SERVICE_SEND_REQ         (MSG_TYPE_EVT_BASE | \
					 MSG_ID_TUNIT_SERVICE_BASE | 0x04)
#define MSG_ID_SERVICE_DATA_EVT         (MSG_TYPE_EVT_BASE | \
					 MSG_ID_TUNIT_SERVICE_BASE | 0x05)
#define MSG_ID_SERVICE_DATA_REQ         (MSG_TYPE_REQ_BASE | \
					 MSG_ID_TUNIT_SERVICE_BASE | 0x05)
#define MSG_ID_SERVICE_DATA_RSP         (MSG_TYPE_RSP_BASE | \
					 MSG_ID_TUNIT_SERVICE_BASE | 0x05)
#define MSG_ID_SERVICE_DATA_BROADCAST   (MSG_TYPE_EVT_BASE | \
					 MSG_ID_TUNIT_SERVICE_BASE | 0x06)

typedef struct {
	message_t	rcvd_message[DEF_MAX_MSG_BUFF_NUM];
	int		rcvd_num;
} tcase_service_foo_priv_t;

static int tcase_service_foo_init(const service_t *svc, void *priv)
{
	tcase_service_foo_priv_t *priv_data = (tcase_service_foo_priv_t *)priv;

	(void)memset(priv_data, 0, sizeof(tcase_service_foo_priv_t));

	return 0;
}

static int tcase_service_foo_deinit(const service_t *svc, void *priv)
{
	return 0;
}

static void tcase_service_foo_handle_message(const message_t *	message,
					     message_t *	rsp_message,
					     void *		priv)
{
	tcase_service_foo_priv_t *priv_data = (tcase_service_foo_priv_t *)priv;

	switch (message->id) {
	case MSG_ID_SERVICE_INIT_EVT:

		(void)memset(priv_data->rcvd_message, 0, DEF_MAX_MSG_BUFF_NUM);
		priv_data->rcvd_num = 0;

		break;

	case MSG_ID_SERVICE_DATA_EVT:

		if (priv_data->rcvd_num < DEF_MAX_MSG_BUFF_NUM) {
			priv_data->rcvd_message[priv_data->rcvd_num].id =
				message->id;
			priv_data->rcvd_message[priv_data->rcvd_num].param0 =
				message->param0;
			priv_data->rcvd_message[priv_data->rcvd_num].param1 =
				message->param1;
			priv_data->rcvd_message[priv_data->rcvd_num].ptr =
				message->ptr;
		}

		priv_data->rcvd_num++;

		break;

	case MSG_ID_SERVICE_DATA_REQ:

		if (priv_data->rcvd_num < DEF_MAX_MSG_BUFF_NUM) {
			priv_data->rcvd_message[priv_data->rcvd_num].id =
				message->id;
			priv_data->rcvd_message[priv_data->rcvd_num].param0 =
				message->param0;
			priv_data->rcvd_message[priv_data->rcvd_num].param1 =
				message->param1;
			priv_data->rcvd_message[priv_data->rcvd_num].ptr =
				message->ptr;
		}

		priv_data->rcvd_num++;

		rsp_message->id = MSG_ID_SERVICE_DATA_RSP;
		rsp_message->param0 = DEF_MSG_RSP_PARAM_0;
		rsp_message->param1 = DEF_MSG_RSP_PARAM_1;
		rsp_message->ptr = NULL;

		break;

	case MSG_ID_SERVICE_DATA_BROADCAST:

		if (priv_data->rcvd_num < DEF_MAX_MSG_BUFF_NUM) {
			priv_data->rcvd_message[priv_data->rcvd_num].id =
				message->id;
			priv_data->rcvd_message[priv_data->rcvd_num].param0 =
				message->param0;
			priv_data->rcvd_message[priv_data->rcvd_num].param1 =
				message->param1;
			priv_data->rcvd_message[priv_data->rcvd_num].ptr =
				message->ptr;
		}

		priv_data->rcvd_num++;

		break;

	default:
		break;
	}
}

static tcase_service_foo_priv_t service_foo_priv;

DECLARE_SERVICE(CONFIG_TUNIT_SERVICE_FOO_NAME,
		CONFIG_TUNIT_SERVICE_FOO_LABEL,
		&service_foo_priv,
		tcase_service_foo_init,
		tcase_service_foo_deinit,
		tcase_service_foo_handle_message);

typedef struct {
	const service_t *	foo_svc;
	const service_t *	owner_svc;

	message_t		send_message[DEF_MAX_MSG_BUFF_NUM];
	message_t		rcvd_message[DEF_MAX_MSG_BUFF_NUM];
	int			send_num;
	int			rcvd_num;
} tcase_service_bar_priv_t;

static int tcase_service_bar_init(const service_t *svc, void *priv)
{
	tcase_service_bar_priv_t *priv_data = (tcase_service_bar_priv_t *)priv;

	(void)memset(priv_data, 0, sizeof(tcase_service_bar_priv_t));

	priv_data->foo_svc = service_get_binding(CONFIG_TUNIT_SERVICE_FOO_NAME);
	if (!priv_data->foo_svc)
		return -ENODEV;

	priv_data->owner_svc = svc;

	return 0;
}

static int tcase_service_bar_deinit(const service_t *svc, void *priv)
{
	return 0;
}

static void tcase_service_bar_handle_message(const message_t *	message,
					     message_t *	rsp_message,
					     void *		priv)
{
	tcase_service_bar_priv_t *priv_data = (tcase_service_bar_priv_t *)priv;
	message_t send_message;
	int i;
	int ret;

	switch (message->id) {
	case MSG_ID_SERVICE_INIT_EVT:

		(void)memset(priv_data->send_message, 0, DEF_MAX_MSG_BUFF_NUM);
		(void)memset(priv_data->rcvd_message, 0, DEF_MAX_MSG_BUFF_NUM);
		priv_data->send_num = 0;
		priv_data->rcvd_num = 0;

		break;

	case MSG_ID_SERVICE_START_EVT_TEST:

		for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
			priv_data->send_message[i].id = MSG_ID_SERVICE_DATA_EVT;
			priv_data->send_message[i].param0 = i;
			priv_data->send_message[i].param1 =
				DEF_MSG_SEND_PARAM_1;
			priv_data->send_message[i].ptr = NULL;
			ret = service_send_evt(priv_data->foo_svc,
					       &priv_data->send_message[i]);
			if (!ret)
				priv_data->send_num++;
		}

		break;

	case MSG_ID_SERVICE_START_REQ_TEST:

		send_message.id = MSG_ID_SERVICE_SEND_REQ;
		send_message.param0 = 0;
		send_message.param1 = 0;
		send_message.ptr = NULL;
		service_send_evt(priv_data->owner_svc, &send_message);

		break;

	case MSG_ID_SERVICE_SEND_REQ:

		if (priv_data->send_num < DEF_MAX_MSG_BUFF_NUM) {
			priv_data->send_message[priv_data->send_num].id =
				MSG_ID_SERVICE_DATA_REQ;
			priv_data->send_message[priv_data->send_num].param0 =
				DEF_MSG_REQ_PARAM_0;
			priv_data->send_message[priv_data->send_num].param1 =
				DEF_MSG_REQ_PARAM_1;
			priv_data->send_message[priv_data->send_num].ptr = NULL;
			ret = service_send_req(priv_data->foo_svc,
					       priv_data->owner_svc,
					       &priv_data->send_message[
						       priv_data->send_num]);
			if (!ret)
				priv_data->send_num++;
		}

		break;

	case MSG_ID_SERVICE_DATA_RSP:

		if (priv_data->rcvd_num < DEF_MAX_MSG_BUFF_NUM) {
			priv_data->rcvd_message[priv_data->rcvd_num].id =
				message->id;
			priv_data->rcvd_message[priv_data->rcvd_num].param0 =
				message->param0;
			priv_data->rcvd_message[priv_data->rcvd_num].param1 =
				message->param1;
			priv_data->rcvd_message[priv_data->rcvd_num].ptr =
				message->ptr;
		}

		priv_data->rcvd_num++;

		if (priv_data->send_num < DEF_MAX_MSG_BUFF_NUM) {
			send_message.id = MSG_ID_SERVICE_SEND_REQ;
			send_message.param0 = 0;
			send_message.param1 = 0;
			send_message.ptr = NULL;
			service_send_evt(priv_data->owner_svc, &send_message);
		}

		break;

	case MSG_ID_SERVICE_DATA_BROADCAST:

		if (priv_data->rcvd_num < DEF_MAX_MSG_BUFF_NUM) {
			priv_data->rcvd_message[priv_data->rcvd_num].id =
				message->id;
			priv_data->rcvd_message[priv_data->rcvd_num].param0 =
				message->param0;
			priv_data->rcvd_message[priv_data->rcvd_num].param1 =
				message->param1;
			priv_data->rcvd_message[priv_data->rcvd_num].ptr =
				message->ptr;
		}

		priv_data->rcvd_num++;

		break;

	default:
		break;
	}
}

static tcase_service_bar_priv_t service_bar_priv;

DECLARE_SERVICE(CONFIG_TUNIT_SERVICE_BAR_NAME,
		CONFIG_TUNIT_SERVICE_BAR_LABEL,
		&service_bar_priv,
		tcase_service_bar_init,
		tcase_service_bar_deinit,
		tcase_service_bar_handle_message);

/**
 * @brief   Suite initialization function.
 *
 * @param   None.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int tcase_suit_initialize(void)
{
	return 0;
}

/**
 * @brief   Suite cleanup function.
 *
 * @param   None.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int tcase_suit_cleanup(void)
{
	return 0;
}

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_service_check_structure(void)
{
	const service_t *svc;
	const object *obj;

	/* Check foo structure */
	svc = service_get_binding(CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(svc);

	TUNIT_ASSERT_STRING_EQUAL(svc->name, CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL(svc->thread_id);
	TUNIT_ASSERT_PTR_NOT_NULL(svc->queue_id);
	TUNIT_ASSERT_PTR_EQUAL(svc->init, tcase_service_foo_init);
	TUNIT_ASSERT_PTR_EQUAL(svc->deinit, tcase_service_foo_deinit);
	TUNIT_ASSERT_PTR_EQUAL(svc->handle_message,
			       tcase_service_foo_handle_message);
	TUNIT_ASSERT_PTR_EQUAL(svc->priv, &service_foo_priv);

	obj = service_get_owner(svc);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(obj);

	TUNIT_ASSERT_STRING_EQUAL(obj->name, CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_EQUAL(obj->probe, service_probe);
	TUNIT_ASSERT_PTR_EQUAL(obj->shutdown, service_shutdown);
	TUNIT_ASSERT_PTR_EQUAL(obj->suspend, NULL);
	TUNIT_ASSERT_PTR_EQUAL(obj->resume, NULL);
	TUNIT_ASSERT_PTR_EQUAL(obj->object_intf, &service_intf_default);
	TUNIT_ASSERT_PTR_EQUAL(obj->object_data, svc);
	TUNIT_ASSERT_PTR_EQUAL(obj->object_config, &service_config_default);

	/* Check bar structure */
	svc = service_get_binding(CONFIG_TUNIT_SERVICE_BAR_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(svc);

	TUNIT_ASSERT_STRING_EQUAL(svc->name, CONFIG_TUNIT_SERVICE_BAR_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL(svc->thread_id);
	TUNIT_ASSERT_PTR_NOT_NULL(svc->queue_id);
	TUNIT_ASSERT_PTR_EQUAL(svc->init, tcase_service_bar_init);
	TUNIT_ASSERT_PTR_EQUAL(svc->deinit, tcase_service_bar_deinit);
	TUNIT_ASSERT_PTR_EQUAL(svc->handle_message,
			       tcase_service_bar_handle_message);
	TUNIT_ASSERT_PTR_EQUAL(svc->priv, &service_bar_priv);

	obj = service_get_owner(svc);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(obj);

	TUNIT_ASSERT_STRING_EQUAL(obj->name, CONFIG_TUNIT_SERVICE_BAR_NAME);
	TUNIT_ASSERT_PTR_EQUAL(obj->probe, service_probe);
	TUNIT_ASSERT_PTR_EQUAL(obj->shutdown, service_shutdown);
	TUNIT_ASSERT_PTR_EQUAL(obj->suspend, NULL);
	TUNIT_ASSERT_PTR_EQUAL(obj->resume, NULL);
	TUNIT_ASSERT_PTR_EQUAL(obj->object_intf, &service_intf_default);
	TUNIT_ASSERT_PTR_EQUAL(obj->object_data, svc);
	TUNIT_ASSERT_PTR_EQUAL(obj->object_config, &service_config_default);
}

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_service_check_queue_attr(void)
{
	const service_t *svc;
	osMessageQueueId_t queue_id;
	uint32_t capacity;
	uint32_t size;
	uint32_t count;
	uint32_t space;

	svc = service_get_binding(CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(svc);

	queue_id = service_get_queue_id(svc);
	TUNIT_ASSERT_PTR_NOT_NULL(queue_id);

	capacity = osMessageQueueGetCapacity(queue_id);
	TUNIT_ASSERT_EQUAL(capacity, CONFIG_SERVICE_DEFAULT_QUEUE_LENGTH);

	size = osMessageQueueGetMsgSize(queue_id);
	TUNIT_ASSERT_EQUAL(size, sizeof(service_message_t));

	count = osMessageQueueGetCount(queue_id);
	TUNIT_ASSERT_EQUAL(count, 0);

	space = osMessageQueueGetSpace(queue_id);
	TUNIT_ASSERT_EQUAL(space, capacity);
}

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_service_check_thread_attr(void)
{
	const service_t *svc;
	osThreadId_t thread_id;
	const char *name;
	osPriority_t priority;
	osThreadState_t state;

	svc = service_get_binding(CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(svc);

	thread_id = service_get_thread_id(svc);
	TUNIT_ASSERT_PTR_NOT_NULL(thread_id);

	name = osThreadGetName(thread_id);
	TUNIT_ASSERT_STRING_EQUAL(name, CONFIG_SERVICE_DEFAULT_THREAD_NAME);

	priority = osThreadGetPriority(thread_id);
	TUNIT_ASSERT_EQUAL(priority, CONFIG_SERVICE_DEFAULT_THREAD_PRIORITY);

	state = osThreadGetState(thread_id);
	TUNIT_ASSERT_EQUAL(state, osThreadBlocked);
}

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_service_check_single_queue_evt(void)
{
	const service_t *svc;
	tcase_service_foo_priv_t *priv_data;
	message_t message;
	int i;
	int ret;

	svc = service_get_binding(CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(svc);

	message.id = MSG_ID_SERVICE_INIT_EVT;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		message.id = MSG_ID_SERVICE_DATA_EVT;
		message.param0 = i;
		message.param1 = DEF_MSG_SEND_PARAM_1;
		message.ptr = NULL;
		ret = service_send_evt(svc, &message);
		TUNIT_ASSERT_EQUAL(ret, 0);
	}

	/* Waiting 10ms, for server async processing messages */
	osDelay(10 * osKernelGetTickFreq() / 1000);

	/* Check the service test result */
	priv_data = &service_foo_priv;

	TUNIT_ASSERT_EQUAL(priv_data->rcvd_num, DEF_MAX_MSG_BUFF_NUM);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		TUNIT_ASSERT_EQUAL(priv_data->rcvd_message[i].id,
				   MSG_ID_SERVICE_DATA_EVT);
		TUNIT_ASSERT_EQUAL(priv_data->rcvd_message[i].param0, i);
		TUNIT_ASSERT_EQUAL(priv_data->rcvd_message[i].param1,
				   DEF_MSG_SEND_PARAM_1);
		TUNIT_ASSERT_PTR_NULL(priv_data->rcvd_message[i].ptr);
	}
}

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_service_check_multi_queue_evt(void)
{
	const service_t *foo_svc;
	const service_t *bar_svc;
	tcase_service_foo_priv_t *foo_priv_data;
	tcase_service_bar_priv_t *bar_priv_data;
	message_t message;
	int i;
	int ret;

	foo_svc = service_get_binding(CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(foo_svc);

	message.id = MSG_ID_SERVICE_INIT_EVT;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(foo_svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	bar_svc = service_get_binding(CONFIG_TUNIT_SERVICE_BAR_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(bar_svc);

	message.id = MSG_ID_SERVICE_INIT_EVT;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(bar_svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	message.id = MSG_ID_SERVICE_START_EVT_TEST;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(bar_svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	/* Waiting 10ms, for server async processing messages */
	osDelay(10 * osKernelGetTickFreq() / 1000);

	/* Check the service test result */
	foo_priv_data = &service_foo_priv;

	TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_num, DEF_MAX_MSG_BUFF_NUM);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].id,
				   MSG_ID_SERVICE_DATA_EVT);
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].param0, i);
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].param1,
				   DEF_MSG_SEND_PARAM_1);
		TUNIT_ASSERT_PTR_NULL(foo_priv_data->rcvd_message[i].ptr);
	}

	bar_priv_data = &service_bar_priv;

	TUNIT_ASSERT_EQUAL(bar_priv_data->send_num, DEF_MAX_MSG_BUFF_NUM);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		TUNIT_ASSERT_EQUAL(bar_priv_data->send_message[i].id,
				   MSG_ID_SERVICE_DATA_EVT);
		TUNIT_ASSERT_EQUAL(bar_priv_data->send_message[i].param0, i);
		TUNIT_ASSERT_EQUAL(bar_priv_data->send_message[i].param1,
				   DEF_MSG_SEND_PARAM_1);
		TUNIT_ASSERT_PTR_NULL(bar_priv_data->send_message[i].ptr);
	}
}

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_service_check_multi_queue_req(void)
{
	const service_t *foo_svc;
	const service_t *bar_svc;
	tcase_service_foo_priv_t *foo_priv_data;
	tcase_service_bar_priv_t *bar_priv_data;
	message_t message;
	int i;
	int ret;

	foo_svc = service_get_binding(CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(foo_svc);

	message.id = MSG_ID_SERVICE_INIT_EVT;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(foo_svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	bar_svc = service_get_binding(CONFIG_TUNIT_SERVICE_BAR_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(bar_svc);

	message.id = MSG_ID_SERVICE_INIT_EVT;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(bar_svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	message.id = MSG_ID_SERVICE_START_REQ_TEST;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(bar_svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	/* Waiting 10ms, for server async processing messages */
	osDelay(10 * osKernelGetTickFreq() / 1000);

	/* Check the service test result */
	foo_priv_data = &service_foo_priv;

	TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_num, DEF_MAX_MSG_BUFF_NUM);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].id,
				   MSG_ID_SERVICE_DATA_REQ);
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].param0,
				   DEF_MSG_REQ_PARAM_0);
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].param1,
				   DEF_MSG_REQ_PARAM_1);
		TUNIT_ASSERT_PTR_NULL(foo_priv_data->rcvd_message[i].ptr);
	}

	bar_priv_data = &service_bar_priv;

	TUNIT_ASSERT_EQUAL(bar_priv_data->send_num, DEF_MAX_MSG_BUFF_NUM);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		TUNIT_ASSERT_EQUAL(bar_priv_data->send_message[i].id,
				   MSG_ID_SERVICE_DATA_REQ);
		TUNIT_ASSERT_EQUAL(bar_priv_data->send_message[i].param0,
				   DEF_MSG_REQ_PARAM_0);
		TUNIT_ASSERT_EQUAL(bar_priv_data->send_message[i].param1,
				   DEF_MSG_REQ_PARAM_1);
		TUNIT_ASSERT_PTR_NULL(bar_priv_data->send_message[i].ptr);
	}

	TUNIT_ASSERT_EQUAL(bar_priv_data->rcvd_num, DEF_MAX_MSG_BUFF_NUM);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		TUNIT_ASSERT_EQUAL(bar_priv_data->rcvd_message[i].id,
				   MSG_ID_SERVICE_DATA_RSP);
		TUNIT_ASSERT_EQUAL(bar_priv_data->rcvd_message[i].param0,
				   DEF_MSG_RSP_PARAM_0);
		TUNIT_ASSERT_EQUAL(bar_priv_data->rcvd_message[i].param1,
				   DEF_MSG_RSP_PARAM_1);
		TUNIT_ASSERT_PTR_NULL(bar_priv_data->rcvd_message[i].ptr);
	}
}

/**
 * @brief   Testing function in a test case.
 *
 * @param   None.
 *
 * @retval  None.
 */
static void tcace_service_check_broadcast_evt(void)
{
	const service_t *foo_svc;
	const service_t *bar_svc;
	tcase_service_foo_priv_t *foo_priv_data;
	tcase_service_bar_priv_t *bar_priv_data;
	message_t message;
	int i;
	int ret;

	foo_svc = service_get_binding(CONFIG_TUNIT_SERVICE_FOO_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(foo_svc);

	message.id = MSG_ID_SERVICE_INIT_EVT;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(foo_svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	bar_svc = service_get_binding(CONFIG_TUNIT_SERVICE_BAR_NAME);
	TUNIT_ASSERT_PTR_NOT_NULL_FATAL(bar_svc);

	message.id = MSG_ID_SERVICE_INIT_EVT;
	message.param0 = 0;
	message.param1 = 0;
	message.ptr = NULL;
	ret = service_send_evt(bar_svc, &message);
	TUNIT_ASSERT_EQUAL(ret, 0);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		message.id = MSG_ID_SERVICE_DATA_BROADCAST;
		message.param0 = DEF_MSG_SEND_PARAM_0;
		message.param1 = DEF_MSG_SEND_PARAM_1;
		message.ptr = NULL;
		ret = service_broadcast_evt(&message);
		TUNIT_ASSERT_EQUAL(ret, 0);
	}

	/* Waiting 10ms, for server async processing messages */
	osDelay(10 * osKernelGetTickFreq() / 1000);

	/* Check the service test result */
	foo_priv_data = &service_foo_priv;

	TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_num, DEF_MAX_MSG_BUFF_NUM);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].id,
				   MSG_ID_SERVICE_DATA_BROADCAST);
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].param0,
				   DEF_MSG_SEND_PARAM_0);
		TUNIT_ASSERT_EQUAL(foo_priv_data->rcvd_message[i].param1,
				   DEF_MSG_SEND_PARAM_1);
		TUNIT_ASSERT_PTR_NULL(foo_priv_data->rcvd_message[i].ptr);
	}

	bar_priv_data = &service_bar_priv;

	TUNIT_ASSERT_EQUAL(bar_priv_data->rcvd_num, DEF_MAX_MSG_BUFF_NUM);

	for (i = 0; i < DEF_MAX_MSG_BUFF_NUM; i++) {
		TUNIT_ASSERT_EQUAL(bar_priv_data->rcvd_message[i].id,
				   MSG_ID_SERVICE_DATA_BROADCAST);
		TUNIT_ASSERT_EQUAL(bar_priv_data->rcvd_message[i].param0,
				   DEF_MSG_SEND_PARAM_0);
		TUNIT_ASSERT_EQUAL(bar_priv_data->rcvd_message[i].param1,
				   DEF_MSG_SEND_PARAM_1);
		TUNIT_ASSERT_PTR_NULL(bar_priv_data->rcvd_message[i].ptr);
	}
}

define_tunit_suit(CONFIG_TUNIT_SERVICE_SUIT_NAME,
		  CONFIG_TUNIT_SERVICE_SUIT_LABEL,
		  tcase_suit_initialize,
		  tcase_suit_cleanup);
define_tunit_case(CONFIG_TUNIT_SERVICE_SUIT_NAME,
		  CONFIG_TUNIT_SERVICE_SUIT_LABEL,
		  "Service check structure test",
		  tcace_service_check_structure);
define_tunit_case(CONFIG_TUNIT_SERVICE_SUIT_NAME,
		  CONFIG_TUNIT_SERVICE_SUIT_LABEL,
		  "Service check queue attr test",
		  tcace_service_check_queue_attr);
define_tunit_case(CONFIG_TUNIT_SERVICE_SUIT_NAME,
		  CONFIG_TUNIT_SERVICE_SUIT_LABEL,
		  "Service check thread attr test",
		  tcace_service_check_thread_attr);
define_tunit_case(CONFIG_TUNIT_SERVICE_SUIT_NAME,
		  CONFIG_TUNIT_SERVICE_SUIT_LABEL,
		  "Service check single queue evt",
		  tcace_service_check_single_queue_evt);
define_tunit_case(CONFIG_TUNIT_SERVICE_SUIT_NAME,
		  CONFIG_TUNIT_SERVICE_SUIT_LABEL,
		  "Service check multi queue evt",
		  tcace_service_check_multi_queue_evt);
define_tunit_case(CONFIG_TUNIT_SERVICE_SUIT_NAME,
		  CONFIG_TUNIT_SERVICE_SUIT_LABEL,
		  "Service check multi queue request",
		  tcace_service_check_multi_queue_req);
define_tunit_case(CONFIG_TUNIT_SERVICE_SUIT_NAME,
		  CONFIG_TUNIT_SERVICE_SUIT_LABEL,
		  "Service check broadcast evt",
		  tcace_service_check_broadcast_evt);

#endif /* CONFIG_TUNIT_SERVICE_SUIT_NAME */
