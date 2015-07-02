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

#ifndef __TIZEN_TELEPHONY_INFORMATION_DOC_H__
#define __TIZEN_TELEPHONY_INFORMATION_DOC_H__

/**
 * @ingroup CAPI_TELEPHONY_FRAMEWORK
 * @defgroup CAPI_TELEPHONY_INFORMATION Telephony Information
 * @brief The Telephony provides call, modem, network, and SIM information.
 *
 * @section CAPI_TELEPHONY_INFORMATION_HEADER Required Header
 *   \#include <telephony.h>
 *
 * @section CAPI_TELEPHONY_INFORMATION_OVERVIEW Overview
 * The Telephony provides call, modem, network, and SIM information.
 * @section CAPI_TELEPHONY_INFORMATION_FEATURE Related Features
 * This API is related with the following feature:\n
 * - http://tizen.org/feature/network.telephony
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.gettingstarted/html/native/details/app_filtering_n.htm"><b>Feature List</b>.</a>
 */

/**
 * @ingroup CAPI_TELEPHONY_INFORMATION
 * @defgroup CAPI_TELEPHONY_INFORMATION_CALL Call
 * @brief The Call API provides functions to check the state of voice and video calls.
 *
 * @section CAPI_TELEPHONY_INFORMATION_CALL_HEADER Required Header
 *   \#include <telephony.h>
 *
 * @section CAPI_TELEPHONY_INFORMATION_CALL_OVERVIEW Overview
 * The Telephony Call API allows you to get the voice and videos call states.
 * You can use this information about call related actions.
 * @section CAPI_TELEPHONY_INFORMATION_CALL_FEATURE Related Features
 * This API is related with the following feature:\n
 * - http://tizen.org/feature/network.telephony
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.gettingstarted/html/native/details/app_filtering_n.htm"><b>Feature List</b>.</a>
 */

/**
 * @ingroup CAPI_TELEPHONY_INFORMATION
 * @defgroup CAPI_TELEPHONY_INFORMATION_NETWORK Network
 * @brief The Network Information API provides functions to obtain information about the current telephony service network.
 *
 * @section CAPI_TELEPHONY_INFORMATION_NETWORK_HEADER Required Header
 *   \#include <telephony.h>
 *
 * @section CAPI_TELEPHONY_INFORMATION_NETWORK_OVERVIEW Overview
 * The Telephony Network Information API allows you to access, but not change the
 * information about the current cellular network and telephony service.
 * @section CAPI_TELEPHONY_INFORMATION_NETWORK_FEATURE Related Features
 * This API is related with the following feature:\n
 * - http://tizen.org/feature/network.telephony
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.gettingstarted/html/native/details/app_filtering_n.htm"><b>Feature List</b>.</a>
 */

/**
 * @ingroup CAPI_TELEPHONY_INFORMATION
 * @defgroup CAPI_TELEPHONY_INFORMATION_SIM SIM
 * @brief The SIM API provides functions to obtain information from the SIM card, as well as SIM status.
 *
 * @section CAPI_TELEPHONY_INFORMATION_SIM_HEADER Required Header
 *   \#include <telephony.h>
 *
 * @section CAPI_TELEPHONY_INFORMATION_SIM_OVERVIEW Overview
 * The Telephony SIM API allows you to extract information stored on a SIM card, but not change the information.
 * @section CAPI_TELEPHONY_INFORMATION_SIM_FEATURE Related Features
 * This API is related with the following feature:\n
 * - http://tizen.org/feature/network.telephony
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.gettingstarted/html/native/details/app_filtering_n.htm"><b>Feature List</b>.</a>
 */

/**
 * @ingroup CAPI_TELEPHONY_INFORMATION
 * @defgroup CAPI_TELEPHONY_INFORMATION_MODEM Modem
 * @brief The Modem Information API provides functions to obtain information from the modem.
 *
 * @section CAPI_TELEPHONY_INFORMATION_MODEM_HEADER Required Header
 *   \#include <telephony.h>
 *
 * @section CAPI_TELEPHONY_INFORMATION_MODEM_OVERVIEW Overview
 * The Telephony Modem Information API allows you to access, but not change the information about IMEI.
 * @section CAPI_TELEPHONY_INFORMATION_MODEM_FEATURE Related Features
 * This API is related with the following feature:\n
 * - http://tizen.org/feature/network.telephony
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.gettingstarted/html/native/details/app_filtering_n.htm"><b>Feature List</b>.</a>
 */

#endif /* __TIZEN_TELEPHONY_INFORMATION_DOC_H__ */
