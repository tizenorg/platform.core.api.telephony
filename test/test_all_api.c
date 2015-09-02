/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <glib.h>
#include <dlog.h>

#include <telephony.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_TELEPHONY_TEST"

static GMainLoop *event_loop;
static telephony_handle_list_s handle_list;

static int network_noti_tbl[] = {
	TELEPHONY_NOTI_NETWORK_SERVICE_STATE,
	TELEPHONY_NOTI_NETWORK_CELLID,
	TELEPHONY_NOTI_NETWORK_ROAMING_STATUS,
	TELEPHONY_NOTI_NETWORK_SIGNALSTRENGTH_LEVEL,
	TELEPHONY_NOTI_NETWORK_NETWORK_NAME,
	TELEPHONY_NOTI_NETWORK_PS_TYPE,
	TELEPHONY_NOTI_NETWORK_DEFAULT_DATA_SUBSCRIPTION,
	TELEPHONY_NOTI_NETWORK_DEFAULT_SUBSCRIPTION
};

static int call_noti_tbl[] = {
	TELEPHONY_NOTI_VOICE_CALL_STATUS_IDLE,
	TELEPHONY_NOTI_VOICE_CALL_STATUS_ACTIVE,
	TELEPHONY_NOTI_VOICE_CALL_STATUS_HELD,
	TELEPHONY_NOTI_VOICE_CALL_STATUS_DIALING,
	TELEPHONY_NOTI_VOICE_CALL_STATUS_ALERTING,
	TELEPHONY_NOTI_VOICE_CALL_STATUS_INCOMING,
	TELEPHONY_NOTI_CALL_PREFERRED_VOICE_SUBSCRIPTION
};

static const char *_mapping_sim_state(telephony_sim_state_e sim_state)
{
	switch (sim_state) {
	case TELEPHONY_SIM_STATE_AVAILABLE:
		return "Available";
	case TELEPHONY_SIM_STATE_LOCKED:
		return "Locked";
	case TELEPHONY_SIM_STATE_UNKNOWN:
		return "Unknown";
	default:
		return "Unavailable";
	}
}

static void sim_noti_cb(telephony_h handle, telephony_noti_e noti_id, void *data, void *user_data)
{
	LOGI("Noti!! SIM status: [%d]", *(int *)data);
}

static const char *_mapping_service_state(telephony_network_service_state_e service_state)
{
	switch (service_state) {
	case TELEPHONY_NETWORK_SERVICE_STATE_IN_SERVICE:
		return "In service";
	case TELEPHONY_NETWORK_SERVICE_STATE_OUT_OF_SERVICE:
		return "Out of service";
	case TELEPHONY_NETWORK_SERVICE_STATE_EMERGENCY_ONLY:
		return "Emergency only";
	default:
		return "Unknown state";
	}
}

static const char *_mapping_name_option(telephony_network_name_option_e option)
{
	switch (option) {
	case TELEPHONY_NETWORK_NAME_OPTION_NETWORK:
		return "By network";
	case TELEPHONY_NETWORK_NAME_OPTION_SPN:
		return "By SPN";
	case TELEPHONY_NETWORK_NAME_OPTION_ANY:
		return "By both";
	default:
		return "Unknown priority";
	}
}

static const char *_mapping_ps_type(telephony_network_ps_type_e ps_type)
{
	switch (ps_type) {
	case TELEPHONY_NETWORK_PS_TYPE_HSDPA:
		return "HSDPA";
	case TELEPHONY_NETWORK_PS_TYPE_HSUPA:
		return "HSUPA";
	case TELEPHONY_NETWORK_PS_TYPE_HSPA:
		return "HSPA";
	case TELEPHONY_NETWORK_PS_TYPE_HSPAP:
		return "HSPAP";
	default:
		return "Unknown";
	}
}

static const char *_mapping_default_data_sub(telephony_network_default_data_subs_e default_data_sub)
{
	switch (default_data_sub) {
	case TELEPHONY_NETWORK_DEFAULT_DATA_SUBS_SIM1:
		return "SIM1";
	case TELEPHONY_NETWORK_DEFAULT_DATA_SUBS_SIM2:
		return "SIM2";
	case TELEPHONY_NETWORK_DEFAULT_DATA_SUBS_UNKNOWN:
		return "Unknown";
	default:
		return "Unavailable";
	}
}

static const char *_mapping_default_sub(telephony_network_default_subs_e default_sub)
{
	switch (default_sub) {
	case TELEPHONY_NETWORK_DEFAULT_SUBS_SIM1:
		return "SIM1";
	case TELEPHONY_NETWORK_DEFAULT_SUBS_SIM2:
		return "SIM2";
	case TELEPHONY_NETWORK_DEFAULT_SUBS_UNKNOWN:
		return "Unknown";
	default:
		return "Unavailable";
	}
}

static const char *_mapping_network_type(telephony_network_type_e network_type)
{
	switch (network_type) {
	case TELEPHONY_NETWORK_TYPE_GSM:
		return "GSM";
	case TELEPHONY_NETWORK_TYPE_GPRS:
		return "GPRS";
	case TELEPHONY_NETWORK_TYPE_EDGE:
		return "EDGE";
	case TELEPHONY_NETWORK_TYPE_UMTS:
		return "UMTS";
	case TELEPHONY_NETWORK_TYPE_HSDPA:
		return "HSDPA";
	case TELEPHONY_NETWORK_TYPE_LTE:
		return "LTE";
	default:
		return "Unknown";
	}
}

static void network_noti_cb(telephony_h handle, telephony_noti_e noti_id, void *data, void *user_data)
{
	switch (noti_id) {
	case TELEPHONY_NOTI_NETWORK_SERVICE_STATE:
		LOGI("Noti!!! Service state: [%s]", _mapping_service_state(*(int *)data));
		break;
	case TELEPHONY_NOTI_NETWORK_CELLID:
		LOGI("Noti!!! Cell ID: [%d]", *(int *)data);
		break;
	case TELEPHONY_NOTI_NETWORK_ROAMING_STATUS:
		LOGI("Noti!!! Roaming status: [%s]", *(int *)data ? "ON" : "OFF");
		break;
	case TELEPHONY_NOTI_NETWORK_SIGNALSTRENGTH_LEVEL:
		LOGI("Noti!!! Signal strength: [%d]", *(int *)data);
		break;
	case TELEPHONY_NOTI_NETWORK_NETWORK_NAME:
		LOGI("Noti!!! Network Name: [%s]", *(int *)data);
		break;
	case TELEPHONY_NOTI_NETWORK_PS_TYPE:
		LOGI("Noti!!! PS Type: [%s]", _mapping_ps_type(*(int *)data));
		break;
	case TELEPHONY_NOTI_NETWORK_DEFAULT_DATA_SUBSCRIPTION:
		LOGI("Noti!!! Default Data Subscription: [%d]", *(int *)data);
		break;
	case TELEPHONY_NOTI_NETWORK_DEFAULT_SUBSCRIPTION:
		LOGI("Noti!!! Default Subscription: [%d]", *(int *)data);
		break;
	default:
		LOGE("Unknown noti");
		break;
	}

}

static const char *_mapping_call_state(telephony_call_state_e call_state)
{
	switch (call_state) {
	case TELEPHONY_CALL_STATE_IDLE:
		return "Idle";
	case TELEPHONY_CALL_STATE_CONNECTING:
		return "Connecting";
	case TELEPHONY_CALL_STATE_CONNECTED:
		return "Active";
	default:
		return "Unknown";
	}
}

static const char *_mapping_preferred_voice_sub(telephony_call_preferred_voice_subs_e call_sub)
{
	switch (call_sub) {
	case TELEPHONY_CALL_PREFERRED_VOICE_SUBS_CURRENT_NETWORK:
		return "CURRENT_NETWORK";
	case TELEPHONY_CALL_PREFERRED_VOICE_SUBS_ASK_ALWAYS:
		return "ASK_ALWAYS";
	case TELEPHONY_CALL_PREFERRED_VOICE_SUBS_SIM1:
		return "SIM1";
	case TELEPHONY_CALL_PREFERRED_VOICE_SUBS_SIM2:
		return "SIM2";
	case TELEPHONY_CALL_PREFERRED_VOICE_SUBS_UNKNOWN:
	default:
		return "Unknown";
	}
}

static void call_noti_cb(telephony_h handle, telephony_noti_e noti_id, void *data, void *user_data)
{
	int ret_value;
	telephony_call_h *call_list;
	unsigned int count = 0;

	if (noti_id == TELEPHONY_NOTI_CALL_PREFERRED_VOICE_SUBSCRIPTION) {
		LOGI("Noti!!! Preferred Voice Subscription: [%d]", *(int *)data);
		return;
	}

	switch (noti_id) {
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_IDLE:
		LOGI("Noti!!! Voice Call Status Idle!!! [%d]", *(int *)data);
		break;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_ACTIVE:
		LOGI("Noti!!! Voice Call Status Active!!! [%d]", *(int *)data);
		break;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_HELD:
		LOGI("Noti!!! Voice Call Status Held!!! [%d]", *(int *)data);
		break;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_DIALING:
		LOGI("Noti!!! Voice Call Status Dialing!!! [%d]", *(int *)data);
		break;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_ALERTING:
		LOGI("Noti!!! Voice Call Status Alerting!!! [%d]", *(int *)data);
		break;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_INCOMING:
		LOGI("Noti!!! Voice Call Status Incoming!!! [%d]", *(int *)data);
		break;
	default:
		LOGE("Unknown noti");
		break;
	}

	ret_value = telephony_call_get_call_list(handle_list.handle[0], &count, &call_list);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_call_get_call_list() failed!!!zz [0x%x]", ret_value);
	} else {
		int i;
		unsigned int handle_id = 0;
		char *number = NULL;
		telephony_call_type_e type = 0;
		telephony_call_status_e status = 0;
		telephony_call_direction_e direction = 0;
		bool conf_status = 0;

		for (i = 0; i < count; i++) {
			telephony_call_get_handle_id(call_list[i], &handle_id);
			telephony_call_get_number(call_list[i], &number);
			telephony_call_get_type(call_list[i], &type);
			telephony_call_get_status(call_list[i], &status);
			telephony_call_get_direction(call_list[i], &direction);
			telephony_call_get_conference_status(call_list[i], &conf_status);
			LOGI("id[%d] number[%s] type[%s] status[%s] direction[%s] conference_status[%s]",
				handle_id, number,
				type == TELEPHONY_CALL_TYPE_VOICE ? "VOICE" :
				type == TELEPHONY_CALL_TYPE_VIDEO ? "VIDEO" : "E911",
				status == TELEPHONY_CALL_STATUS_IDLE ? "IDLE" :
				status == TELEPHONY_CALL_STATUS_ACTIVE ? "ACTVIE" :
				status == TELEPHONY_CALL_STATUS_HELD ? "HELD" :
				status == TELEPHONY_CALL_STATUS_DIALING ? "DIALING" :
				status == TELEPHONY_CALL_STATUS_ALERTING ? "ALERTING" :
				status == TELEPHONY_CALL_STATUS_INCOMING ? "INCOMING" : "UNKNOWN",
				direction == TELEPHONY_CALL_DIRECTION_MO ? "MO" : "MT",
				conf_status ? "TRUE" : "FALSE");
			free(number);
			number = NULL;
		}
		telephony_call_release_call_list(count, &call_list);
	}

}

static void on_telephony_state_changed_cb(telephony_state_e state, void *user_data)
{
	LOGI("telephony state value : [%d]", state);
}

int main()
{
	int ret_value, i;

	/* SIM value */
	char *operator = NULL;
	char *icc_id = NULL;
	char *msin = NULL;
	char *spn = NULL;
	telephony_sim_state_e sim_state = 0;
	unsigned int app_list = 0;
	char *subscriber_number = NULL;
	char *subscriber_id = NULL;
	bool is_changed = FALSE;

	/* Network value */
	int cell_id = 0;
	int lac = 0;
	char *mcc = NULL;
	char *mnc = NULL;
	char *network_name = NULL;
	bool roaming_status;
	telephony_network_rssi_e rssi = 0;
	telephony_network_service_state_e service_state = 0;
	telephony_network_type_e network_type = 0;
	telephony_network_default_data_subs_e default_data_sub = 0;
	telephony_network_default_subs_e default_sub = 0;
	telephony_network_name_option_e network_name_option = 0;
	telephony_network_ps_type_e ps_type = 0;

	/* Call value */
	telephony_call_state_e call_state = 0;
	telephony_call_preferred_voice_subs_e call_sub = 0;
	telephony_call_h *call_list;
	unsigned int count = 0;

	/* Modem value */
	char *imei = NULL;
	telephony_modem_power_status_e power_status = 0;

	/* Ready */
	telephony_state_e ready_state = 0;

	/* Initialize handle */
	ret_value = telephony_init(&handle_list);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("Initialize failed!!!");
		return 0;
	}

	LOGI("handle count: [%d]", handle_list.count);

	/* SIM API */
	ret_value = telephony_sim_is_changed(handle_list.handle[0], &is_changed);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_sim_is_changed() failed!!! [%d]", ret_value);
	else
		LOGI("SIM is [%s]", is_changed ? "Changed" : "Not changed");

	ret_value = telephony_sim_get_operator(handle_list.handle[0], &operator);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_sim_get_operator() failed!!! [%d]", ret_value);
	} else {
		LOGI("Operator is [%s]", operator);
		free(operator);
	}

	ret_value = telephony_sim_get_icc_id(handle_list.handle[0], &icc_id);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_sim_get_icc_id() failed!!! [%d]", ret_value);
	} else {
		LOGI("ICC-ID is [%s]", icc_id);
		free(icc_id);
	}

	ret_value = telephony_sim_get_msin(handle_list.handle[0], &msin);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_sim_get_msin() failed!!! [%d]", ret_value);
	} else {
		LOGI("MSIN is [%s]", msin);
		free(msin);
	}

	ret_value = telephony_sim_get_spn(handle_list.handle[0], &spn);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_sim_get_spn() failed!!! [%d]", ret_value);
	} else {
		LOGI("SPN is [%s]", spn);
		free(spn);
	}

	ret_value = telephony_sim_get_state(handle_list.handle[0], &sim_state);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_sim_get_state() failed!!! [%d]", ret_value);
	else
		LOGI("SIM state is [%s]", _mapping_sim_state(sim_state));

	ret_value = telephony_sim_get_application_list(handle_list.handle[0], &app_list);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_sim_get_type() failed!!! [%d]", ret_value);
	} else {
		if (app_list & TELEPHONY_SIM_APP_TYPE_SIM)
			LOGI("Sim type [SIM]");
		if (app_list & TELEPHONY_SIM_APP_TYPE_USIM)
			LOGI("Sim type [USIM]");
		if (app_list & TELEPHONY_SIM_APP_TYPE_CSIM)
			LOGI("Sim type [CSIM]");
	}

	ret_value = telephony_sim_get_subscriber_number(handle_list.handle[0], &subscriber_number);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_sim_get_subscriber_number() failed!!! [%d]", ret_value);
	} else {
		LOGI("Subscriber number is [%s]", subscriber_number);
		free(subscriber_number);
	}

	ret_value = telephony_sim_get_subscriber_id(handle_list.handle[0], &subscriber_id);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_sim_get_subscriber_id() failed!!! [%d]", ret_value);
	} else {
		LOGI("Subscriber ID is [%s]", subscriber_id);
		free(subscriber_id);
	}

	/* Network API */
	ret_value = telephony_network_get_cell_id(handle_list.handle[0], &cell_id);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_cell_id() failed!!! [%d]", ret_value);
	else
		LOGI("Cell ID is [%d]", cell_id);

	ret_value = telephony_network_get_lac(handle_list.handle[0], &lac);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_lac() failed!!! [%d]", ret_value);
	else
		LOGI("Location Area Code is [%d]", lac);

	ret_value = telephony_network_get_mcc(handle_list.handle[0], &mcc);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_network_get_mcc() failed!!! [%d]", ret_value);
	} else {
		LOGI("Mobile Country Code is [%s]", mcc);
		free(mcc);
	}

	ret_value = telephony_network_get_mnc(handle_list.handle[0], &mnc);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_network_get_mnc() failed!!! [%d]", ret_value);
	} else {
		LOGI("Mobile Network Code is [%s]", mnc);
		free(mnc);
	}

	ret_value = telephony_network_get_network_name(handle_list.handle[0], &network_name);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_network_get_network_name() failed!!! [%d]", ret_value);
	} else {
		LOGI("Network name is [%s]", network_name);
		free(network_name);
	}

	ret_value = telephony_network_get_network_name_option(handle_list.handle[0], &network_name_option);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_network_name_option() failed!!! [%d]", ret_value);
	else
		LOGI("Network_name_option name is [%s]", _mapping_name_option(network_name_option));

	ret_value = telephony_network_get_roaming_status(handle_list.handle[0], &roaming_status);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_roaming_status() failed!!! [%d]", ret_value);
	else
		LOGI("Network Roaming: [%s]", roaming_status ? "ON" : "OFF");

	ret_value = telephony_network_get_rssi(handle_list.handle[0], &rssi);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_rssi() failed!!! [%d]", ret_value);
	else
		LOGI("Received Signal Strength Indicator is [%d]", rssi);

	ret_value = telephony_network_get_service_state(handle_list.handle[0], &service_state);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_service_state() failed!!! [%d]", ret_value);
	else
		LOGI("Network service state is [%s]", _mapping_service_state(service_state));

	ret_value = telephony_network_get_type(handle_list.handle[0], &network_type);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_type() failed!!! [%d]", ret_value);
	else
		LOGI("Network type is [%s]", _mapping_network_type(network_type));

	ret_value = telephony_network_get_ps_type(handle_list.handle[0], &ps_type);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_ps_type() failed!!! [%d]", ret_value);
	else
		LOGI("PS type is [%s]", _mapping_ps_type(ps_type));

	ret_value = telephony_network_get_default_data_subscription(handle_list.handle[0], &default_data_sub);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_default_data_subscription() failed!!! [%d]", ret_value);
	else
		LOGI("Default data subscription is [%s]", _mapping_default_data_sub(default_data_sub));

	ret_value = telephony_network_get_default_subscription(handle_list.handle[0], &default_sub);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_network_get_default_subscription() failed!!! [%d]", ret_value);
	else
		LOGI("Default subscription is [%s]", _mapping_default_sub(default_sub));

	/* Call API */
	ret_value = telephony_call_get_voice_call_state(handle_list.handle[0], &call_state);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_call_get_voice_call_state() failed!!! [%d]", ret_value);
	else
		LOGI("Voice Call state is [%s]", _mapping_call_state(call_state));

	ret_value = telephony_call_get_video_call_state(handle_list.handle[0], &call_state);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_call_get_video_call_state() failed!!! [%d]", ret_value);
	else
		LOGI("Video Call state is [%s]", _mapping_call_state(call_state));

	ret_value = telephony_call_get_preferred_voice_subscription(handle_list.handle[0], &call_sub);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_call_get_preferred_voice_subscription() failed!!! [%d]", ret_value);
	else
		LOGI("Preferred voice subscription is [%s]", _mapping_preferred_voice_sub(call_sub));

	ret_value = telephony_call_get_call_list(handle_list.handle[0], &count, &call_list);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_call_get_call_list() failed!!! [0x%x]", ret_value);
	} else {
		unsigned int handle_id = 0;
		char *number = NULL;
		telephony_call_type_e type = 0;
		telephony_call_status_e status = 0;
		telephony_call_direction_e direction = 0;
		bool conf_status = 0;

		for (i = 0; i < count; i++) {
			telephony_call_get_handle_id(call_list[i], &handle_id);
			telephony_call_get_number(call_list[i], &number);
			telephony_call_get_type(call_list[i], &type);
			telephony_call_get_status(call_list[i], &status);
			telephony_call_get_direction(call_list[i], &direction);
			telephony_call_get_conference_status(call_list[i], &conf_status);
			LOGI("handle_id[%d] number[%s] type[%d] status[%d] direction[%d] conf_status[%d]",
				handle_id, number, type, status, direction, conf_status);
			free(number);
			number = NULL;
		}
		telephony_call_release_call_list(count, &call_list);
	}

	/* Modem API */
	ret_value = telephony_modem_get_imei(handle_list.handle[0], &imei);
	if (ret_value != TELEPHONY_ERROR_NONE) {
		LOGE("telephony_modem_get_imei() failed!!! [%d]", ret_value);
	} else {
		LOGI("IMEI is [%s]", imei);
		free(imei);
	}

	ret_value = telephony_modem_get_power_status(handle_list.handle[0], &power_status);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_modem_get_power_status() failed!!! [%d]", ret_value);
	else
		LOGI("Modem power status is [%d] (0=on,1=off,2=rst,3=low)", power_status);

	/* set_noti_cb */
	ret_value = telephony_set_noti_cb(handle_list.handle[0], TELEPHONY_NOTI_SIM_STATUS, sim_noti_cb, NULL);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("Set noti failed!!!");

	for (i = 0; i < (sizeof(network_noti_tbl) / sizeof(int)); i++) {
		ret_value = telephony_set_noti_cb(handle_list.handle[0], network_noti_tbl[i], network_noti_cb, NULL);
		if (ret_value != TELEPHONY_ERROR_NONE)
			LOGE("Set noti failed!!!");
	}

	for (i = 0; i < (sizeof(call_noti_tbl) / sizeof(int)); i++) {
		ret_value = telephony_set_noti_cb(handle_list.handle[0], call_noti_tbl[i], call_noti_cb, NULL);
		if (ret_value != TELEPHONY_ERROR_NONE)
			LOGE("Set noti failed!!!");
	}

	ret_value = telephony_get_state(&ready_state);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGI("telephony_get_state() failed!!!");
	else
		LOGI("telephony_get_state() succeed ! : %s", ready_state ? "TRUE" : "FALSE");

	ret_value = telephony_set_state_changed_cb(on_telephony_state_changed_cb, NULL);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGI("telephony_set_state_changed_cb() failed!!!");
	else
		LOGI("telephony_set_state_changed_cb() succeed!!");

	LOGI("If telephony status is changed, then callback function will be called");
	event_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(event_loop);

	ret_value = telephony_unset_noti_cb(handle_list.handle[0], TELEPHONY_NOTI_SIM_STATUS);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("Unset noti failed!!!");

	for (i = 0; i < (sizeof(network_noti_tbl) / sizeof(int)); i++) {
		ret_value = telephony_unset_noti_cb(handle_list.handle[0], network_noti_tbl[i]);
		if (ret_value != TELEPHONY_ERROR_NONE)
			LOGE("Unset noti failed!!!");
	}

	for (i = 0; i < (sizeof(call_noti_tbl) / sizeof(int)); i++) {
		ret_value = telephony_unset_noti_cb(handle_list.handle[0], call_noti_tbl[i]);
		if (ret_value != TELEPHONY_ERROR_NONE)
			LOGE("Unset noti failed!!!");
	}

	ret_value = telephony_deinit(&handle_list);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("Deinitialize failed!!!");

	ret_value = telephony_unset_state_changed_cb(on_telephony_state_changed_cb);
	if (ret_value != TELEPHONY_ERROR_NONE)
		LOGE("telephony_unset_state_changed_cb() failed!!!");
	else
		LOGD("telephony_unset_state_changed_cb() succeed!!");

	return 0;
}
