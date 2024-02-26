/**
 * \copyright Copyright (c) 2021-2024, Buildstorm Pvt Ltd
 *
 * \file 05_BleDeviceProvisioning.ino
 * \brief 05_BleDeviceProvisioning.ino file.
 *
 * The 05_BleDeviceProvisioning.ino main entry of the application.
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
#include "LibBle.h"
#include "Thingsboard.h"
#include "TFT_eSPI.h"
#include "icons.h"
#include "Free_Fonts.h"
#include "LibWifi.h"
#include "LibFlash.h"

LibSystem SYS;
LibBle BLE;
Thingsboard TB;
LibWifi WIFI;
LibFlash FLASH;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

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

void app_init()
{

  systemInitConfig_st sysConfig = {0};

  sysConfig.systemEventCallbackHandler = app_eventsCallBackHandler;
  sysConfig.pAppVersionStr = (char *)APP_VERSION;
  sysConfig.maxTelemetryElements_u8 = 2;
  sysConfig.maxAttributes_u8 = 4;

  if (SYS.init(&sysConfig))
  {
    BLE.start();
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
  static uint32_t temp_u32 = 20;
  static uint32_t count_u32 = 0;
  static float humidity = 10.0;
  static int i=0;

  switch (SYS.getMode())
  {
  case SYSTEM_MODE_DEVICE_CONFIG:
    if (SYS.isDeviceRegistered())
    {
      printf("\nDevice successfully provisioned");
      provisionedScreen();
      delay(500);
      rebootingScreen();
      delay(2000);
    }
    else
    {
      // printf("\nDevice is not provisioned");
      if (!eventBLEState)
      {
        advertisingScreen();
      }
      else if(EVENT_BLE_CONNECTED)
      {
        if(i==0)
        {
        appConnectedScreen();
        i=1;
        }
      }
      if(WIFI.getState() == STATE_WIFI_SCAN)
      {
        wifiScanningScreen();
      }
      else if(WIFI.getState() == STATE_WIFI_START)
      {
        wifiConnScreen();
      }
    }  
  break;
  case SYSTEM_MODE_NORMAL:
    if (TB.isConnected())
    {
      if ((millis() > nextPubTime))
      {
        TB.telemetryUpdate("temp", &temp_u32, VALUE_TYPE_INT);
        TB.telemetryUpdate("hum", &humidity, VALUE_TYPE_FLOAT);
        printf("[temp]:%d [hum]: %.2f\n", temp_u32++, humidity);
        TB.attributeUpdate("clientAttr", &count_u32);
        printf("[clientAttr]:%d \n", count_u32++);
        humidity = humidity + 1.1;
        nextPubTime = millis() + 5000;
        // afterProvisioned(humidity);
      }
    }
    afterProvisioned(humidity, temp_u32);
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
    case EVENT_BLE_CONNECTED:
      eventBLEState = DISPLAY_STATUS_ON;
      printStatus = true;
      printf("\nEVENT_BLE_CONNECTED");
      break;
    case EVENT_BLE_AUTHENTICATED:
      printf("\nEVENT_BLE_AUTHENTICATED");
      break;
    case EVENT_BLE_DISCONNECTED:
      eventBLEState = DISPLAY_STATUS_OFF;
      printStatus = true;
      printf("\nEVENT_BLE_DISCONNECTED");
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
  sprite.fillSprite(TFT_BLACK);
}

void drawHeaderFooter()
{
  sprite.setTextColor(TFT_SKYBLUE);
  sprite.setFreeFont(FF5);
  sprite.setTextSize(1);
  sprite.setCursor(10, 20);
  sprite.print("ThingsBoard IoT");
  sprite.drawRect(5, 137, 310, 26, TFT_WHITE);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("BLE Provisioning", 170, 148);
  sprite.drawRect(0, 0, 320, 170, gray);
}

void bicon_status()
{
  static bool blinkState = false;

  if (!eventBLEState)
  {
    if (blinkState)
    {
      sprite.pushImage(290, 2, 25, 25, btDiscon_icon);
    }
    else
    {
      sprite.drawRect(290, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(290, 2, 25, 25, bt_icon);
  }

  blinkState = !blinkState;
}

void aicon_status()
{
  static bool blinkState = false;

  if (!eventBLEState)
  {
    sprite.pushImage(290, 2, 25, 25, btDiscon_icon);
  }
  else
  {
    sprite.pushImage(290, 2, 25, 25, bt_icon);
  }

  if (!eventWIFIState)
  {
    if (blinkState)
    {
      sprite.pushImage(260, 2, 25, 25, wifiDiscon_icon);
    }
    else
    {
      sprite.drawRect(260, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(260, 2, 25, 25, wifi_icon);
  }

  if (eventWIFIState==DISPLAY_STATUS_ON && !eventMQTTState)
  {
    if (blinkState)
    {
      sprite.pushImage(230, 4, 25, 25, tbDiscon_icon);
    }
    else
    {
      sprite.drawRect(230, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(230, 4, 25, 25, tbDiscon_icon);
  }
  if(eventWIFIState==DISPLAY_STATUS_ON && eventMQTTState==DISPLAY_STATUS_ON)
  {
   sprite.pushImage(230, 4, 25, 25, tbCon_icon);
  }

  blinkState = !blinkState;
}

void provisioningScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Provisioning...", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void provisionedScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Registration Successful", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void appConnectedScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("App Connected", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void wifiScanningScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Wifi Scanning", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void wifiConnScreen()
{
  static wifiCred_st  s_wifiCred = {0};
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Connected to Wifi", 170, 80);
  // sprite.drawString(String(sysConfig.pWifiSsidStr), 170, 100);
  // sprite.drawString((FLASH_getDeviceConfig(DEVICE_CONFIG_WIFI, (void *)&s_wifiCred)), 170, 100);
  sprite.drawString(String(WIFI.getSSID()), 170, 100);

  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void rebootingScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Rebooting...", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void advertisingScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Advertising", 160, 55);
  // int deviceNameLength = sprite.textWidth(SYS.getDeviceName());
  // sprite.drawRect(90,70, deviceNameLength+10, 26, TFT_WHITE);
  sprite.drawString(String(SYS.getDeviceName()), 160, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void openScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  if (SYS.isDeviceRegistered())
  {
    provsScreen();
  }
  aicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void provsScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Provision Successful", 170, 80);
  thingVersion();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void thingVersion()
{
  sprite.setTextColor (TFT_SKYBLUE);
  sprite.setFreeFont(FF5);
  sprite.setTextSize (1);
  sprite.setCursor (10, 20);
  sprite.print (String(SYS.getDeviceName()));
  int deviceNameLength = sprite.textWidth(SYS.getDeviceName());
  sprite.setFreeFont(FF0);
  sprite.setCursor (deviceNameLength + 15, 15);
  sprite.print (APP_VERSION);
  sprite.drawRect(5, 137, 310, 26, TFT_WHITE);
  sprite.setFreeFont(FF5);
  sprite.setTextColor (TFT_WHITE);
  sprite.drawString("Buildstorm Technologies", 170, 148);
  sprite.drawRect  (  0, 0, 320, 170,   gray);
}

void afterProvisioned(float message, int message1)
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("temp :", 50, 60);
  sprite.drawString("hum  :", 50, 100);
  sprite.drawString(String(message), 130, 60);
  sprite.drawString(String(message1), 130, 100);
  thingVersion();
  aicon_status();
  sprite.pushSprite(0, 0);
  delay(500);
}






