 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file Thingsboard.h
 * \brief Thingsboard library header file.
 *
 *
 *  This library handles the ThingsBoard platform's core features:
 *   - Device Provisioning
 *   - Telemetry updates
 *   - Client and Shared Attribute hanlding
 *   - OTA
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

#ifndef __THINGSBOARD_H__
#define __THINGSBOARD_H__

 #include "ThingsboardTypes.h"

class Thingsboard{

    private:

    public:
        bool isConnected();
        bool subscribe(char *pTopicStr, uint8_t qos_e);
        bool publish(mqttMsg_st *ps_msg);
        uint8_t available();
        bool read(mqttMsg_st *ps_msg);
        const char *getThingName();
	    bool attributeRegister(const char *pKey, attributeType_et attrType, valueType_et valuetype,attributeUpdateCallBack_t callbackHandler);
        bool attributeUpdate(const char *pKeyStr, void *pValue);
        bool telemetryUpdate(const char *pKeyStr, void *pValue,valueType_et valuetype);
};


#endif //__THINGSBOARD_H__
