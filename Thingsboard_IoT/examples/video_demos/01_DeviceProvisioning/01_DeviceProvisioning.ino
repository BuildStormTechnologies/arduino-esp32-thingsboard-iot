/**
 * \copyright Copyright (c) 2021-2024, Buildstorm Pvt Ltd
 *
 * \file 01_DeviceProvisioning.ino
 * \brief 01_DeviceProvisioning.ino file.
 *
 * The 01_DeviceProvisioning.ino main entry of the application.
 *
 * The file have been tested on the ESP32 modules.
 * Buildstorm explicitly denies responsibility for any hardware failures
 * arising from the use of these file, whether directly or indirectly.
 * Please note that files are subject to change without prior notice.
 *
 * EULA LICENSE:
 * This file is licensed under end user license EULA agreement.
 * The EULA is available at https://buildstorm.com/eula/
 * For any support contact us at hello@buildstorm.com
 *
 */

#include "app_config.h"
#include "LibSystem.h"
#include "LibFlash.h"
#include "LibBle.h"
#include "Thingsboard.h"
#include "TFT_eSPI.h"
#include "icons.h"
#include "Free_Fonts.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

LibSystem SYS;
LibFlash FLASH;
LibBle BLE;
Thingsboard TB;

#define gray 0x6B6D

typedef enum
{
  DISPLAY_STATUS_OFF,
  DISPLAY_STATUS_NO_CHANGE,
  DISPLAY_STATUS_TOGGLE,
  DISPLAY_STATUS_ON,
} displayStatusCodes_et;

static uint8_t eventWIFIState = DISPLAY_STATUS_OFF;
static uint8_t eventBLEState = DISPLAY_STATUS_OFF;
static uint8_t eventMQTTState = DISPLAY_STATUS_OFF;
bool printStatus = false;

void app_init();
void app_loop();

void setup()
{
  Serial.begin(115200);
  app_init();
  disp_init();
}

void loop()
{
  app_loop();
  delay(200);
}


void app_init()
{

  systemInitConfig_st sysConfig = {0};

  sysConfig.systemEventCallbackHandler = app_eventsCallBackHandler;
  sysConfig.pAppVersionStr = (char *)APP_VERSION;

  /* Disable below for Ble provisioning */
  sysConfig.pWifiSsidStr = (char *)TEST_WIFI_SSID;
  sysConfig.pWifiPwdStr = (char *)TEST_WIFI_PASSWORD;
  sysConfig.maxTelemetryElements_u8 = 2;
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

   switch (SYS.getMode()) 
   {
    case SYSTEM_MODE_DEVICE_CONFIG:
        if (SYS.isDeviceRegistered() == 1)
        {
          provisionedScreen();
          delay(1000);
          rebootingScreen();
        }
        else
        {
          provisioningScreen();
        }
    break;

  case SYSTEM_MODE_NORMAL:
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
    afterProvisioned(humidity);
  break;
  
  
    case SYSTEM_MODE_OTA:
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
      eventWIFIState = DISPLAY_STATUS_ON;
      printStatus = true;
      printf("\nEVENT_WIFI_CONNECTED");
      break;
    case EVENT_WIFI_DISCONNECTED:
      eventWIFIState = DISPLAY_STATUS_OFF;
      printStatus = true;
      printf("\nEVENT_WIFI_DISCONNECTED");
      break;


    case EVENT_MQTT_CONNECTED:
      eventMQTTState = DISPLAY_STATUS_ON;
      printStatus = true;
      printf("\nEVENT_THINGSBOARD_CONNECTED");
      break;
    case EVENT_MQTT_DISCONNECTED:
      eventMQTTState = DISPLAY_STATUS_OFF;
      printStatus = true;
      printf("\nEVENT_THINGSBOARD_DISCONNECTED");
      break;

  default:
    break;
  }
}

void disp_init()
{
  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(320, 170);
  sprite.setSwapBytes(true);
  sprite.setTextColor(TFT_WHITE);
  sprite.setTextDatum(4);
}


void drawHeaderFooter()
{
  sprite.setTextColor (TFT_SKYBLUE);
  sprite.setFreeFont(FF5);
  sprite.setTextSize (1);
  sprite.setCursor (10, 20);
  sprite.print ("ThingsBoard IoT");
    sprite.drawRect  (  2, 142, 316, 26, TFT_WHITE);
  sprite.setTextColor (TFT_WHITE);
  sprite.drawString("Buildstorm Technologies", 160, 152);
  sprite.drawRect  (  0, 0, 320, 170,   gray);

}

void icon_status()
{
  static bool blinkState = false;

  if (!eventWIFIState)
  {
    if (blinkState)
    {
      sprite.pushImage(290, 2, 25, 25, wifiDiscon_icon);
    }
    else
    {
      sprite.drawRect(290, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(290, 2, 25, 25, wifi_icon);
  }

  if (eventWIFIState==DISPLAY_STATUS_ON && !eventMQTTState)
  {
    if (blinkState)
    {
      sprite.pushImage(260, 4, 25, 25, tbDiscon_icon);
    }
    else
    {
      sprite.drawRect(260, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(260, 4, 25, 25, tbDiscon_icon);
  }
  if(eventMQTTState==DISPLAY_STATUS_ON)
  {
   sprite.pushImage(260, 4, 25, 25, tbCon_icon);
  }

  blinkState = !blinkState;
}

void provisioningScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Provisioning...", 170, 80);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void provisionedScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Provisioned...", 170, 80);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void rebootingScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Rebooting...", 170, 80);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}


void thingVersion()
{
  sprite.setTextColor (TFT_SKYBLUE);
  sprite.setFreeFont(FF5);
  sprite.setTextSize (1);
  sprite.setCursor (10, 20);
  sprite.print (TB.getThingName());
  int deviceNameLength = sprite.textWidth(TB.getThingName());
  sprite.setFreeFont(FF0);
  sprite.setCursor (deviceNameLength + 15, 15);
  sprite.print (APP_VERSION);
    sprite.drawRect  (  2, 142, 316, 26, TFT_WHITE);
  sprite.setFreeFont(FF5);
  sprite.setTextColor (TFT_WHITE);
  sprite.drawString("Buildstorm Technologies", 170, 152);
  sprite.drawRect  (  0, 0, 320, 170,   gray);
}

void afterProvisioned(float message)
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString(String(message), 160, 80);
  thingVersion();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}
