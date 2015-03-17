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

#ifndef __CAPI_TELEPHONY_PRIVATE_H__
#define __CAPI_TELEPHONY_PRIVATE_H__

#include <gio/gio.h>
#include <glib.h>
#include <system_info.h>
#include "telephony_common.h"
#include "telephony_call.h"

#define TELEPHONY_FEATURE	"http://tizen.org/feature/network.telephony"

#define CHECK_TELEPHONY_SUPPORTED(feature_name) { \
	bool telephony_supported = FALSE; \
	if (!system_info_get_platform_bool(feature_name, &telephony_supported)) { \
		if (telephony_supported == FALSE) { \
			LOGE("telephony feature is disabled"); \
			return TELEPHONY_ERROR_NOT_SUPPORTED; \
		} \
	} else { \
		LOGE("Error - Feature getting from System Info"); \
		return TELEPHONY_ERROR_OPERATION_FAILED; \
	} \
}

typedef struct {
	GSList *evt_list;
	struct tapi_handle *tapi_h;
} telephony_data;

/*
 * Need to define the struct same as tapi_handle of libslp-tapi
 * because we are request dbus directly using dbus_connection, path
 */
struct tapi_handle {
	gpointer dbus_connection;
	char *path;
	char *cp_name;
	GHashTable *evt_list;
	char cookie[20];
	GCancellable *ca;

	GHashTable *cache_property;
	guint prop_callback_evt_id;
};

/**
 * @brief The structure type for the call information.
 * @since_tizen 2.4
 */
typedef struct {
	unsigned int id; /**< The handle of the call */
	char number[TELEPHONY_CALL_NUMBER_LEN_MAX + 1]; /**< Current Calling number */
	telephony_call_type_e type; /**< Type of call (voice, data, emergency) */
	telephony_call_state_e state; /**< Current Call state */
	telephony_call_direction_e direction; /**< Current Call direction (MO, MT) */
	bool conference_status; /**< true: Conference call, false: Single call */
} telephony_call_info_s;

#endif /* __CAPI_TELEPHONY_PRIVATE_H__ */
