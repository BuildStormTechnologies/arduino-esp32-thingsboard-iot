#include "app_config.h"
#include "LibSystem.h"
#include "LibFlash.h"
#include "LibBle.h"
#include "Thingsboard.h"
#include "TFT_eSPI.h"
#include "icons.h"
#include "Free_Fonts.h"

LibSystem SYS;
LibFlash FLASH;
LibBle BLE;
Thingsboard TB;

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

//progress bar variables
int progress = 0;
int w = 300;
int h = 18;
int x = 12;
int y = 110;
int blocks = 0;

void app_init();
void app_loop();

void setup() {
  Serial.begin(115200);
  app_init();
  disp_init();

}

void loop() {
  app_loop();
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

void app_task(void *param)
{
  while (1)
  {
    if (SYS.getMode() == SYSTEM_MODE_NORMAL)
    {
      OTAScreen();
    }
    if (SYS.getMode() == SYSTEM_MODE_OTA)
    {
      if (SYS.getOtaPercentage() == 100)
      {
        rebootScreen();
      }
      else if (SYS.getOtaPercentage() == 0)
      {
        OTARequestScreen();
      }
      else
      {
        progressbar(SYS.getOtaPercentage());
        //        progress++;
      }
    }
    if (printStatus == true)
    {
      icon_status();
      printStatus = false;
    }

    vTaskDelay(10);
  }
}

/**
   @brief    entry point of the project
   @param    None
   @return   None
*/
void app_init() {

  systemInitConfig_st sysConfig = { 0 };

  sysConfig.systemEventCallbackHandler = app_eventsCallBackHandler;
  sysConfig.pDeviceNamePrefixStr = NULL;
  sysConfig.pAppVersionStr = (char *)APP_VERSION;

  /* Disable below for Ble provisioning */
  sysConfig.pWifiSsidStr = (char *)TEST_WIFI_SSID;
  sysConfig.pWifiPwdStr = (char *)TEST_WIFI_PASSWORD;

  sysConfig.s_mqttClientConfig.pThingNameStr = (char *)PROVISION_DEVICE_NAME;
  sysConfig.s_mqttClientConfig.pUriStr = (char *)THINGSBOARD_URI;
  sysConfig.s_mqttClientConfig.pProvisionDeviceKey = (char *)PROVISION_DEVICE_KEY;
  sysConfig.s_mqttClientConfig.pProvisionDeviceSecret = (char *)PROVISION_DEVICE_SECRET;

  if (SYS.init(&sysConfig)) {
    //    BLE.init();
    SYS.start();
  } else {
    while (1) {
      printf("\n System Init failed, verify the init config ....");
      delay(5000);
    }
  }
  uint8_t err = xTaskCreate(&app_task, "app_task", TASK_APP_STACK_SIZE, NULL, TASK_APP_PRIORITY, NULL);
  if (pdPASS != err)
  {
    printf("\n Error 0x%X in creating app_task \n restarting system\n\r\n\r", err);
  }
}

void app_loop() {

  static uint32_t nextPubTime = 0;
  static uint32_t count_u32 = 100;

  if (SYS.getMode() == SYSTEM_MODE_NORMAL) {
    if (TB.isConnected()) {
      mqttMsg_st s_mqttMsg = { 0 };
      if (TB.read(&s_mqttMsg)) {
        printf("\n Received: [%s]:%s", s_mqttMsg.topicStr, s_mqttMsg.payloadStr);
      }
      if ((millis() > nextPubTime)) {
        s_mqttMsg.payloadLen_u16 = sprintf(s_mqttMsg.payloadStr, "{\"temp\": %d}", count_u32++);
        strcpy(s_mqttMsg.topicStr, THINGSBOARD_TELEMETRY_TOPIC);
        s_mqttMsg.qos_e = QOS0_AT_MOST_ONCE;
        TB.publish(&s_mqttMsg);
        nextPubTime = millis() + 5000;
        printf("\n %s\n", s_mqttMsg.payloadStr);
      }
    }
  }
  //  if (SYS.getMode() == SYSTEM_MODE_NORMAL)
  //  {
  //    OTAScreen();
  //  }
  //  if (SYS.getMode() == SYSTEM_MODE_OTA)
  //  {
  //    if (SYS.getOtaPercentage() == 100)
  //    {
  //      rebootScreen();
  //    }
  //    else if (SYS.getOtaPercentage() == 0)
  //    {
  //      OTARequestScreen();
  //    }
  //    else
  //    {
  //      progressbar(SYS.getOtaPercentage());
  //      //        progress++;
  //    }
  //  }
  //  if (printStatus == true)
  //  {
  //    icon_status();
  //    printStatus = false;
  //  }

  delay(200);
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
  //  tft.drawLine  (  0, 207, 320, 207,   TFT_WHITE);
  //  sprite.drawRect  (  2, 142, 316, 26, TFT_WHITE);
  //  sprite.setTextColor (TFT_WHITE);
  //  sprite.drawString("OTA updates", 170, 152);
  sprite.drawRect  (  0, 0, 320, 170,   gray);

}

void icon_status()
{

  if (eventBLEState == DISPLAY_STATUS_ON)
  {
    sprite.pushImage(290, 2, 25, 25, bt_icon);
  }
  else
  {
    sprite.pushImage(290, 2, 25, 25, btDiscon_icon);
  }
  if (eventWIFIState == DISPLAY_STATUS_ON)
  {
    sprite.pushImage(260, 2, 25, 25, wifi_icon);
  }
  else
  {
    sprite.pushImage(260, 2, 25, 25, wifiDiscon_icon);
  }
  if (eventMQTTState == DISPLAY_STATUS_ON)
  {
    sprite.pushImage(230, 4, 25, 25, tbCon_icon);
  }
  else
  {
    sprite.pushImage(230, 4, 25, 25, tbDiscon_icon);
  }
}

void progressbar(int progress)
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170,   gray);
  sprite.setFreeFont(&Orbitron_Light_24);
  //  sprite.drawString("Progress:", 75, 16);
  //  sprite.setFreeFont(&Orbitron_Light_32);

  sprite.drawString(String(progress) + "%", 170, 80);

  int blockWidth = w / 100;
  blocks = progress;
  sprite.drawRoundRect(x, y, w, h, 3, TFT_WHITE);

  for (int i = 0; i < blocks; i++) {
    sprite.fillRect(x + 2 + i * blockWidth, y + 4, blockWidth - 1.5, 10, TFT_GREEN);
  }
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(300);
}

void rebootScreen()
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

void OTAScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setTextColor (TFT_WHITE);
  sprite.setFreeFont(FF5);
  sprite.drawString("OTA UPDATES", 170, 80);
  sprite.drawString(APP_VERSION, 170, 100);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void OTARequestScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setTextColor (TFT_WHITE);
  sprite.setFreeFont(FF5);
  sprite.drawString("OTA Requested", 170, 80);
  //  sprite.drawString(APP_VERSION, 170, 100);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}
