/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "telephony_common.h"
#include "telephony_sim.h"
#include "telephony_call.h"
#include "telephony_network.h"
#include "telephony_private.h"
#include <gio/gio.h>
#include <dlog.h>
#include <stdlib.h>

#include <tapi_common.h>
#include <TelCall.h>
#include <TelSim.h>
#include <TelNetwork.h>
#include <TapiUtility.h>

#define CALLBACK_CALL(data) \
	if (evt_cb_data->cb) { \
		evt_cb_data->cb(evt_cb_data->handle, \
			 evt_cb_data->noti_id, data, evt_cb_data->user_data); \
	}

/* Handle deprecated noti_id for backward compatibility */
#define CALLBACK_CALL_FOR_DEPRECATED_NOTI(evt_cb_data) \
	if (evt_cb_data->noti_id == TELEPHONY_NOTI_VOICE_CALL_STATE) { \
		telephony_call_state_e call_state; \
		telephony_call_get_voice_call_state(evt_cb_data->handle, &call_state); \
		CALLBACK_CALL(&call_state); \
		return; \
	} else if (evt_cb_data->noti_id == TELEPHONY_NOTI_VIDEO_CALL_STATE) { \
		telephony_call_state_e call_state; \
		telephony_call_get_video_call_state(evt_cb_data->handle, &call_state); \
		CALLBACK_CALL(&call_state); \
		return; \
	}

typedef struct {
	telephony_h handle;
	telephony_noti_e noti_id;
	telephony_noti_cb cb;
	void *user_data;
} telephony_evt_cb_data;

static const char *voice_call_state_tbl[] = {
	TAPI_NOTI_VOICE_CALL_STATUS_IDLE,
	TAPI_NOTI_VOICE_CALL_STATUS_ACTIVE,
	TAPI_NOTI_VOICE_CALL_STATUS_HELD,
	TAPI_NOTI_VOICE_CALL_STATUS_DIALING,
	TAPI_NOTI_VOICE_CALL_STATUS_ALERT,
	TAPI_NOTI_VOICE_CALL_STATUS_INCOMING
};

static const char *video_call_state_tbl[] = {
	TAPI_NOTI_VIDEO_CALL_STATUS_IDLE,
	TAPI_NOTI_VIDEO_CALL_STATUS_ACTIVE,
	TAPI_NOTI_VIDEO_CALL_STATUS_DIALING,
	TAPI_NOTI_VIDEO_CALL_STATUS_ALERT,
	TAPI_NOTI_VIDEO_CALL_STATUS_INCOMING
};

static const char *_mapping_noti_id(telephony_noti_e noti_id)
{
	switch (noti_id) {
	case TELEPHONY_NOTI_SIM_STATUS:
		return TAPI_NOTI_SIM_STATUS;
	case TELEPHONY_NOTI_SIM_CALL_FORWARDING_INDICATOR_STATE:
		return TAPI_PROP_SIM_CALL_FORWARD_STATE;
	case TELEPHONY_NOTI_NETWORK_SERVICE_STATE:
		return TAPI_PROP_NETWORK_SERVICE_TYPE;
	case TELEPHONY_NOTI_NETWORK_CELLID:
		return TAPI_NOTI_NETWORK_CELLID;
	case TELEPHONY_NOTI_NETWORK_ROAMING_STATUS:
		return TAPI_PROP_NETWORK_ROAMING_STATUS;
	case TELEPHONY_NOTI_NETWORK_SIGNALSTRENGTH_LEVEL:
		return TAPI_PROP_NETWORK_SIGNALSTRENGTH_LEVEL;
	case TELEPHONY_NOTI_NETWORK_NETWORK_NAME:
		return TAPI_PROP_NETWORK_NETWORK_NAME;
	case TELEPHONY_NOTI_CALL_PREFERRED_VOICE_SUBSCRIPTION:
		return TAPI_NOTI_CALL_PREFERRED_VOICE_SUBSCRIPTION;
	case TELEPHONY_NOTI_NETWORK_PS_TYPE:
		return TAPI_PROP_NETWORK_PS_TYPE;
	case TELEPHONY_NOTI_NETWORK_DEFAULT_DATA_SUBSCRIPTION:
		return TAPI_NOTI_NETWORK_DEFAULT_DATA_SUBSCRIPTION;
	case TELEPHONY_NOTI_NETWORK_DEFAULT_SUBSCRIPTION:
		return TAPI_NOTI_NETWORK_DEFAULT_SUBSCRIPTION;
	case TELEPHONY_NOTI_NETWORK_LAC:
		return TAPI_NOTI_NETWORK_LAC;
	case TELEPHONY_NOTI_NETWORK_TAC:
		return TAPI_NOTI_NETWORK_TAC;
	case TELEPHONY_NOTI_NETWORK_SYSTEM_ID:
		return TAPI_NOTI_NETWORK_SYSTEM_ID;
	case TELEPHONY_NOTI_NETWORK_NETWORK_ID:
		return TAPI_NOTI_NETWORK_NETWORK_ID;
	case TELEPHONY_NOTI_NETWORK_BS_ID:
		return TAPI_NOTI_NETWORK_BS_ID;
	case TELEPHONY_NOTI_NETWORK_BS_LATITUDE:
		return TAPI_NOTI_NETWORK_BS_LATITUDE;
	case TELEPHONY_NOTI_NETWORK_BS_LONGITUDE:
		return TAPI_NOTI_NETWORK_BS_LONGITUDE;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_IDLE:
		return TAPI_NOTI_VOICE_CALL_STATUS_IDLE;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_ACTIVE:
		return TAPI_NOTI_VOICE_CALL_STATUS_ACTIVE;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_HELD:
		return TAPI_NOTI_VOICE_CALL_STATUS_HELD;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_DIALING:
		return TAPI_NOTI_VOICE_CALL_STATUS_DIALING;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_ALERTING:
		return TAPI_NOTI_VOICE_CALL_STATUS_ALERT;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_INCOMING:
		return TAPI_NOTI_VOICE_CALL_STATUS_INCOMING;
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_IDLE:
		return TAPI_NOTI_VIDEO_CALL_STATUS_IDLE;
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_ACTIVE:
		return TAPI_NOTI_VIDEO_CALL_STATUS_ACTIVE;
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_DIALING:
		return TAPI_NOTI_VIDEO_CALL_STATUS_DIALING;
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_ALERTING:
		return TAPI_NOTI_VIDEO_CALL_STATUS_ALERT;
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_INCOMING:
		return TAPI_NOTI_VIDEO_CALL_STATUS_INCOMING;
	default:
		return NULL;
	}
}
/* LCOV_EXCL_START */
static int _mapping_sim_status(TelSimCardStatus_t tapi_sim_status)
{
	int sim_status;

	switch (tapi_sim_status) {
	case TAPI_SIM_STATUS_CARD_ERROR:
	case TAPI_SIM_STATUS_CARD_NOT_PRESENT:
	case TAPI_SIM_STATUS_CARD_BLOCKED:
	case TAPI_SIM_STATUS_CARD_REMOVED:
	case TAPI_SIM_STATUS_CARD_CRASHED:
		sim_status = TELEPHONY_SIM_STATE_UNAVAILABLE;
		break;
	case TAPI_SIM_STATUS_SIM_PIN_REQUIRED:
	case TAPI_SIM_STATUS_SIM_PUK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_NCK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_NSCK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_SPCK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_CCK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_LOCK_REQUIRED:
		sim_status = TELEPHONY_SIM_STATE_LOCKED;
		break;
	case TAPI_SIM_STATUS_SIM_INIT_COMPLETED:
		sim_status = TELEPHONY_SIM_STATE_AVAILABLE;
		break;
	case TAPI_SIM_STATUS_UNKNOWN:
	case TAPI_SIM_STATUS_SIM_INITIALIZING:
	default:
		sim_status = TELEPHONY_SIM_STATE_UNKNOWN;
		break;
	}

	return sim_status;
}

static int _mapping_service_state(int tapi_service_type)
{
	int service_state;

	switch (tapi_service_type) {
	case TAPI_NETWORK_SERVICE_TYPE_UNKNOWN:
	case TAPI_NETWORK_SERVICE_TYPE_NO_SERVICE:
	case TAPI_NETWORK_SERVICE_TYPE_SEARCH:
		service_state = TELEPHONY_NETWORK_SERVICE_STATE_OUT_OF_SERVICE;
		break;
	case TAPI_NETWORK_SERVICE_TYPE_EMERGENCY:
		service_state = TELEPHONY_NETWORK_SERVICE_STATE_EMERGENCY_ONLY;
		break;
	default:
		service_state = TELEPHONY_NETWORK_SERVICE_STATE_IN_SERVICE;
		break;
	}

	return service_state;
}
/* LCOV_EXCL_STOP */

static telephony_error_e __deregister_all_noti(telephony_h handle, telephony_noti_e noti_id)
{
	const char *tapi_noti;
	int ret, i;
	TapiHandle *tapi_h;

	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;

	tapi_noti = _mapping_noti_id(noti_id);
	if (tapi_noti == NULL && noti_id != TELEPHONY_NOTI_VOICE_CALL_STATE
			&& noti_id != TELEPHONY_NOTI_VIDEO_CALL_STATE) {
		LOGE("Not supported noti_id");
		return TELEPHONY_ERROR_INVALID_PARAMETER;
	}

	/* Deregister all TAPI_NOTI_VOICE/VIDEO_CALL_STATUS_xxx notification */
	if (noti_id == TELEPHONY_NOTI_VOICE_CALL_STATE) {
		int count = sizeof(voice_call_state_tbl) / sizeof(char *);
		for (i = 0; i < count; i++) {
			ret = tel_deregister_noti_event(tapi_h, voice_call_state_tbl[i]);
			if (ret != TAPI_API_SUCCESS)
				LOGE("Noti [%s] deregistration failed", voice_call_state_tbl[i]);
		}
	} else if (noti_id == TELEPHONY_NOTI_VIDEO_CALL_STATE) {
		int count = sizeof(video_call_state_tbl) / sizeof(char *);
		for (i = 0; i < count; i++) {
			ret = tel_deregister_noti_event(tapi_h, video_call_state_tbl[i]);
			if (ret != TAPI_API_SUCCESS)
				LOGE("Noti [%s] deregistration failed", video_call_state_tbl[i]);
		}
	} else {
		/* Deregister other notifications */
		ret = tel_deregister_noti_event(tapi_h, tapi_noti);
		if (ret != TAPI_API_SUCCESS)
			LOGE("Noti [%s] deregistration failed", tapi_noti);
	}

	return TELEPHONY_ERROR_NONE;
}

static void _deregister_noti(gpointer data)
{
	telephony_evt_cb_data *evt_cb_data = data;

	if (evt_cb_data != NULL) {
		/* Deregister all TAPI_NOTI_VOICE/VIDEO_CALL_STATUS_xxx notification */
		if (__deregister_all_noti(evt_cb_data->handle, evt_cb_data->noti_id)
				== TELEPHONY_ERROR_NONE) {
			LOGI("De-registered noti_id: [%d]", evt_cb_data->noti_id);
		}

		/* Free evt_cb_data */
		g_free(evt_cb_data);
	}
}

/* LCOV_EXCL_START */
static void on_signal_callback(TapiHandle *tapi_h, const char *evt_id,
	void *data, void *user_data)
{
	telephony_evt_cb_data *evt_cb_data = user_data;

	if (evt_cb_data == NULL) {
		LOGE("evt_cb_data is NULL");
		return;
	}

	if (!g_strcmp0(evt_id, TAPI_NOTI_SIM_STATUS)) {
		int sim_status = _mapping_sim_status(*(TelSimCardStatus_t *)data);
		CALLBACK_CALL(&sim_status);
	} else if (!g_strcmp0(evt_id, TAPI_PROP_SIM_CALL_FORWARD_STATE)) {
		bool state = *(bool *)data;
		CALLBACK_CALL(&state);
	} else if (!g_strcmp0(evt_id, TAPI_PROP_NETWORK_SERVICE_TYPE)) {
		int service_state = _mapping_service_state(*(int *)data);
		CALLBACK_CALL(&service_state);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_CELLID)) {
		int cell_id = *(int *)data;
		CALLBACK_CALL(&cell_id);
	} else if (!g_strcmp0(evt_id, TAPI_PROP_NETWORK_ROAMING_STATUS)) {
		int roaming_status = *(int *)data;
		CALLBACK_CALL(&roaming_status);
	} else if (!g_strcmp0(evt_id, TAPI_PROP_NETWORK_SIGNALSTRENGTH_LEVEL)) {
		int rssi = *(int *)data;
		CALLBACK_CALL(&rssi);
	} else if (!g_strcmp0(evt_id, TAPI_PROP_NETWORK_NETWORK_NAME)) {
		char *network_name = data;
		CALLBACK_CALL(network_name);
	} else if (!g_strcmp0(evt_id, TAPI_PROP_NETWORK_PS_TYPE)) {
		int ps_type = *(int *)data;
		CALLBACK_CALL(&ps_type);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_DEFAULT_DATA_SUBSCRIPTION)) {
		int default_data_sub = *(int *)data;
		CALLBACK_CALL(&default_data_sub);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_DEFAULT_SUBSCRIPTION)) {
		int default_sub = *(int *)data;
		CALLBACK_CALL(&default_sub);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_LAC)) {
		int lac = *(int *)data;
		CALLBACK_CALL(&lac);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_TAC)) {
		int tac = *(int *)data;
		CALLBACK_CALL(&tac);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_SYSTEM_ID)) {
		int sid = *(int *)data;
		CALLBACK_CALL(&sid);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_NETWORK_ID)) {
		int nid = *(int *)data;
		CALLBACK_CALL(&nid);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_BS_ID)) {
		int bsid = *(int *)data;
		CALLBACK_CALL(&bsid);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_BS_LATITUDE)) {
		int latitude = *(int *)data;
		CALLBACK_CALL(&latitude);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_NETWORK_BS_LONGITUDE)) {
		int longtitude = *(int *)data;
		CALLBACK_CALL(&longtitude);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_VOICE_CALL_STATUS_IDLE)
			|| !g_strcmp0(evt_id, TAPI_NOTI_VIDEO_CALL_STATUS_IDLE)) {
		TelCallStatusIdleNoti_t *noti = data;
		unsigned int handle_id = noti->id;
		CALLBACK_CALL_FOR_DEPRECATED_NOTI(evt_cb_data);
		CALLBACK_CALL(&handle_id);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_VOICE_CALL_STATUS_ACTIVE)
			|| !g_strcmp0(evt_id, TAPI_NOTI_VIDEO_CALL_STATUS_ACTIVE)) {
		TelCallStatusActiveNoti_t *noti = data;
		unsigned int handle_id = noti->id;
		CALLBACK_CALL_FOR_DEPRECATED_NOTI(evt_cb_data);
		CALLBACK_CALL(&handle_id);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_VOICE_CALL_STATUS_HELD)) {
		TelCallStatusHeldNoti_t *noti = data;
		unsigned int handle_id = noti->id;
		CALLBACK_CALL_FOR_DEPRECATED_NOTI(evt_cb_data);
		CALLBACK_CALL(&handle_id);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_VOICE_CALL_STATUS_DIALING)
			|| !g_strcmp0(evt_id, TAPI_NOTI_VIDEO_CALL_STATUS_DIALING)) {
		TelCallStatusDialingNoti_t *noti = data;
		unsigned int handle_id = noti->id;
		CALLBACK_CALL_FOR_DEPRECATED_NOTI(evt_cb_data);
		CALLBACK_CALL(&handle_id);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_VOICE_CALL_STATUS_ALERT)
			|| !g_strcmp0(evt_id, TAPI_NOTI_VIDEO_CALL_STATUS_ALERT)) {
		TelCallStatusAlertNoti_t *noti = data;
		unsigned int handle_id = noti->id;
		CALLBACK_CALL_FOR_DEPRECATED_NOTI(evt_cb_data);
		CALLBACK_CALL(&handle_id);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_VOICE_CALL_STATUS_INCOMING)
			|| !g_strcmp0(evt_id, TAPI_NOTI_VIDEO_CALL_STATUS_INCOMING)) {
		TelCallIncomingCallInfo_t *noti = data;
		unsigned int handle_id = noti->CallHandle;
		CALLBACK_CALL_FOR_DEPRECATED_NOTI(evt_cb_data);
		CALLBACK_CALL(&handle_id);
	} else if (!g_strcmp0(evt_id, TAPI_NOTI_CALL_PREFERRED_VOICE_SUBSCRIPTION)) {
		int call_pref_voice_sub = *(int *)data;
		CALLBACK_CALL(&call_pref_voice_sub);
	} else {
		LOGE("Unhandled noti: [%s]", evt_id);
	}
}
/* LCOV_EXCL_STOP */

int telephony_set_noti_cb(telephony_h handle,
	telephony_noti_e noti_id, telephony_noti_cb cb, void *user_data)
{
	telephony_evt_cb_data *evt_cb_data = NULL;
	const char *tapi_noti;
	int ret, i;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);

	LOGI("Entry");
	tapi_h = ((telephony_data *)handle)->tapi_h;

	/* Mapping TAPI notification */
	tapi_noti = _mapping_noti_id(noti_id);
	if (tapi_noti == NULL && noti_id != TELEPHONY_NOTI_VOICE_CALL_STATE
			&& noti_id != TELEPHONY_NOTI_VIDEO_CALL_STATE) {
		LOGE("Not supported noti_id");
		return TELEPHONY_ERROR_INVALID_PARAMETER;
	}

	/* Make evt_cb_data */
	evt_cb_data = g_new0(telephony_evt_cb_data, 1);
	evt_cb_data->handle = handle;
	evt_cb_data->noti_id = noti_id;
	evt_cb_data->cb = cb;
	evt_cb_data->user_data = user_data;

	/*
	 * In case of register Call State notification,
	 * we should take care of all TAPI_NOTI_VOICE/VIDEO_CALL_STATUS_xxx notification
	 */
	if (noti_id == TELEPHONY_NOTI_VOICE_CALL_STATE) {
		int count = sizeof(voice_call_state_tbl) / sizeof(char *);
		for (i = 0; i < count; i++) {
			ret = tel_register_noti_event(tapi_h, voice_call_state_tbl[i], on_signal_callback, evt_cb_data);
			if (ret != TAPI_API_SUCCESS) {
				LOGE("Noti registration failed");
				g_free(evt_cb_data);
				return TELEPHONY_ERROR_OPERATION_FAILED;
			}
		}
	} else if (noti_id == TELEPHONY_NOTI_VIDEO_CALL_STATE) {
		int count = sizeof(video_call_state_tbl) / sizeof(char *);
		for (i = 0; i < count; i++) {
			ret = tel_register_noti_event(tapi_h, video_call_state_tbl[i], on_signal_callback, evt_cb_data);
			if (ret != TAPI_API_SUCCESS) {
				LOGE("Noti registration failed");
				g_free(evt_cb_data);
				return TELEPHONY_ERROR_OPERATION_FAILED;
			}
		}
	} else { /* Register other notifications */
		ret = tel_register_noti_event(tapi_h, tapi_noti, on_signal_callback, evt_cb_data);
		if (ret != TAPI_API_SUCCESS) {
			LOGE("Noti registration failed");
			g_free(evt_cb_data);
			return TELEPHONY_ERROR_OPERATION_FAILED;
		}
	}

	/* Append evt_cb_data to free */
	((telephony_data *)handle)->evt_list = g_slist_append(((telephony_data *)handle)->evt_list, evt_cb_data);

	return TELEPHONY_ERROR_NONE;
}

int telephony_unset_noti_cb(telephony_h handle, telephony_noti_e noti_id)
{
	telephony_evt_cb_data *evt_cb_data = NULL;
	GSList *list = NULL;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);

	LOGI("Entry");

	/* Deregister all TAPI_NOTI_VOICE/VIDEO_CALL_STATUS_xxx notification */
	if (__deregister_all_noti(handle, noti_id) != TELEPHONY_ERROR_NONE) {
		LOGE("De-registration failed");
		return TELEPHONY_ERROR_INVALID_PARAMETER;
	}
	LOGI("De-registered noti_id: [%d]", noti_id);

	/* Free evt_cb_data */
	list = g_slist_nth(((telephony_data *)handle)->evt_list, 0);
	while (list) {
		evt_cb_data = list->data;
		if (evt_cb_data->noti_id == noti_id) {
			((telephony_data *)handle)->evt_list = g_slist_remove(((telephony_data *)handle)->evt_list, evt_cb_data);
			g_free(evt_cb_data);
			break;
		}

		list = g_slist_next(list);
	}

	return TELEPHONY_ERROR_NONE;
}

int telephony_init(telephony_handle_list_s *list)
{
	char **cp_list;
	int cp_count = 0;
	int i;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(list);

#if !GLIB_CHECK_VERSION(2, 35, 0)
	/* Need g_type_init() to use tel_get_cp_name_list() */
	g_type_init();
#endif

	cp_list = tel_get_cp_name_list();
	if (cp_list == NULL) {
		LOGE("cp_list is NULL");
		return TELEPHONY_ERROR_OPERATION_FAILED;
	}

	while (cp_list[cp_count])
		cp_count++;

	list->count = cp_count;
	list->handle = malloc(cp_count * sizeof(telephony_h));
	if (list->handle == NULL) {
		LOGE("Out of memory");
		g_strfreev(cp_list);
		return TELEPHONY_ERROR_OUT_OF_MEMORY;
	}

	for (i = 0; i < cp_count; i++) {
		telephony_data *tmp = malloc(sizeof(telephony_data));
		if (tmp == NULL) {
			/* LCOV_EXCL_START */
			int j;
			LOGE("Out of memory");
			/* Need to free already allocated data */
			for (j = 0; j < i; j++) {
				if (list->handle[j]) {
					tel_deinit(((telephony_data *)list->handle[j])->tapi_h);
					free(list->handle[j]);
				}
			}
			free(list->handle);
			g_strfreev(cp_list);
			return TELEPHONY_ERROR_OUT_OF_MEMORY;
			/* LCOV_EXCL_STOP */
		}

		tmp->evt_list = NULL;
		tmp->tapi_h = tel_init(cp_list[i]);
		if (tmp->tapi_h == NULL) {
			/* LCOV_EXCL_START */
			int j;
			LOGE("tel_init() is failed");
			/* Need to free already allocated data */
			for (j = 0; j < i; j++) {
				if (list->handle[j]) {
					tel_deinit(((telephony_data *)list->handle[j])->tapi_h);
					free(list->handle[j]);
				}
			}
			free(list->handle);
			g_strfreev(cp_list);
			return TELEPHONY_ERROR_OPERATION_FAILED;
			/* LCOV_EXCL_STOP */
		}
		list->handle[i] = (telephony_h)tmp;
	}
	g_strfreev(cp_list);

	return TELEPHONY_ERROR_NONE;
}

int telephony_deinit(telephony_handle_list_s *list)
{
	unsigned int i;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(list);

	for (i = 0; i < list->count; i++) {
		telephony_data *tmp = (telephony_data *)list->handle[i];

		/* De-init all TapiHandle */
		tel_deinit(tmp->tapi_h);
		tmp->tapi_h = NULL;

		/* De-register all registered events */
		g_slist_free_full(tmp->evt_list, _deregister_noti);
		tmp->evt_list = NULL;

		/* Free handle[i] */
		free(list->handle[i]);
	}
	free(list->handle);
	list->handle = NULL;
	list->count = 0;

	return TELEPHONY_ERROR_NONE;
}

int telephony_get_state(telephony_state_e *state)
{
	int res = 0;
	int value = 0;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(state);

	res = tel_get_ready_state(&value);
	if (res != TAPI_API_SUCCESS)
		return TELEPHONY_ERROR_OPERATION_FAILED;

	*state = value;

	return TELEPHONY_ERROR_NONE;
}

int telephony_set_state_changed_cb(telephony_state_changed_cb callback, void *user_data)
{
	int res = 0;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(callback);

	res = tel_register_ready_state_cb((tapi_state_cb)callback, user_data);
	if (res != TAPI_API_SUCCESS)
		return TELEPHONY_ERROR_OPERATION_FAILED;

	return TELEPHONY_ERROR_NONE;
}

int telephony_unset_state_changed_cb(telephony_state_changed_cb callback)
{
	int res = 0;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(callback);

	res = tel_deregister_ready_state_cb((tapi_state_cb)callback);
	if (res != TAPI_API_SUCCESS)
		return TELEPHONY_ERROR_OPERATION_FAILED;

	return TELEPHONY_ERROR_NONE;
}
