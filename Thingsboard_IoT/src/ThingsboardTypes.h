 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file ThingsboardTypes.h
 * \brief ThingsboardTypes library header file.
 *
 *
 * This file defines the structure and enums used by the ThingsBoard library.
 *
 * The libraries have been tested on the ESP32 modules.
 * Buildstorm explicitly denies responsibility for any hardware failures
 * arising from the use of these libraries, whether directly or indirectly.

 *
 * EULA LICENSE:
 * This library is licensed under end user license EULA agreement.
 * The EULA is available at https://buildstorm.com/eula/
 * For any support contact us at hello@buildstorm.com
 *
 */

#ifndef __THINGSBOARD_TYPES_H__
#define __THINGSBOARD_TYPES_H__

#include "LibConfig.h"
#include "LibMsgTypes.h"

#define MAX_ATTRIBUTE_ELEMETS 10
#define LENGTH_ATTRIBUTE_ELEMENT_BUFFER 20
#define LENGTH_ATTRIBUTE_KEY 20
typedef struct
{
    char *pUriStr;                /*!< MQTT broker URI */
    char *pHostNameStr;           /*!< MQTT IoT Endpoint */
    uint16_t port_u16;            /*!< MQTT IoT port number */
    char *pCertStr;               /*!< Certificate string */
    char *pUserNameStr;           /*!< MQTT username */
    char *pPasswordStr;           /*!< MQTT password */
    char *pAccessTokenStr;        /*!< MQTT password */
    char *pClientIdStr;           /*!< MQTT password */
    const char *pThingNameStr;    /*!< MQTT IoT thing name */
    char *pProvisionDeviceSecret; /*!< Provision Device Secret */
    char *pProvisionDeviceKey;    /*!< Provision Device Key */
} mqttClientConfig_st;

/**
 * @struct value_st
 * This is a twin value type.
 */
typedef struct
{
    int32_t val_i32;                      /*!< Value of type int32_t */
    float val_f32;                        /*!< Value of type float */
    char str[LENGTH_ATTRIBUTE_ELEMENT_BUFFER]; /*!< Value of type string */
} value_st;

/**
 * @enum attributeType_et
 * An enum that represents all possible attribute value types.
 */
typedef enum
{
    ATTRIBUTE_CLIENT,    /*!< Client Attributes*/
    ATTRIBUTE_SHARED,    /*!< Shared Attributes */
    ATTRIBUTE_MAX,      /*!< Total number of attribute types */
} attributeType_et;

/**
 * @enum attributeValueType_et
 * An enum that represents all possible attribute value types.
 */
typedef enum
{
    VALUE_TYPE_INT,    /*!< Attribute value type as integer */
    VALUE_TYPE_FLOAT,  /*!< Attribute value type as float */
    VALUE_TYPE_STRING, /*!< Attribute value type as string */
    VALUE_TYPE_MAX,    /*!< Total number of attribute value types */
}valueType_et;



/**
 * @brief Attribute update callback function type. The application should
 * define the callback function and intialize it in the @ref attribute_st
 * configuration.
 */
typedef void (*attributeUpdateCallBack_t)(const char *pKeyStr, const void *pValue,valueType_et valuetype);


#endif //__THINGSBOARD_TYPES_H__