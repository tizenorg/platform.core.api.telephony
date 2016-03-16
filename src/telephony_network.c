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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <dlog.h>
#include <tapi_common.h>
#include <ITapiNetwork.h>
#include <TapiUtility.h>
#include <TelNetwork.h>
#include "telephony_network.h"
#include "telephony_common.h"
#include "telephony_private.h"

int telephony_network_get_lac(telephony_h handle, int *lac)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(lac);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetLac", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		g_variant_get(dbus_result, "(i)", lac);
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(error);
	}

	return ret;
}

int telephony_network_get_cell_id(telephony_h handle, int *cell_id)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(cell_id);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetCellId", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		g_variant_get(dbus_result, "(i)", cell_id);
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(error);
	}

	return ret;
}

int telephony_network_get_rssi(telephony_h handle, telephony_network_rssi_e *rssi)
{
	int ret;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(rssi);

	ret = tel_get_property_int(tapi_h, TAPI_PROP_NETWORK_SIGNALSTRENGTH_LEVEL, (int *)rssi);
	if (ret == TAPI_API_SUCCESS) {
		LOGI("rssi:[%d]", *rssi);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_roaming_status(telephony_h handle, bool *status)
{
	int ret;
	int temp = 0;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(status);

	ret = tel_get_property_int(tapi_h, TAPI_PROP_NETWORK_ROAMING_STATUS, &temp);
	if (ret == TAPI_API_SUCCESS) {
		if (temp == 1)
			*status = true;
		else
			*status = false;
		LOGI("status:[%d]", *status);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_mcc(telephony_h handle, char **mcc)
{
	int ret = TELEPHONY_ERROR_NONE;
	char *plmn_str = NULL;
	int mcc_length = 3;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(mcc);

	ret = tel_get_property_string(tapi_h, TAPI_PROP_NETWORK_PLMN, &plmn_str);
	if (ret == TAPI_API_SUCCESS) {
		if (plmn_str != NULL && strlen(plmn_str) != 0) {
			*mcc = malloc(sizeof(char) * (mcc_length + 1));
			if (*mcc == NULL) {
				LOGE("OUT_OF_MEMORY");
				ret = TELEPHONY_ERROR_OUT_OF_MEMORY;
			} else {
				snprintf(*mcc, mcc_length + 1, "%s", plmn_str);
			}
		} else {
			*mcc = strdup("");
		}
		LOGI("MCC: [%s]", *mcc);
		g_free(plmn_str);
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_mnc(telephony_h handle, char **mnc)
{
	int ret = TELEPHONY_ERROR_NONE;
	char *plmn_str = NULL;
	int mcc_length = 3;
	int plmn_length;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(mnc);

	ret = tel_get_property_string(tapi_h, TAPI_PROP_NETWORK_PLMN, &plmn_str);
	if (ret == TAPI_API_SUCCESS) {
		if (plmn_str != NULL && strlen(plmn_str) != 0) {
			plmn_length = strlen(plmn_str);
			*mnc = malloc(sizeof(char) * (plmn_length - mcc_length + 1));
			if (*mnc == NULL) {
				LOGE("OUT_OF_MEMORY");
				ret = TELEPHONY_ERROR_OUT_OF_MEMORY;
			} else {
				snprintf(*mnc, plmn_length - mcc_length + 1, "%s", plmn_str + mcc_length);
			}
		} else {
			*mnc = strdup("");
		}
		LOGI("MNC: [%s]", *mnc);
		g_free(plmn_str);
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_network_name(telephony_h handle, char **network_name)
{
	int ret;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(network_name);

	ret = tel_get_property_string(tapi_h, TAPI_PROP_NETWORK_NETWORK_NAME, network_name);
	if (ret == TAPI_API_SUCCESS) {
		LOGI("network_name:[%s]", *network_name);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_network_name_option(telephony_h handle, telephony_network_name_option_e *network_name_option)
{
	int ret;
	int name_option = 0;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(network_name_option);

	ret = tel_get_property_int(tapi_h, TAPI_PROP_NETWORK_NAME_OPTION, &name_option);
	if (ret == TAPI_API_SUCCESS) {
		switch (name_option) {
		case TAPI_NETWORK_NAME_OPTION_SPN:
			*network_name_option = TELEPHONY_NETWORK_NAME_OPTION_SPN;
			break;
		case TAPI_NETWORK_NAME_OPTION_OPERATOR:
			*network_name_option = TELEPHONY_NETWORK_NAME_OPTION_NETWORK;
			break;
		case TAPI_NETWORK_NAME_OPTION_ANY:
			*network_name_option = TELEPHONY_NETWORK_NAME_OPTION_ANY;
			break;
		default:
			*network_name_option = TELEPHONY_NETWORK_NAME_OPTION_UNKNOWN;
			break;
		}

		LOGI("network_name_option:[%d]", *network_name_option);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_type(telephony_h handle, telephony_network_type_e *network_type)
{
	int ret;
	int act;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(network_type);

	ret = tel_get_property_int(tapi_h, TAPI_PROP_NETWORK_ACT, &act);
	if (ret == TAPI_API_SUCCESS) {
		switch (act) {
		case TAPI_NETWORK_SYSTEM_GSM:
		case TAPI_NETWORK_SYSTEM_PCS1900:
			*network_type = TELEPHONY_NETWORK_TYPE_GSM;
			break;
		case TAPI_NETWORK_SYSTEM_GPRS:
			*network_type = TELEPHONY_NETWORK_TYPE_GPRS;
			break;
		case TAPI_NETWORK_SYSTEM_EGPRS:
			*network_type = TELEPHONY_NETWORK_TYPE_EDGE;
			break;
		case TAPI_NETWORK_SYSTEM_UMTS:
		case TAPI_NETWORK_SYSTEM_GSM_AND_UMTS:
			*network_type = TELEPHONY_NETWORK_TYPE_UMTS;
			break;
		case TAPI_NETWORK_SYSTEM_HSDPA:
			*network_type = TELEPHONY_NETWORK_TYPE_HSDPA;
			break;
		case TAPI_NETWORK_SYSTEM_IS95A:
			*network_type = TELEPHONY_NETWORK_TYPE_IS95A;
			break;
		case TAPI_NETWORK_SYSTEM_IS95B:
			*network_type = TELEPHONY_NETWORK_TYPE_IS95B;
			break;
		case TAPI_NETWORK_SYSTEM_CDMA_1X:
			*network_type = TELEPHONY_NETWORK_TYPE_CDMA_1X;
			break;
		case TAPI_NETWORK_SYSTEM_EVDO_REV_0:
		case TAPI_NETWORK_SYSTEM_1X_EVDO_REV_0_HYBRID:
			*network_type = TELEPHONY_NETWORK_TYPE_EVDO_REV_0;
			break;
		case TAPI_NETWORK_SYSTEM_EVDO_REV_A:
		case TAPI_NETWORK_SYSTEM_1X_EVDO_REV_A_HYBRID:
			*network_type = TELEPHONY_NETWORK_TYPE_EVDO_REV_A;
			break;
		case TAPI_NETWORK_SYSTEM_EVDO_REV_B:
		case TAPI_NETWORK_SYSTEM_1X_EVDO_REV_B_HYBRID:
			*network_type = TELEPHONY_NETWORK_TYPE_EVDO_REV_B;
			break;
		case TAPI_NETWORK_SYSTEM_EVDV:
			*network_type = TELEPHONY_NETWORK_TYPE_EVDV;
			break;
		case TAPI_NETWORK_SYSTEM_EHRPD:
			*network_type = TELEPHONY_NETWORK_TYPE_EHRPD;
			break;
		case TAPI_NETWORK_SYSTEM_LTE:
			*network_type = TELEPHONY_NETWORK_TYPE_LTE;
			break;
		default:
			*network_type = TELEPHONY_NETWORK_TYPE_UNKNOWN;
			break;
		}

		LOGI("network_type:[%d]", *network_type);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_ps_type(telephony_h handle, telephony_network_ps_type_e *ps_type)
{
	int ret;
	int service_type = 0;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(ps_type);

	ret = tel_get_property_int(tapi_h, TAPI_PROP_NETWORK_PS_TYPE, &service_type);
	if (ret == TAPI_API_SUCCESS) {
		switch (service_type) {
		case TAPI_NETWORK_PS_TYPE_HSDPA:
			*ps_type = TELEPHONY_NETWORK_PS_TYPE_HSDPA;
			break;
		case TAPI_NETWORK_PS_TYPE_HSUPA:
			*ps_type = TELEPHONY_NETWORK_PS_TYPE_HSUPA;
			break;
		case TAPI_NETWORK_PS_TYPE_HSPA:
			*ps_type = TELEPHONY_NETWORK_PS_TYPE_HSPA;
			break;
		case TAPI_NETWORK_PS_TYPE_HSPAP:
			*ps_type = TELEPHONY_NETWORK_PS_TYPE_HSPAP;
			break;
		default:
			*ps_type = TELEPHONY_NETWORK_PS_TYPE_UNKNOWN;
			break;
		}

		LOGI("ps_type:[%d]", *ps_type);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_service_state(telephony_h handle, telephony_network_service_state_e *network_service_state)
{
	int ret;
	int service_type = 0;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(network_service_state);

	ret = tel_get_property_int(tapi_h, TAPI_PROP_NETWORK_SERVICE_TYPE, &service_type);
	if (ret == TAPI_API_SUCCESS) {
		switch (service_type) {
		case TAPI_NETWORK_SERVICE_TYPE_UNKNOWN:
		case TAPI_NETWORK_SERVICE_TYPE_NO_SERVICE:
		case TAPI_NETWORK_SERVICE_TYPE_SEARCH:
			*network_service_state = TELEPHONY_NETWORK_SERVICE_STATE_OUT_OF_SERVICE;
			break;
		case TAPI_NETWORK_SERVICE_TYPE_EMERGENCY:
			*network_service_state = TELEPHONY_NETWORK_SERVICE_STATE_EMERGENCY_ONLY;
			break;
		default:
			*network_service_state = TELEPHONY_NETWORK_SERVICE_STATE_IN_SERVICE;
			break;
		}
		LOGI("network_service_state:[%d]", *network_service_state);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_default_data_subscription(telephony_h handle,
	telephony_network_default_data_subs_e *default_data_sub)
{
	TapiHandle *tapi_h;
	int ret;
	TelNetworkDefaultDataSubs_t default_data_subscription = TAPI_NETWORK_DEFAULT_DATA_SUBS_UNKNOWN;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(default_data_sub);

	ret = tel_get_network_default_data_subscription(tapi_h,  &default_data_subscription);
	if (ret == TAPI_API_SUCCESS) {
		switch (default_data_subscription) {
		case TAPI_NETWORK_DEFAULT_DATA_SUBS_SIM1:
			*default_data_sub = TELEPHONY_NETWORK_DEFAULT_DATA_SUBS_SIM1;
			break;
		case TAPI_NETWORK_DEFAULT_DATA_SUBS_SIM2:
			*default_data_sub = TELEPHONY_NETWORK_DEFAULT_DATA_SUBS_SIM2;
			break;
		case TAPI_NETWORK_DEFAULT_DATA_SUBS_UNKNOWN:
		default:
			*default_data_sub = TELEPHONY_NETWORK_DEFAULT_DATA_SUBS_UNKNOWN;
			break;
		}
		LOGI("default data subscription: [%d]", *default_data_sub);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_default_subscription(telephony_h handle,
	telephony_network_default_subs_e *default_sub)
{
	TapiHandle *tapi_h;
	int ret;
	TelNetworkDefaultSubs_t default_subscription = TAPI_NETWORK_DEFAULT_SUBS_UNKNOWN;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(default_sub);

	ret = tel_get_network_default_subscription(tapi_h,  &default_subscription);
	if (ret == TAPI_API_SUCCESS) {
		switch (default_subscription) {
		case TAPI_NETWORK_DEFAULT_SUBS_SIM1:
			*default_sub = TELEPHONY_NETWORK_DEFAULT_SUBS_SIM1;
			break;
		case TAPI_NETWORK_DEFAULT_SUBS_SIM2:
			*default_sub = TELEPHONY_NETWORK_DEFAULT_SUBS_SIM2;
			break;
		case TAPI_NETWORK_DEFAULT_SUBS_UNKNOWN:
		default:
			*default_sub = TELEPHONY_NETWORK_DEFAULT_SUBS_UNKNOWN;
			break;
		}
		LOGI("default subscription: [%d]", *default_sub);
		ret = TELEPHONY_ERROR_NONE;
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}

int telephony_network_get_selection_mode(telephony_h handle, telephony_network_selection_mode_e *mode)
{
	int ret = TELEPHONY_ERROR_OPERATION_FAILED;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(mode);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetSelectionMode", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		int tapi_mode;
		int tapi_result;
		g_variant_get(dbus_result, "(ii)", &tapi_mode, &tapi_result);
		if (tapi_result == 0) {
			if (tapi_mode == TAPI_NETWORK_SELECTIONMODE_AUTOMATIC)
				*mode = TELEPHONY_NETWORK_SELECTION_MODE_AUTOMATIC;
			else
				*mode = TELEPHONY_NETWORK_SELECTION_MODE_MANUAL;
			ret = TELEPHONY_ERROR_NONE;
		}
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		}
		g_error_free(error);
	}

	return ret;
}

int telephony_network_get_tac(telephony_h handle, int *tac)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(tac);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetTac", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		g_variant_get(dbus_result, "(i)", tac);
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(error);
	}

	return ret;
}

int telephony_network_get_system_id(telephony_h handle, int *sid)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(sid);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetSystemId", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		g_variant_get(dbus_result, "(i)", sid);
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(error);
	}

	return ret;
}

int telephony_network_get_network_id(telephony_h handle, int *nid)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(nid);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetNetworkId", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		g_variant_get(dbus_result, "(i)", nid);
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(error);
	}

	return ret;
}

int telephony_network_get_base_station_id(telephony_h handle, int *bs_id)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(bs_id);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetBsId", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		g_variant_get(dbus_result, "(i)", bs_id);
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(error);
	}

	return ret;
}

int telephony_network_get_base_station_latitude(telephony_h handle, int *bs_latitude)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(bs_latitude);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetBsLatitude", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		g_variant_get(dbus_result, "(i)", bs_latitude);
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(error);
	}

	return ret;
}

int telephony_network_get_base_station_longitude(telephony_h handle, int *bs_longitude)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GError *error = NULL;
	GVariant *dbus_result;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(bs_longitude);

	dbus_result = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_NETWORK_INTERFACE,
		"GetBsLongitude", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

	if (dbus_result) {
		g_variant_get(dbus_result, "(i)", bs_longitude);
		g_variant_unref(dbus_result);
	} else {
		LOGE("g_dbus_connection_call_sync() failed. (%s)", error->message);
		if (strstr(error->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(error);
	}

	return ret;
}
