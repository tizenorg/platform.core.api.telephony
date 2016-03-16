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
#include <gio/gio.h>
#include <dlog.h>

#include <tapi_common.h>
#include <ITapiModem.h>
#include "telephony_common.h"
#include "telephony_modem.h"
#include "telephony_private.h"

int telephony_modem_get_imei(telephony_h handle, char **imei)
{
	GVariant *gv = NULL;
	GError *gerr = NULL;
	int error = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(imei);

	gv = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_MODEM_INTERFACE,
		"GetIMEI", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &gerr);

	if (gv) {
		int tapi_result;
		char *tapi_imei = NULL;

		g_variant_get(gv, "(is)", &tapi_result, &tapi_imei);
		if (tapi_result == 0) {
			if (tapi_imei != NULL && strlen(tapi_imei) != 0)
				*imei = strdup(tapi_imei);
			else
				*imei = strdup("");
			g_free(tapi_imei);
		} else {
			LOGE("OPERATION_FAILED");
			error = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_variant_unref(gv);
	} else {
		/* LCOV_EXCL_START */
		LOGE("g_dbus_conn failed. error (%s)", gerr->message);
		if (strstr(gerr->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			error = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			error = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(gerr);
		/* LCOV_EXCL_STOP */
	}

	return error;
}

int telephony_modem_get_power_status(telephony_h handle,
	telephony_modem_power_status_e *status)
{
	TapiHandle *tapi_h;
	int ret;
	tapi_power_phone_power_status_t modem_status = TAPI_PHONE_POWER_STATUS_UNKNOWN;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(status);

	ret = tel_check_modem_power_status(tapi_h, &modem_status);
	if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		return TELEPHONY_ERROR_PERMISSION_DENIED;
	} else if (ret != TAPI_API_SUCCESS) {
		LOGE("OPERATION_FAILED");
		return TELEPHONY_ERROR_OPERATION_FAILED;
	} else {
		switch (modem_status) {
		case TAPI_PHONE_POWER_STATUS_ON:
			*status = TELEPHONY_MODEM_POWER_STATUS_ON;
			break;
		/* LCOV_EXCL_START */
		case TAPI_PHONE_POWER_STATUS_OFF:
			*status = TELEPHONY_MODEM_POWER_STATUS_OFF;
			break;
		case TAPI_PHONE_POWER_STATUS_RESET:
			*status = TELEPHONY_MODEM_POWER_STATUS_RESET;
			break;
		case TAPI_PHONE_POWER_STATUS_LOW:
			*status = TELEPHONY_MODEM_POWER_STATUS_LOW;
			break;
		case TAPI_PHONE_POWER_STATUS_UNKNOWN:
		case TAPI_PHONE_POWER_STATUS_ERROR:
		default:
			*status = TELEPHONY_MODEM_POWER_STATUS_UNKNOWN;
			break;
		/* LCOV_EXCL_STOP */
		}
	}

	LOGI("modem_power_status = %d (0=on,1=off,2=rst,3=low)", *status);

	return TELEPHONY_ERROR_NONE;

}

/* LCOV_EXCL_START */
int telephony_modem_get_meid(telephony_h handle, char **meid)
{
	int ret = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;
	GVariant *gv = NULL;
	GError *gerr = NULL;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(meid);

	gv = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_MODEM_INTERFACE,
		"GetSerialNumber", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &gerr);

	if (gv) {
		int tapi_result;
		char *tapi_esn = NULL, *tapi_meid = NULL, *tapi_imei = NULL, *tapi_imeisv = NULL;

		g_variant_get(gv, "(issss)",
			&tapi_result, &tapi_esn, &tapi_meid, &tapi_imei, &tapi_imeisv);
		if (tapi_result == 0) {
			if (tapi_meid != NULL && strlen(tapi_meid) != 0)
				*meid = strdup(tapi_meid);
			else
				*meid = strdup("");
			g_free(tapi_esn);
			g_free(tapi_meid);
			g_free(tapi_imei);
			g_free(tapi_imeisv);
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_variant_unref(gv);
	} else {
		LOGE("g_dbus_conn failed. error (%s)", gerr->message);
		if (strstr(gerr->message, "AccessDenied")) {
			LOGE("PERMISSION_DENIED");
			ret = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			ret = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_error_free(gerr);
	}

	return ret;
}
/* LCOV_EXCL_STOP */
