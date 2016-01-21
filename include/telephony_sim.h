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

#ifndef __CAPI_TELEPHONY_SIM_H__
#define __CAPI_TELEPHONY_SIM_H__

#include "telephony_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @file telephony_sim.h
 * @brief This file contains the SIM APIs and related enumerations.
 */

/**
 * @addtogroup CAPI_TELEPHONY_INFORMATION_SIM
 * @{
 */

/**
 * @brief Enumeration for the state of SIM card.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	TELEPHONY_SIM_STATE_UNAVAILABLE, /**< SIM is not available on this device */
	TELEPHONY_SIM_STATE_LOCKED,      /**< SIM is locked */
	TELEPHONY_SIM_STATE_AVAILABLE,   /**< SIM is available on this device */
	TELEPHONY_SIM_STATE_UNKNOWN,     /**< SIM is in transition between states */
} telephony_sim_state_e;

/**
 * @brief Enumeration for the lock state of SIM card.
 * @since_tizen 3.0
 */
typedef enum {
	TELEPHONY_SIM_LOCK_STATE_UNKNOWN, /**< SIM is not in lock */
	TELEPHONY_SIM_LOCK_STATE_PIN_REQUIRED, /**< SIM is PIN(Personal Identification Number) locked */
	TELEPHONY_SIM_LOCK_STATE_PUK_REQUIRED, /**< SIM is PUK(Personal Unblocking Code) locked */
	TELEPHONY_SIM_LOCK_STATE_PERM_LOCKED, /**< SIM is permanently blocked(All the attempts for PIN/PUK failed) */
	TELEPHONY_SIM_LOCK_STATE_NCK_REQUIRED, /**< SIM is NCK(Network Control Key) locked */
	TELEPHONY_SIM_LOCK_STATE_NSCK_REQUIRED, /**< SIM is NSCK(Network Subset Control Key) locked */
	TELEPHONY_SIM_LOCK_STATE_SPCK_REQUIRED, /**< SIM is SPCK(Service Provider Control Key) locked */
	TELEPHONY_SIM_LOCK_STATE_CCK_REQUIRED, /**< SIM is CCK(Corporate Control Key) locked */
	TELEPHONY_SIM_LOCK_STATE_PHONE_REQUIRED /**< SIM is PH-SIM(Phone-SIM) locked */
} telephony_sim_lock_state_e;

/**
 * @brief Enumeration for the type of SIM card .
 * @since_tizen 2.4
 */
typedef enum {
	TELEPHONY_SIM_APP_TYPE_SIM = 0x01, /**< SIM(GSM) Application */
	TELEPHONY_SIM_APP_TYPE_USIM = 0x02, /**< USIM Application */
	TELEPHONY_SIM_APP_TYPE_CSIM = 0x04, /**< CDMA Application */
	TELEPHONY_SIM_APP_TYPE_ISIM = 0x08, /**< ISIM Application */
} telephony_sim_application_type_e;


/**
 * @brief Gets the Integrated Circuit Card IDentification (ICC-ID).
 * @details The Integrated Circuit Card Identification number internationally identifies SIM cards.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks You must release @c icc_id using free() on success case.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] icc_id The Integrated Circuit Card Identification
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 * @retval #TELEPHONY_ERROR_SIM_NOT_AVAILABLE SIM is not available
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_icc_id(telephony_h handle, char **icc_id);

/**
 * @brief Gets the SIM Operator (MCC [3 digits] + MNC [2~3 digits]).
 * @details The Operator is embedded in the SIM card.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks You must release @c sim_operator using free() on success case.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] sim_operator The SIM Operator
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 * @retval #TELEPHONY_ERROR_SIM_NOT_AVAILABLE SIM is not available
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_operator(telephony_h handle, char **sim_operator);

/**
 * @brief Gets the Mobile Subscription Identification Number (MSIN [9~10 digits]) of the SIM provider.
 * @details This function gets Mobile Subscription Identification Number embedded in the SIM card.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks You must release @c msin using free() on success case.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] msin The Mobile Subscription Identification Number
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 * @retval #TELEPHONY_ERROR_SIM_NOT_AVAILABLE SIM is not available
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_msin(telephony_h handle, char **msin);

/**
 * @brief Gets the Service Provider Name (SPN) of the SIM card.
 * @details This function gets Service Provider Name embedded in the SIM card.
 *          If this value is not stored in SIM card, @c NULL will be returned.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks You must release @c spn using free() on success case.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] spn The Service Provider Name
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 * @retval #TELEPHONY_ERROR_SIM_NOT_AVAILABLE SIM is not available
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_spn(telephony_h handle, char **spn);

/**
 * @brief Checks whether the current SIM card is different from the previous SIM card.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] is_changed @c true if the current SIM card is different from the previous SIM card,
 *                        otherwise @c false if the SIM card is not changed
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 * @retval #TELEPHONY_ERROR_SIM_NOT_AVAILABLE SIM is not available
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_is_changed(telephony_h handle, bool *is_changed);

/**
 * @brief Gets the state of the SIM.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] sim_state The current state of the SIM
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 */
int telephony_sim_get_state(telephony_h handle, telephony_sim_state_e *sim_state);

/**
 * @brief Gets the list of application on UICC.
 *
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] app_list The masking value for below values
 *                      #TELEPHONY_SIM_APP_TYPE_SIM 0x01 GSM Application
 *                      #TELEPHONY_SIM_APP_TYPE_USIM 0x02 USIM Application
 *                      #TELEPHONY_SIM_APP_TYPE_CSIM 0x04 CSIM Application
 *                      #TELEPHONY_SIM_APP_TYPE_ISIM 0x08 ISIM Application
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 */
int telephony_sim_get_application_list(telephony_h handle, unsigned int *app_list);

/**
 * @brief Gets the SIM card subscriber number.
 * @details This function gets subscriber number embedded in the SIM card.
 *          This value contains MSISDN related to the subscriber.
 *          If this value is not stored in SIM card, @c NULL will be returned.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks You must release @c subscriber_number using free() on success case.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] subscriber_number The subscriber number in the SIM
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 * @retval #TELEPHONY_ERROR_SIM_NOT_AVAILABLE SIM is not available
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_subscriber_number(telephony_h handle, char **subscriber_number);

/**
 * @brief Gets the Subscriber ID.
 * @details This function gets subscriber ID encoded.
 *
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks You must release @c subscriber_id using free() on success case.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] subscriber_id The subscriber ID
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 * @retval #TELEPHONY_ERROR_SIM_NOT_AVAILABLE SIM is not available
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_subscriber_id(telephony_h handle, char **subscriber_id);

/**
 * @brief Gets the lock state of the SIM.
 * @details This function gets SIM card lock state.
 *          If SIM state is #TELEPHONY_SIM_STATE_LOCKED, you can use this function to retrieve lock state.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] lock_state The current lock state of the SIM
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_LOCKED.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_lock_state(telephony_h handle, telephony_sim_lock_state_e *lock_state);

/**
 * @brief Gets the GID1 (Group Identifier Level 1).
 * @details This function gets Group Identifier Level 1(GID1) embedded in the SIM card.
 *          If this value is not stored in SIM card, @c NULL will be returned.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @remarks You must release @c gid1 using free() on success case.
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] gid1 The GID1
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 * @retval #TELEPHONY_ERROR_SIM_NOT_AVAILABLE SIM is not available
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_group_id1(telephony_h handle, char **gid1);

/**
 * @brief Gets the call fowarding state of the SIM.
 * @details If the state is true, incoming call will be forwareded to selected number.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/telephony
 *
 * @param[in] handle The handle from telephony_init()
 * @param[out] call_forwarding_state The value whether incoming call will be forwarded or not.
 *                                   (true: forwared, false: not forwared)
 *
 * @return @c 0 on success,
 *         otherwise a negative error value
 *
 * @retval #TELEPHONY_ERROR_NONE              Successful
 * @retval #TELEPHONY_ERROR_INVALID_PARAMETER Out of memory
 * @retval #TELEPHONY_ERROR_PERMISSION_DENIED Permission denied
 * @retval #TELEPHONY_ERROR_NOT_SUPPORTED     Not supported
 * @retval #TELEPHONY_ERROR_OPERATION_FAILED  Invalid parameter
 *
 * @pre The SIM state must be #TELEPHONY_SIM_STATE_AVAILABLE.
 *
 * @see telephony_sim_get_state()
 */
int telephony_sim_get_call_forwarding_state(telephony_h handle, bool *call_forwarding_state);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CAPI_TELEPHONY_SIM_H__ */
