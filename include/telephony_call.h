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

#ifndef __CAPI_TELEPHONY_CALL_H__
#define __CAPI_TELEPHONY_CALL_H__

/**
 * @file telephony_call.h
 * @brief This file contains call APIs and related enumeration.
 */

#include "telephony_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup CAPI_TELEPHONY_INFORMATION_CALL
 * @{
 */

/**
 * @brief The call handle.
 * @since_tizen 2.4
 */
typedef struct telephony_call_info_s *telephony_call_h;

/**
 * @deprecated Deprecated Since 2.4. Use #telephony_call_status_e instead.
 *
 * @brief Enumeration for the call state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	TELEPHONY_CALL_STATE_IDLE,       /**< There exists no calls. */
	TELEPHONY_CALL_STATE_CONNECTING, /**< There exists at least one call that is dialing, alerting or incoming */
	TELEPHONY_CALL_STATE_CONNECTED,  /**< There exist active or held calls, and no calls are dialing, alerting or incoming */
} telephony_call_state_e;

/**
 * @brief Enumeration for the call status.
 * @since_tizen 2.4
 */
typedef enum {
	TELEPHONY_CALL_STATUS_IDLE, /**< Idle status */
	TELEPHONY_CALL_STATUS_ACTIVE, /**< Active status */
	TELEPHONY_CALL_STATUS_HELD, /**< Held status */
	TELEPHONY_CALL_STATUS_DIALING, /**< Dialing status */
	TELEPHONY_CALL_STATUS_ALERTING, /**< Alerting status */
	TELEPHONY_CALL_STATUS_INCOMING, /**< Incoming status */
} telephony_call_status_e;

/**
 * @brief Enumeration for the preferred voice call subscription.
 * @since_tizen 2.4
 */
typedef enum {
	TELEPHONY_CALL_PREFERRED_VOICE_SUBS_UNKNOWN = -1, /**< Unknown status */
	TELEPHONY_CALL_PREFERRED_VOICE_SUBS_CURRENT_NETWORK = 0, /**< Current network */
	TELEPHONY_CALL_PREFERRED_VOICE_SUBS_ASK_ALWAYS, /**< ASK Always */
	TELEPHONY_CALL_PREFERRED_VOICE_SUBS_SIM1, /**< SIM 1 */
	TELEPHONY_CALL_PREFERRED_VOICE_SUBS_SIM2, /**<  SIM 2 */
} telephony_call_preferred_voice_subs_e;

/**
 * @brief Enumeration for the call type.
 * @since_tizen 2.4
 */
typedef enum {
	TELEPHONY_CALL_TYPE_VOICE, /**< Voice call */
	TELEPHONY_CALL_TYPE_VIDEO, /**< Video call */
	TELEPHONY_CALL_TYPE_E911, /**< Emergency call */
} telephony_call_type_e;

/**
 * @brief Enumeration for the call direction.
 * @since_tizen 2.4
 */
typedef enum {
	TELEPHONY_CALL_DIRECTION_MO, /**< MO(Mobile Originated) call */
	TELEPHONY_CALL_DIRECTION_MT, /**< MT(Mobile Terminated) call */
} telephony_call_direction_e;

/**
 * @deprecated Deprecated Since 2.4. Use #telephony_call_get_status instead.
 *
 * @brief Gets the voice call state of the telephony service.
 * @details Determines if the voice call is connecting, connected, or idle.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks When you are dialing a number or a new voice call is ringing,
 *          the state of the voice call is #TELEPHONY_CALL_STATE_CONNECTING.
 *          When a new voice call is connecting while another voice call is already connected,
 *          the state of the voice call is #TELEPHONY_CALL_STATE_CONNECTING as well.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] call_state The current state of the voice call
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 */
int telephony_call_get_voice_call_state(telephony_h handle, telephony_call_state_e *call_state);

/**
 * @deprecated Deprecated Since 2.4. Use #telephony_call_get_status instead.
 *
 * @brief Gets the video call state of the telephony service.
 * @details Determines if the video call is connecting, connected, or idle.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks When you are dialing a number or a new video call is ringing,
 *          the state of the video call is #TELEPHONY_CALL_STATE_CONNECTING.
 *          If a video call is connected, then the state of any other call cannot be
 *          #TELEPHONY_CALL_STATE_CONNECTING and #TELEPHONY_CALL_STATE_CONNECTED.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] call_state The current state of the video call
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 */
int telephony_call_get_video_call_state(telephony_h handle, telephony_call_state_e *call_state);

/**
 * @brief Gets the current value for the preferred voice call subscription.
 *
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] call_sub The currently set preferred call subscription value. (telephony_call_preferred_voice_subs_e)
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 */
int telephony_call_get_preferred_voice_subscription(telephony_h handle, telephony_call_preferred_voice_subs_e *call_sub);

/**
 * @brief Gets the list of the current call.
 *
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks You must release @c call_list using telephony_call_release_call_list().
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] count Count of the existing calls.
 * @param[out] call_list List of call information for existing calls.
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Operation failed
 */
int telephony_call_get_call_list(telephony_h handle,
	unsigned int *count, telephony_call_h **call_list);

/**
 * @brief Releases the list allocated from #telephony_call_get_call_list()
 *
 * @since_tizen 2.4
 *
 * @param[in] count The count of the calls from #telephony_call_get_call_list()
 * @param[in] call_list The handle from #telephony_call_get_call_list()
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre The call list should be validated from #telephony_call_get_call_list()
 */
int telephony_call_release_call_list(unsigned int count, telephony_call_h **call_list);

/**
 * @brief Gets the call handle ID.
 *
 * @since_tizen 2.4
 *
 * @param[in] call_handle The handle from #telephony_call_get_call_list()
 * @param[out] handle_id The id of the call handle
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre The call handle should be obtained from #telephony_call_get_call_list()
 * @post The call list should be released by using #telephony_call_release_call_list()
 */
int telephony_call_get_handle_id(telephony_call_h call_handle, unsigned int *handle_id);

/**
 * @brief Gets the call number.
 *
 * @since_tizen 2.4
 *
 * @remarks You must release @c number using free().
 *
 * @param[in] call_handle The handle from #telephony_call_get_call_list()
 * @param[out] number The number of the call
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre The call handle should be obtained from #telephony_call_get_call_list()
 * @post The call list should be released by using #telephony_call_release_call_list()
 */
int telephony_call_get_number(telephony_call_h call_handle, char **number);

/**
 * @brief Gets the call type.
 *
 * @since_tizen 2.4
 *
 * @param[in] call_handle The handle from #telephony_call_get_call_list()
 * @param[out] type The type of the call (#telephony_call_type_e)
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre The call handle should be obtained from #telephony_call_get_call_list()
 * @post The call list should be released by using #telephony_call_release_call_list()
 */
int telephony_call_get_type(telephony_call_h call_handle, telephony_call_type_e *type);

/**
 * @brief Gets the call status.
 *
 * @since_tizen 2.4
 *
 * @param[in] call_handle The handle from #telephony_call_get_call_list()
 * @param[out] status The status of the call (#telephony_call_status_e)
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre The call handle should be obtained from #telephony_call_get_call_list()
 * @post The call list should be released by using #telephony_call_release_call_list()
 */
int telephony_call_get_status(telephony_call_h call_handle, telephony_call_status_e *status);

/**
 * @brief Gets whether the call is MO(Mobile Originated) call or MT(Mobile Terminated).
 * @since_tizen 2.4
 *
 * @param[in] call_handle The handle from #telephony_call_get_call_list()
 * @param[out] direction The direction of the call (#telephony_call_direction_e)
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre The call handle should be obtained from #telephony_call_get_call_list()
 * @post The call list should be released by using #telephony_call_release_call_list()
 */
int telephony_call_get_direction(telephony_call_h call_handle, telephony_call_direction_e *direction);

/**
 * @brief Gets whether the call is conference call or not.
 * @since_tizen 2.4
 *
 * @param[in] call_handle The handle from #telephony_call_get_call_list()
 * @param[out] conference_status The value whether the call is conference call or not.
 *                            (true: Conference call, false: Single call)
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre The call handle should be obtained from #telephony_call_get_call_list()
 * @post The call list should be released by using #telephony_call_release_call_list()
 */
int telephony_call_get_conference_status(telephony_call_h call_handle, bool *conference_status);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // __CAPI_TELEPHONY_CALL_H__
