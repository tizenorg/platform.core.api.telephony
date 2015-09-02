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

#ifndef __CAPI_TELEPHONY_COMMON_H__
#define __CAPI_TELEPHONY_COMMON_H__

/**
 * @file telephony_common.h
 * @brief This file contains telephony common APIs and related enumerations.
 */

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file telephony_common.h
 * @brief This file contains the common information APIs.
 */

/**
 * @addtogroup CAPI_TELEPHONY_INFORMATION
 * @{
 */

/**
 * @brief The Telephony API handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct telephony_data *telephony_h;

/**
 * @brief The structure type for the list of handles to use the Telephony API.
 * @remarks There are two handles in case of dual SIM device. \n
 *          In this case, handle[0] is mean Primary SIM and handle[1] is mean Secondary SIM.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct {
    unsigned int count; /**< Number of available handle */
    telephony_h *handle; /**< Handle to use Telephony API */
} telephony_handle_list_s;

/**
 * @brief Enumeration for Telephony error.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
    TELEPHONY_ERROR_NONE = TIZEN_ERROR_NONE,                            /**< Successful */
    TELEPHONY_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,          /**< Out of memory */
    TELEPHONY_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,  /**< Invalid parameter */
    TELEPHONY_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED,  /**< Permission denied */
    TELEPHONY_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED,          /**< Not supported */
    TELEPHONY_ERROR_OPERATION_FAILED = TIZEN_ERROR_TELEPHONY | 0x0001,  /**< Operation failed */
    TELEPHONY_ERROR_SIM_NOT_AVAILABLE = TIZEN_ERROR_TELEPHONY | 0x1001, /**< SIM is not available */
} telephony_error_e;

/**
 * @brief Enumeration for Telephony notification.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum  {
    TELEPHONY_NOTI_SIM_STATUS = 0x10,            /**< Notification to be invoked when the SIM card state changes, '#telephony_sim_state_e' will be delivered in notification data */

    TELEPHONY_NOTI_NETWORK_SERVICE_STATE = 0x20, /**< Notification to be invoked when the network service state changes, '#telephony_network_service_state_e' will be delivered in notification data */
    TELEPHONY_NOTI_NETWORK_CELLID,               /**< Notification to be invoked when the cell ID changes, 'cell_id(int)' will be delivered in notification data */
    TELEPHONY_NOTI_NETWORK_ROAMING_STATUS,       /**< Notification to be invoked when the roaming status changes, 'roaming_status(bool)' will be delivered in notification data */
    TELEPHONY_NOTI_NETWORK_SIGNALSTRENGTH_LEVEL, /**< Notification to be invoked when the signal strength changes, '#telephony_network_rssi_e' will be delivered in notification data */
    TELEPHONY_NOTI_NETWORK_NETWORK_NAME, /**< Notification to be invoked when the network name changes, 'network_name(char *)' wil be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_NETWORK_PS_TYPE, /**< Notification to be invoked when the ps type changes, '#telephony_network_ps_type_e' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_NETWORK_DEFAULT_DATA_SUBSCRIPTION, /**< Notification to be invoked when the default data subscription changes, '#telephony_network_default_data_subs_e' wil be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_NETWORK_DEFAULT_SUBSCRIPTION, /**< Notification to be invoked when the default subscription changes, '#telephony_network_default_subs_e' will be delivered in notification data (@b Since: 2.4) */

    TELEPHONY_NOTI_VOICE_CALL_STATE = 0x30,      /**< Notification to be invoked when the voice call state changes, '#telephony_call_state_e' will be delivered in notification data (Deprecated Since 2.4. Use #TELEPHONY_NOTI_VOICE_CALL_STATUS_XXX instead.) */
    TELEPHONY_NOTI_VIDEO_CALL_STATE,              /**< Notification to be invoked when the video call state changes, '#telephony_call_state_e' will be delivered in notification data (Deprecated Since 2.4. Use #TELEPHONY_NOTI_VIDEO_CALL_STATUS_XXX instead.) */
    TELEPHONY_NOTI_VOICE_CALL_STATUS_IDLE, /**< Notification to be invoked when a voice call is in idle status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VOICE_CALL_STATUS_ACTIVE, /**< Notification to be invoked when a voice call is in active status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VOICE_CALL_STATUS_HELD, /**< Notification to be invoked when a voice call is in held status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VOICE_CALL_STATUS_DIALING, /**< Notification to be invoked when a voice call is in dialing status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VOICE_CALL_STATUS_ALERTING, /**< Notification to be invoked when a voice call is in alerting status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VOICE_CALL_STATUS_INCOMING, /**< Notification to be invoked when a voice call is in incoming status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VIDEO_CALL_STATUS_IDLE, /**< Notification to be invoked when a video call is in idle status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VIDEO_CALL_STATUS_ACTIVE, /**< Notification to be invoked when a video call is in active status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VIDEO_CALL_STATUS_DIALING, /**< Notification to be invoked when a video call is in dialing status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VIDEO_CALL_STATUS_ALERTING, /**< Notification to be invoked when a video call is in alerting status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_VIDEO_CALL_STATUS_INCOMING, /**< Notification to be invoked when a video call is in incoming status, 'handle_id(unsigned int)' will be delivered in notification data (@b Since: 2.4) */
    TELEPHONY_NOTI_CALL_PREFERRED_VOICE_SUBSCRIPTION /**< Notification to be invoked when the preferred voice subscription changes, '#telephony_call_preferred_voice_subs_e' will be delivered in notification data (@b Since: 2.4) */
} telephony_noti_e;

/**
 * @brief Called when the telephony state changes.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef void (*telephony_noti_cb)(telephony_h handle, telephony_noti_e noti_id, void *data, void *user_data);

/**
 * @brief Sets a callback function to be invoked when the telephony state changes.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @param[in] handle  The handle to use the telephony API
 * @param[in] noti_id The notification ID to set the callback
 * @param[in] cb The callback to be invoked when the telephony state changes
 * @param[in] user_data The user data passed to the callback function
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 *
 * @post telephony_noti_cb() will be invoked.
 *
 * @see telephony_unset_noti_cb()
 */
int telephony_set_noti_cb(telephony_h handle,
    telephony_noti_e noti_id, telephony_noti_cb cb, void *user_data);

/**
 * @brief Unsets a callback function.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @param[in] handle  The handle to use the telephony API
 * @param[in] noti_id The notification ID to unset a callback
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 *
 * @see telephony_set_noti_cb()
 */
int telephony_unset_noti_cb(telephony_h handle, telephony_noti_e noti_id);

/**
 * @brief Acquires the list of available handles to use the telephony API.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks You will get two handles in case of dual SIM device. \n
 *          In this case, handle[0] is mean Primary SIM and handle[1] is mean Secondary SIM.
 *
 * @param[out] list The list contains the number of
 *                  available handles and array of handles
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 *
 * @see telephony_deinit()
 */
int telephony_init(telephony_handle_list_s *list);

/**
 * @brief Deinitializes the telephony handle list.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] list The handle list to be deinitialized
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 *
 * @see telephony_init()
 */
int telephony_deinit(telephony_handle_list_s *list);

/**
 * @brief Enumeration for the telephony state.
 * @since_tizen 2.4
 */
typedef enum {
    TELEPHONY_STATE_NOT_READY, /**< Telephony state is not ready */
    TELEPHONY_STATE_READY, /**< Telephony state is ready */
} telephony_state_e;

/**
 * @brief Called for the telephony state changes.
 * @since_tizen 2.4
 */
typedef void (*telephony_state_changed_cb)(telephony_state_e state, void *user_data);

/**
 * @brief Acquires the telephony state value
 *
 * @since_tizen 2.4
 *
 * @param[out] state The state value of telephony.
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 *
 * @see telephony_set_state_changed_cb()
 * @see telephony_unset_state_changed_cb()
 */
int telephony_get_state(telephony_state_e *state);

/**
 * @brief Sets a callback function to be invoked when the telephony state changes.
 *
 * @since_tizen 2.4
 *
 * @param[in] callback The callback to be invoked when the telephony state changes
 * @param[in] user_data The user data passed to the callback function
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 *
 * @post telephony_state_changed_cb() will be invoked.
 *
 * @see telephony_unset_state_changed_cb()
 */
int telephony_set_state_changed_cb(telephony_state_changed_cb callback, void *user_data);

/**
 * @brief Unsets a telephony state callback function.
 *
 * @since_tizen 2.4
 *
 * @param[in] callback The callback to unset when the telephony state changes
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 *
 * @see telephony_set_state_changed_cb()
 */
int telephony_unset_state_changed_cb(telephony_state_changed_cb callback);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CAPI_TELEPHONY_COMMON_H__ */
