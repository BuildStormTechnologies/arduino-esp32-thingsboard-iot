/**
 * \copyright Copyright (c) 2021-2024, Buildstorm Pvt Ltd
 *
 * \file combined.ino
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
  delay(200);
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
    printf("\nEVENT_MQTT_CONNECTED");
    break;
  case EVENT_MQTT_DISCONNECTED:
    printf("\nEVENT_MQTT_DISCONNECTED");
    break;

  case EVENT_BLE_CONNECTED:
    printf("\nEVENT_BLE_CONNECTED");
    break;
  case EVENT_BLE_AUTHENTICATED:
    printf("\nEVENT_BLE_AUTHENTICATED");
    break;
  case EVENT_BLE_DISCONNECTED:
    printf("\nEVENT_BLE_DISCONNECTED");
    break;

  default:
    break;
  }
}

void attributeUpdateCallBack(const char *pKeyStr, const void *pValue, valueType_et valuetype)
{
  switch (valuetype)
  {
  case VALUE_TYPE_INT:
    printf("\n[%s]: %d", pKeyStr, *(int *)pValue);
    break;

  case VALUE_TYPE_FLOAT:
    printf("\n[%s]: %.2f", pKeyStr, *(float *)pValue);
    break;

  case VALUE_TYPE_STRING:
    printf("\n[%s]: %s", pKeyStr, pValue);
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

  /* Attribute and Telemetry Configuration */
  sysConfig.maxAttributes_u8 = 4;
  sysConfig.maxTelemetryElements_u8 = 2;

  /* Wifi Configuration*/
  sysConfig.pWifiSsidStr = (char *)TEST_WIFI_SSID;
  sysConfig.pWifiPwdStr = (char *)TEST_WIFI_PASSWORD;

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
  static float hum_f32 = 10.6;
  static uint16_t temp_u16 = 20;
  static uint32_t count_u32 = 0;

  if (SYS.getMode() == SYSTEM_MODE_NORMAL)
  {
    if (TB.isConnected())
    {
      if ((millis() > nextPubTime))
      {

        TB.telemetryUpdate("temp", &temp_u16, VALUE_TYPE_INT);
        TB.telemetryUpdate("hum", &hum_f32, VALUE_TYPE_FLOAT);
        printf("\n temp:%d humidity:%.2f", temp_u16, hum_f32);
        TB.attributeUpdate("clientAttr", &count_u32);
        printf("[clientAttr]:%d \n", count_u32);
        count_u32++;

        temp_u16++;
        hum_f32 += 1.1;
        nextPubTime = millis() + 10000;
      }
    }
  }
}
