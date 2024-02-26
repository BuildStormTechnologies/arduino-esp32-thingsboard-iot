/**
 * \copyright Copyright (c) 2021-2024, Buildstorm Pvt Ltd
 *
 * \file 04_OTA.ino
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

  /* Telemetry Configuration */
  sysConfig.maxTelemetryElements_u8 = 2;

  /* ThingsBoard Configuration */
  sysConfig.s_mqttClientConfig.pUriStr = (char *)THINGSBOARD_URI;
  sysConfig.s_mqttClientConfig.pProvisionDeviceKey = (char *)PROVISION_DEVICE_KEY;
  sysConfig.s_mqttClientConfig.pProvisionDeviceSecret = (char *)PROVISION_DEVICE_SECRET;

  if (SYS.init(&sysConfig))
  {
    SYS.start();
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
  static uint32_t temp_u32 = 20;
  static float humidity = 10.0;

  if (SYS.getMode() == SYSTEM_MODE_NORMAL)
  {
    if (TB.isConnected())
    {
      if ((millis() > nextPubTime))
      {
        TB.telemetryUpdate("temp", &temp_u32, VALUE_TYPE_INT);
        TB.telemetryUpdate("hum", &humidity, VALUE_TYPE_FLOAT);
        printf("[temp]:%d [hum]: %.2f\n", temp_u32, humidity);
        temp_u32++;
        humidity = humidity + 1.1;
        nextPubTime = millis() + 5000;
      }
    }
  }

  delay(200);
}
