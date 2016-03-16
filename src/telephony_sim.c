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
#include <gio/gio.h>
#include <dlog.h>
#include <openssl/sha.h>

#include <tapi_common.h>
#include <TapiUtility.h>
#include <ITapiSim.h>
#include "telephony_sim.h"
#include "telephony_private.h"

#define DBUS_SIM_STATUS_ERROR "SIM STATUS ERROR"
#define DBUS_SIM_NOT_FOUND "SIM NOT FOUND"
#define DBUS_SIM_PERM_BLOCKED "SIM PERM BLOCKED"
#define DBUS_SIM_CARD_ERROR "SIM CARD ERROR"
#define DBUS_SIM_NOT_INITIALIZED "SIM NOT INITIALIZED"
#define DBUS_SIM_INIT_COMPLETED "SIM INIT COMPLETED"
#define DBUS_SIM_LOCKED "SIM LOCKED"
#define DBUS_SIM_NOT_READY "SIM NOT READY"
#define DBUS_SIM_RESPONSE_DATA_ERROR "SIM RESPONSE DATA ERROR"
#define DBUS_SIM_ACCESS_DENIED "AccessDenied"

#define GET_SIM_STATUS(tapi_h, sim_card_state) { \
	int card_changed = 0; \
	int ret = tel_get_sim_init_info(tapi_h, &sim_card_state, &card_changed); \
	if (ret == TAPI_API_ACCESS_DENIED) { \
		LOGE("PERMISSION_DENIED"); \
		return TELEPHONY_ERROR_PERMISSION_DENIED; \
	} else if (ret != TAPI_API_SUCCESS) { \
		LOGE("OPERATION_FAILED"); \
		return TELEPHONY_ERROR_OPERATION_FAILED; \
	} \
}

/* LCOV_EXCL_START */
static telephony_error_e _convert_dbus_errmsg_to_sim_error(gchar *err_msg)
{
	telephony_error_e ret = TELEPHONY_ERROR_OPERATION_FAILED;
	if (err_msg == NULL)
		return ret;

	if (strstr(err_msg, DBUS_SIM_NOT_FOUND)) {
		ret = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else if (strstr(err_msg, DBUS_SIM_PERM_BLOCKED)) {
		ret = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else if (strstr(err_msg, DBUS_SIM_CARD_ERROR)) {
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	} else if (strstr(err_msg, DBUS_SIM_NOT_INITIALIZED)) {
		ret = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else if (strstr(err_msg, DBUS_SIM_INIT_COMPLETED)) {
		ret = TELEPHONY_ERROR_NONE;
	} else if (strstr(err_msg, DBUS_SIM_LOCKED)) {
		ret = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else if (strstr(err_msg, DBUS_SIM_NOT_READY)) {
		ret = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else if (strstr(err_msg, DBUS_SIM_RESPONSE_DATA_ERROR)) {
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	} else if (strstr(err_msg, DBUS_SIM_ACCESS_DENIED)) {
		LOGE("PERMISSION_DENIED");
		ret = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		ret = TELEPHONY_ERROR_OPERATION_FAILED;
	}

	return ret;
}
/* LCOV_EXCL_STOP */

int telephony_sim_get_icc_id(telephony_h handle, char **icc_id)
{
	int error_code = TELEPHONY_ERROR_NONE;
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(icc_id);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	if (sim_card_state == TAPI_SIM_STATUS_CARD_ERROR
			|| sim_card_state == TAPI_SIM_STATUS_CARD_BLOCKED
			|| sim_card_state == TAPI_SIM_STATUS_CARD_NOT_PRESENT
			|| sim_card_state == TAPI_SIM_STATUS_CARD_REMOVED
			|| sim_card_state == TAPI_SIM_STATUS_UNKNOWN) {
		error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else {
		GError *gerr = NULL;
		GVariant *sync_gv = NULL;

		sync_gv = g_dbus_connection_call_sync(tapi_h->dbus_connection,
			DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_SIM_INTERFACE,
			"GetICCID", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &gerr);

		if (sync_gv) {
			TelSimAccessResult_t result = TAPI_SIM_ACCESS_SUCCESS;
			gchar *iccid = NULL;

			g_variant_get(sync_gv, "(is)", &result, &iccid);
			if (result == TAPI_SIM_ACCESS_SUCCESS) {
				if (iccid != NULL && strlen(iccid) != 0)
					*icc_id = strdup(iccid);
				else
					*icc_id = strdup("");
			} else {
				error_code = TELEPHONY_ERROR_OPERATION_FAILED;
			}
			g_free(iccid);
			g_variant_unref(sync_gv);
		} else {
			/* LCOV_EXCL_START */
			LOGE("g_dbus_conn failed. error (%s)", gerr->message);
			error_code = _convert_dbus_errmsg_to_sim_error(gerr->message);
			g_error_free(gerr);
			/* LCOV_EXCL_STOP */
		}
	}

	return error_code;
}

int telephony_sim_get_operator(telephony_h handle, char **sim_operator)
{
	int error_code = TELEPHONY_ERROR_NONE;
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(sim_operator);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	if (sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else {
		TelSimImsiInfo_t sim_imsi_info;
		int ret = tel_get_sim_imsi(tapi_h, &sim_imsi_info);
		if (ret == TAPI_API_SUCCESS) {
			int len = strlen(sim_imsi_info.szMcc) + strlen(sim_imsi_info.szMnc);
			*sim_operator = malloc(len + 1);
			snprintf(*sim_operator, len + 1, "%s%s", sim_imsi_info.szMcc, sim_imsi_info.szMnc);
			LOGI("SIM operator: [%s]", *sim_operator);
		} else if (ret == TAPI_API_ACCESS_DENIED) {
			LOGE("PERMISSION_DENIED");
			error_code = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			error_code = TELEPHONY_ERROR_OPERATION_FAILED;
		}
	}

	return error_code;
}

int telephony_sim_get_msin(telephony_h handle, char **msin)
{
	int error_code = TELEPHONY_ERROR_NONE;
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(msin);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	if (sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else {
		TelSimImsiInfo_t sim_imsi_info;
		int ret = tel_get_sim_imsi(tapi_h, &sim_imsi_info);
		if (ret == TAPI_API_SUCCESS) {
			*msin = strdup(sim_imsi_info.szMsin);
		} else if (ret == TAPI_API_ACCESS_DENIED) {
			LOGE("PERMISSION_DENIED");
			error_code = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			error_code = TELEPHONY_ERROR_OPERATION_FAILED;
		}
	}

	return error_code;
}

int telephony_sim_get_spn(telephony_h handle, char **spn)
{
	int error_code = TELEPHONY_ERROR_NONE;
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(spn);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	if (sim_card_state == TAPI_SIM_STATUS_CARD_ERROR
			|| sim_card_state == TAPI_SIM_STATUS_CARD_BLOCKED
			|| sim_card_state == TAPI_SIM_STATUS_CARD_NOT_PRESENT
			|| sim_card_state == TAPI_SIM_STATUS_CARD_REMOVED
			|| sim_card_state == TAPI_SIM_STATUS_UNKNOWN) {
		error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else {
		GError *gerr = NULL;
		GVariant *sync_gv = NULL;

		sync_gv = g_dbus_connection_call_sync(tapi_h->dbus_connection,
			DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_SIM_INTERFACE,
			"GetSpn", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &gerr);

		if (sync_gv) {
			TelSimAccessResult_t result = TAPI_SIM_ACCESS_SUCCESS;
			gchar *spn_str = NULL;
			guchar dc = 0;

			g_variant_get(sync_gv, "(iys)", &result, &dc, &spn_str);
			if (result == TAPI_SIM_ACCESS_SUCCESS) {
				if (spn_str != NULL && strlen(spn_str) != 0)
					*spn = strdup(spn_str);
				else
					*spn = strdup("");
				LOGI("SPN: [%s]", *spn);
			} else {
				error_code = TELEPHONY_ERROR_OPERATION_FAILED;
			}
			g_free(spn_str);
			g_variant_unref(sync_gv);
		} else {
			/* LCOV_EXCL_START */
			LOGE("g_dbus_conn failed. error (%s)", gerr->message);
			error_code = _convert_dbus_errmsg_to_sim_error(gerr->message);
			g_error_free(gerr);
			/* LCOV_EXCL_STOP */
		}
	}

	return error_code;
}

int telephony_sim_is_changed(telephony_h handle, bool *is_changed)
{
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	int error_code = TELEPHONY_ERROR_NONE;
	int ret;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(is_changed);

	ret = tel_get_sim_init_info(tapi_h, &sim_card_state, &card_changed);
	if (ret == TAPI_API_SUCCESS) {
		if (sim_card_state == TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
			*is_changed = card_changed;
		} else {
			LOGE("NOT_AVAILABLE");
			error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
		}
	/* LCOV_EXCL_START */
	} else if (ret == TAPI_API_ACCESS_DENIED) {
		LOGE("PERMISSION_DENIED");
		error_code = TELEPHONY_ERROR_PERMISSION_DENIED;
	} else {
		LOGE("OPERATION_FAILED");
		error_code = TELEPHONY_ERROR_OPERATION_FAILED;
	}
	/* LCOV_EXCL_STOP */

	return error_code;
}

int telephony_sim_get_state(telephony_h handle, telephony_sim_state_e *sim_state)
{
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	int error_code = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(sim_state);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	switch (sim_card_state) {
	/* LCOV_EXCL_START */
	case TAPI_SIM_STATUS_CARD_ERROR:
	case TAPI_SIM_STATUS_CARD_NOT_PRESENT:
	case TAPI_SIM_STATUS_CARD_REMOVED:
	case TAPI_SIM_STATUS_CARD_CRASHED:
		*sim_state = TELEPHONY_SIM_STATE_UNAVAILABLE;
		break;
	case TAPI_SIM_STATUS_SIM_PIN_REQUIRED:
	case TAPI_SIM_STATUS_SIM_PUK_REQUIRED:
	case TAPI_SIM_STATUS_CARD_BLOCKED:
	case TAPI_SIM_STATUS_SIM_NCK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_NSCK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_SPCK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_CCK_REQUIRED:
	case TAPI_SIM_STATUS_SIM_LOCK_REQUIRED:
		*sim_state = TELEPHONY_SIM_STATE_LOCKED;
		break;
	/* LCOV_EXCL_STOP */
	case TAPI_SIM_STATUS_SIM_INIT_COMPLETED:
		*sim_state = TELEPHONY_SIM_STATE_AVAILABLE;
		break;
	/* LCOV_EXCL_START */
	case TAPI_SIM_STATUS_UNKNOWN:
	case TAPI_SIM_STATUS_SIM_INITIALIZING:
	default:
		*sim_state = TELEPHONY_SIM_STATE_UNKNOWN;
		break;
	}
	/* LCOV_EXCL_STOP */

	return error_code;
}

int telephony_sim_get_application_list(telephony_h handle, unsigned int *app_list)
{
	int error_code = TELEPHONY_ERROR_NONE;
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(app_list);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	if (sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else {
		unsigned char tapi_app_list;
		int ret = tel_get_sim_application_list(tapi_h, &tapi_app_list);
		if (ret == TAPI_API_SUCCESS) {
			*app_list = (unsigned int)tapi_app_list;
			LOGI("SIM Application List: [0x%x]", *app_list);
		} else if (ret == TAPI_API_ACCESS_DENIED) {
			LOGE("PERMISSION_DENIED");
			error_code = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else if (ret != TAPI_API_SUCCESS) {
			LOGE("OPERATION_FAILED");
			error_code = TELEPHONY_ERROR_OPERATION_FAILED;
		}
	}

	return error_code;
}

int telephony_sim_get_subscriber_number(telephony_h handle, char **subscriber_number)
{
	int error_code = TELEPHONY_ERROR_NONE;
	GError *gerr = NULL;
	GVariant *sync_gv = NULL;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(subscriber_number);

	sync_gv = g_dbus_connection_call_sync(tapi_h->dbus_connection,
		DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_SIM_INTERFACE,
		"GetMSISDN", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &gerr);
	if (sync_gv) {
		TelSimAccessResult_t result = TAPI_SIM_ACCESS_SUCCESS;
		GVariantIter *iter = NULL;

		g_variant_get(sync_gv, "(iaa{sv})", &result, &iter);
		if (result == TAPI_SIM_ACCESS_SUCCESS) {
			gchar *key = NULL;
			GVariant *value = NULL;
			const gchar *str_value = NULL;
			GVariantIter *iter_row = NULL;

			while (g_variant_iter_next(iter, "a{sv}", &iter_row)) {
				while (g_variant_iter_loop(iter_row, "{sv}", &key, &value)) {
					if (!g_strcmp0(key, "number")) {
						str_value = g_variant_get_string(value, NULL);
						if (str_value != NULL && strlen(str_value) != 0)
							*subscriber_number = strdup(str_value);
					}
				}
				g_variant_iter_free(iter_row);
				/* Acquire only 1 subscriber number */
				break;
			}
			if (!*subscriber_number)
				*subscriber_number = strdup("");
		} else {
			error_code = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		g_variant_iter_free(iter);
		g_variant_unref(sync_gv);
	} else {
		/* LCOV_EXCL_START */
		LOGE("g_dbus_conn failed. error (%s)", gerr->message);
		error_code = _convert_dbus_errmsg_to_sim_error(gerr->message);
		g_error_free(gerr);
		/* LCOV_EXCL_STOP */
	}

	return error_code;
}

int telephony_sim_get_subscriber_id(telephony_h handle, char **subscriber_id)
{
	int error_code = TELEPHONY_ERROR_NONE;
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(subscriber_id);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	if (sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else {
		TelSimImsiInfo_t imsi_info;
		error_code = tel_get_sim_imsi(tapi_h, &imsi_info);
		if (error_code == TAPI_API_SUCCESS) {
			SHA256_CTX ctx;
			char *imsi;
			unsigned char md[SHA256_DIGEST_LENGTH];
			int i;

			imsi = g_strdup_printf("%s%s%s",
				imsi_info.szMcc, imsi_info.szMnc, imsi_info.szMsin);

			SHA256_Init(&ctx);
			SHA256_Update(&ctx, imsi, strlen(imsi));
			SHA256_Final(md, &ctx);

			*subscriber_id = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
			for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
				snprintf(*subscriber_id + (i * 2), 3,  "%02x", md[i]);
			LOGI("Subscriber ID: [%s]", *subscriber_id);
			g_free(imsi);
		/* LCOV_EXCL_START */
		} else if (error_code == TAPI_API_ACCESS_DENIED) {
			LOGE("get_subscriber_id: PERMISSION_DENIED");
			error_code = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("get_subscriber_id: OPERATION_FAILED");
			error_code = TELEPHONY_ERROR_OPERATION_FAILED;
		}
		/* LCOV_EXCL_STOP */
	}

	return error_code;
}

/* LCOV_EXCL_START */
int telephony_sim_get_lock_state(telephony_h handle, telephony_sim_lock_state_e *lock_state)
{
	TelSimCardStatus_t sim_card_state;
	int error_code = TELEPHONY_ERROR_NONE;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(lock_state);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	switch (sim_card_state) {
	case TAPI_SIM_STATUS_SIM_PIN_REQUIRED:
		*lock_state = TELEPHONY_SIM_LOCK_STATE_PIN_REQUIRED;
		break;
	case TAPI_SIM_STATUS_SIM_PUK_REQUIRED:
		*lock_state = TELEPHONY_SIM_LOCK_STATE_PUK_REQUIRED;
		break;
	case TAPI_SIM_STATUS_CARD_BLOCKED:
		*lock_state = TELEPHONY_SIM_LOCK_STATE_PERM_LOCKED;
		break;
	case TAPI_SIM_STATUS_SIM_NCK_REQUIRED:
		*lock_state = TELEPHONY_SIM_LOCK_STATE_NCK_REQUIRED;
		break;
	default:
		*lock_state = TELEPHONY_SIM_LOCK_STATE_UNKNOWN;
		break;
	}

	return error_code;
}

int telephony_sim_get_group_id1(telephony_h handle, char **gid1)
{
	int error_code = TELEPHONY_ERROR_NONE;
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(gid1);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	if (sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else {
		GError *gerr = NULL;
		GVariant *sync_gv = NULL;

		sync_gv = g_dbus_connection_call_sync(tapi_h->dbus_connection,
			DBUS_TELEPHONY_SERVICE, tapi_h->path, DBUS_TELEPHONY_SIM_INTERFACE,
			"GetGID", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &gerr);

		if (sync_gv) {
			TelSimAccessResult_t result = TAPI_SIM_ACCESS_SUCCESS;
			GVariant *gid_gv = NULL;
			TelSimGid_t tel_gid;

			g_variant_get(sync_gv, "(ii@v)", &result, &tel_gid.GroupIdentifierLen, &gid_gv);
			if (result == TAPI_SIM_ACCESS_SUCCESS) {
				if (tel_gid.GroupIdentifierLen == 0) {
					*gid1 = strdup("");
				} else {
					GVariant *inner_gv = NULL;
					GVariantIter *iter = NULL;
					int i = 0;
					guchar value = 0;

					inner_gv = g_variant_get_variant(gid_gv);
					g_variant_get(inner_gv, "ay", &iter);
					while (g_variant_iter_loop(iter, "y", &value) && i < TAPI_SIM_GROUP_IDENTIFIER_LEN_MAX) {
						tel_gid.szGroupIdentifier[i] = value;
						i++;
					}
					*gid1 = malloc(tel_gid.GroupIdentifierLen * 2 + 1);
					for (i = 0; i < tel_gid.GroupIdentifierLen; i++)
						snprintf(*gid1 + (i * 2), 3, "%02x", tel_gid.szGroupIdentifier[i]);
					g_variant_iter_free(iter);
					g_variant_unref(inner_gv);
				}
			} else {
				error_code = TELEPHONY_ERROR_OPERATION_FAILED;
			}
			g_variant_unref(gid_gv);
			g_variant_unref(sync_gv);
		} else {
			/* LCOV_EXCL_START */
			LOGE("g_dbus_conn failed. error (%s)", gerr->message);
			error_code = _convert_dbus_errmsg_to_sim_error(gerr->message);
			g_error_free(gerr);
			/* LCOV_EXCL_STOP */
		}
	}

	return error_code;
}

int telephony_sim_get_call_forwarding_indicator_state(telephony_h handle, bool *state)
{
	int error_code = TELEPHONY_ERROR_NONE;
	TelSimCardStatus_t sim_card_state = TAPI_SIM_STATUS_UNKNOWN;
	TapiHandle *tapi_h;

	CHECK_TELEPHONY_SUPPORTED(TELEPHONY_FEATURE);
	CHECK_INPUT_PARAMETER(handle);
	tapi_h = ((telephony_data *)handle)->tapi_h;
	CHECK_INPUT_PARAMETER(tapi_h);
	CHECK_INPUT_PARAMETER(state);
	GET_SIM_STATUS(tapi_h, sim_card_state);

	if (sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		error_code = TELEPHONY_ERROR_SIM_NOT_AVAILABLE;
	} else {
		int tel_state;
		int ret = tel_get_property_int(tapi_h, TAPI_PROP_SIM_CALL_FORWARD_STATE, &tel_state);
		if (ret == TAPI_API_SUCCESS) {
			*state = (bool)tel_state;
			LOGI("Call forwarding indicator state: [%s]", *state ? "ON" : "OFF");
		} else if (ret == TAPI_API_ACCESS_DENIED) {
			LOGE("PERMISSION_DENIED");
			error_code = TELEPHONY_ERROR_PERMISSION_DENIED;
		} else {
			LOGE("OPERATION_FAILED");
			error_code = TELEPHONY_ERROR_OPERATION_FAILED;
		}
	}

	return error_code;
}
/* LCOV_EXCL_STOP */
