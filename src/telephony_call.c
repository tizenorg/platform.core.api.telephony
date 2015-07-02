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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <dlog.h>

#include <tapi_common.h>
#include <TapiUtility.h>
#include <ITapiCall.h>
#include "telephony_common.h"
#include "telephony_call.h"
#include "telephony_private.h"

static void _mapping_call_state(TelCallStates_t tapi_call_state, telephony_call_state_e *call_state)
{
	switch (tapi_call_state) {
	case TAPI_CALL_STATE_ACTIVE:
	case TAPI_CALL_STATE_HELD:
		if (*call_state != TELEPHONY_CALL_STATE_CONNECTING)
			*call_state = TELEPHONY_CALL_STATE_CONNECTED;
		break;
	case TAPI_CALL_STATE_DIALING:
	case TAPI_CALL_STATE_ALERT:
	case TAPI_CALL_STATE_INCOMING:
	case TAPI_CALL_STATE_WAITING:
		*call_state = TELEPHONY_CALL_STATE_CONNECTING;
		break;
	/* Do not check IDLE state to prevent overriding in case of multi-party call */
	default:
		LOGE("Not handled call state: [%d]", tapi_call_state);
		break;
	}
}

static void _mapping_call_status(TelCallStates_t tapi_call_state,
	telephony_call_status_e *status)
{
	switch (tapi_call_state) {
	case TAPI_CALL_STATE_IDLE:
		*status = TELEPHONY_CALL_STATUS_IDLE;
		break;
	case TAPI_CALL_STATE_ACTIVE:
		*status = TELEPHONY_CALL_STATUS_ACTIVE;
		break;
	case TAPI_CALL_STATE_HELD:
		*status = TELEPHONY_CALL_STATUS_HELD;
		break;
	case TAPI_CALL_STATE_DIALING:
		*status = TELEPHONY_CALL_STATUS_DIALING;
		break;
	case TAPI_CALL_STATE_ALERT:
		*status = TELEPHONY_CALL_STATUS_ALERTING;
		break;
	case TAPI_CALL_STATE_INCOMING:
	case TAPI_CALL_STATE_WAITING:
		*status = TELEPHONY_CALL_STATUS_INCOMING;
		break;
	default:
		LOGE("Not handled call state: [%d]", tapi_call_state);
		break;
	}
}

static void _voice_call_get_state_cb(TelCallStatus_t *tapi_status, void *user_data)
{
	telephony_call_state_e *call_state = user_data;

	if (tapi_status->CallType != TAPI_CALL_TYPE_VOICE
			&& tapi_status->CallType != TAPI_CALL_TYPE_E911)
		return;

	_mapping_call_state(tapi_status->CallState, call_state);
}

static void _video_call_get_state_cb(TelCallStatus_t *tapi_status, void *user_data)
{
	telephony_call_state_e *call_state = user_data;

	if (tapi_status->CallType != TAPI_CALL_TYPE_DATA)
		return;

	_mapping_call_state(tapi_status->CallState, call_state);
}

static void _get_call_status_cb(TelCallStatus_t *status, void *user_data)
{
	GSList **list = user_data;
	telephony_call_info_s *call_info;

	call_info = g_malloc0(sizeof(telephony_call_info_s));
	if (!call_info)
		return;
	call_info->id = status->CallHandle;
	call_info->type = status->CallType;
	call_info->direction = status->bMoCall ? TELEPHONY_CALL_DIRECTION_MO : TELEPHONY_CALL_DIRECTION_MT;
	call_info->conference_status = status->bConferenceState;
	_mapping_call_status(status->CallState, &call_info->status);
	strncpy(call_info->number, status->pNumber, TELEPHONY_CALL_NUMBER_LEN_MAX);

	LOGI("id[%d] number[%s] type[%s] status[%s] direction[%s] conference_status[%s]",
		call_info->id, call_info->number,
		call_info->type == TELEPHONY_CALL_TYPE_VOICE ? "VOICE" :
		call_info->type == TELEPHONY_CALL_TYPE_VIDEO ? "VIDEO" : "E911",
		call_info->status == TELEPHONY_CALL_STATUS_IDLE ? "IDLE" :
		call_info->status == TELEPHONY_CALL_STATUS_ACTIVE ? "ACTVIE" :
		call_info->status == TELEPHONY_CALL_STATUS_HELD ? "HELD" :
		call_info->status == TELEPHONY_CALL_STATUS_DIALING ? "DIALING" :
		call_info->status == TELEPHONY_CALL_STATUS_ALERTING ? "ALERTING" :
		call_info->status == TELEPHONY_CALL_STATUS_INCOMING ? "INCOMING" : "UNKNOWN",
		call_info->direction == TELEPHONY_CALL_DIRECTION_MO ? "MO" : "MT",
		call_info->conference_status ? "TRUE" : "FALSE");
	*list = g_slist_append(*list, call_info);
}

int telephony_call_get_voice_call_state(telephony_h handle, telephony_call_state_e *call_state)
{
	int ret;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(call_state);

	*call_state = TELEPHONY_CALL_STATE_IDLE;

	ret = tel_get_call_status_all(tapi_h, _voice_call_get_state_cb, call_state);
	if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		return TELEPHONY_ERROR_PERMISSION_DENIED;
	} else if (ret != TAPI_API_SUCCESS) {
		LOGE("OPERATION_FAILED");
		return TELEPHONY_ERROR_OPERATION_FAILED;
	}

	LOGI("call_state: [%d]", *call_state);

	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_video_call_state(telephony_h handle, telephony_call_state_e *call_state)
{
	int ret;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(call_state);

	*call_state = TELEPHONY_CALL_STATE_IDLE;

	ret = tel_get_call_status_all(tapi_h, _video_call_get_state_cb, call_state);
	if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		return TELEPHONY_ERROR_PERMISSION_DENIED;
	} else if (ret != TAPI_API_SUCCESS) {
		LOGE("OPERATION_FAILED");
		return TELEPHONY_ERROR_OPERATION_FAILED;
	}

	LOGI("call_state: [%d]", *call_state);

	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_preferred_voice_subscription(telephony_h handle, telephony_call_preferred_voice_subs_e *call_sub)
{
	int ret;
	TapiHandle *tapi_h;
	TelCallPreferredVoiceSubs_t preferred_sub = TAPI_CALL_PREFERRED_VOICE_SUBS_UNKNOWN;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(call_sub);

	ret = tel_get_call_preferred_voice_subscription(tapi_h, &preferred_sub);
	if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		return TELEPHONY_ERROR_PERMISSION_DENIED;
	} else if (ret != TAPI_API_SUCCESS) {
		LOGE("OPERATION_FAILED");
		return TELEPHONY_ERROR_OPERATION_FAILED;
	} else {
		switch (preferred_sub) {
		case TAPI_CALL_PREFERRED_VOICE_SUBS_CURRENT_NETWORK:
			*call_sub = TELEPHONY_CALL_PREFERRED_VOICE_SUBS_CURRENT_NETWORK;
			break;
		case TAPI_CALL_PREFERRED_VOICE_SUBS_ASK_ALWAYS:
			*call_sub = TELEPHONY_CALL_PREFERRED_VOICE_SUBS_ASK_ALWAYS;
			break;
		case TAPI_CALL_PREFERRED_VOICE_SUBS_SIM1:
			*call_sub = TELEPHONY_CALL_PREFERRED_VOICE_SUBS_SIM1;
			break;
		case TAPI_CALL_PREFERRED_VOICE_SUBS_SIM2:
			*call_sub = TELEPHONY_CALL_PREFERRED_VOICE_SUBS_SIM2;
			break;
		case TAPI_CALL_PREFERRED_VOICE_SUBS_UNKNOWN:
		default:
			*call_sub = TELEPHONY_CALL_PREFERRED_VOICE_SUBS_UNKNOWN;
			break;
		}
	}
	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_call_list(telephony_h handle,
	unsigned int *count, telephony_call_h **call_list)
{
	int ret;
	TapiHandle *tapi_h;
	GSList *list = NULL, *tmp;
	unsigned int call_index = 0;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(call_list);
	CHECK_INPUT_PARAMETER(count);

	ret = tel_get_call_status_all(tapi_h, _get_call_status_cb, &list);
	if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		return TELEPHONY_ERROR_PERMISSION_DENIED;
	} else if (ret != TAPI_API_SUCCESS) {
		LOGE("OPERATION_FAILED");
		return TELEPHONY_ERROR_OPERATION_FAILED;
	}

	if (g_slist_length(list)) {
		*count = g_slist_length(list);
		*call_list = g_malloc0(*count * sizeof(telephony_call_h));

		tmp = list;
		while (tmp) {
			telephony_call_info_s *call_info = tmp->data;
			(*call_list)[call_index] = g_malloc0(sizeof(telephony_call_info_s));
			memcpy((*call_list)[call_index], call_info, sizeof(telephony_call_info_s));
			tmp = g_slist_next(tmp);
			call_index++;
		}
		g_slist_free(list);
	} else {
		*count = 0;
		*call_list = NULL;
	}

	return TELEPHONY_ERROR_NONE;
}

int telephony_call_release_call_list(unsigned int count, telephony_call_h **call_list)
{
	int i;
	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(call_list);

	if (count > 0) {
		for (i = 0; i < count; i++)
			g_free((telephony_call_info_s *)(*call_list)[i]);
		g_free(*call_list);
	}

	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_handle_id(telephony_call_h call_handle, unsigned int *handle_id)
{
	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(call_handle);
	CHECK_INPUT_PARAMETER(handle_id);

	*handle_id = ((telephony_call_info_s *)call_handle)->id;
	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_number(telephony_call_h call_handle, char **number)
{
	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(call_handle);
	CHECK_INPUT_PARAMETER(number);

	*number = g_strdup(((telephony_call_info_s *)call_handle)->number);
	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_type(telephony_call_h call_handle,
	telephony_call_type_e *type)
{
	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(call_handle);
	CHECK_INPUT_PARAMETER(type);

	*type = ((telephony_call_info_s *)call_handle)->type;
	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_status(telephony_call_h call_handle,
	telephony_call_status_e *status)
{
	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(call_handle);
	CHECK_INPUT_PARAMETER(status);

	*status = ((telephony_call_info_s *)call_handle)->status;
	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_direction(telephony_call_h call_handle,
	telephony_call_direction_e *direction)
{
	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(call_handle);
	CHECK_INPUT_PARAMETER(direction);

	*direction = ((telephony_call_info_s *)call_handle)->direction;
	return TELEPHONY_ERROR_NONE;
}

int telephony_call_get_conference_status(telephony_call_h call_handle, bool *conference_status)
{
	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(call_handle);
	CHECK_INPUT_PARAMETER(conference_status);

	*conference_status = ((telephony_call_info_s *)call_handle)->conference_status;
	return TELEPHONY_ERROR_NONE;
}
