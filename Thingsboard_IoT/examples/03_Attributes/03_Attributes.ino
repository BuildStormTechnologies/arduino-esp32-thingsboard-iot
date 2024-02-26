/**
 * \copyright Copyright (c) 2021-2024, Buildstorm Pvt Ltd
 *
 * \file 03_Attributes.ino
 * \brief main entry of the application.
 */

#include "app_config.h"
#include "LibSystem.h"
#include "Thingsboard.h"

LibSystem SYS;
Thingsboard TB;

void app_init();
void app_loop();

void setup()
{
  Serial.begin(115200);
  app_init();
}

void loop()
{
  app_loop();
}

void attributeUpdateCallBack(const char *pKeyStr, const void *pValue, valueType_et valuetype)
{
  switch (valuetype)
  {
  case VALUE_TYPE_INT:
    printf("[%s]: %d", pKeyStr, *(int *)pValue);
    break;

  case VALUE_TYPE_FLOAT:
    printf("[%s]: %.2f", pKeyStr, *(float *)pValue);
    break;

  case VALUE_TYPE_STRING:
    printf("[%s]: %s", pKeyStr, pValue);
    break;

  default:
    break;
  }
}

void app_eventsCallBackHandler(systemEvents_et event_e)
{
  switch (event_e)
  {
  case EVENT_WIFI_CONNECTED:
    printf("\nEVENT_WIFI_CONNECTED");
    break;
  case EVENT_WIFI_DISCONNECTED:
    printf("\nEVENT_WIFI_DISCONNECTED");
    break;

  case EVENT_MQTT_CONNECTED:
    printf("\nEVENT_THINGSBOARD_CONNECTED");
    break;
  case EVENT_MQTT_DISCONNECTED:
    printf("\nEVENT_THINGSBOARD_DISCONNECTED");
    break;

  default:
    break;
  }
}

/**
 * @brief    entry point of the project
 * @param    None
 * @return   None
 */
void app_init()
{

  systemInitConfig_st sysConfig = {0};

  sysConfig.systemEventCallbackHandler = app_eventsCallBackHandler;
  sysConfig.pAppVersionStr = (char *)APP_VERSION;

  /* Wifi Configuration*/
  sysConfig.pWifiSsidStr = (char *)TEST_WIFI_SSID;
  sysConfig.pWifiPwdStr = (char *)TEST_WIFI_PASSWORD;

  /* Attribute and Telemetry Configuration */
  sysConfig.maxTelemetryElements_u8 = 2,
  sysConfig.maxAttributes_u8 = 4;

  /* ThingsBoard Configuration */
  sysConfig.s_mqttClientConfig.pUriStr = (char *)THINGSBOARD_URI;
  sysConfig.s_mqttClientConfig.pProvisionDeviceKey = (char *)PROVISION_DEVICE_KEY;
  sysConfig.s_mqttClientConfig.pProvisionDeviceSecret = (char *)PROVISION_DEVICE_SECRET;

  if (SYS.init(&sysConfig))
  {
    SYS.start();
    TB.attributeRegister("testString", ATTRIBUTE_SHARED, VALUE_TYPE_STRING, attributeUpdateCallBack);
    TB.attributeRegister("testInt", ATTRIBUTE_SHARED, VALUE_TYPE_INT, attributeUpdateCallBack);
    TB.attributeRegister("testFloat", ATTRIBUTE_SHARED, VALUE_TYPE_FLOAT, attributeUpdateCallBack);
    TB.attributeRegister("clientAttr", ATTRIBUTE_CLIENT, VALUE_TYPE_INT, attributeUpdateCallBack);
  }
  else
  {
    while (1)
    {
      printf("\n System Init failed, verify the init config ....");
      delay(5000);
    }
  }
}

void app_loop()
{

  static uint32_t nextPubTime = 0;
  static uint32_t count_u32 = 0;

  if (SYS.getMode() == SYSTEM_MODE_NORMAL)
  {
    if (TB.isConnected())
    {
      if ((millis() > nextPubTime))
      {
        TB.attributeUpdate("clientAttr", &count_u32);
        printf("[clientAttr]:%d \n", count_u32);
        count_u32++;
        nextPubTime = millis() + 5000;
      }
    }
  }

  delay(200);
}
