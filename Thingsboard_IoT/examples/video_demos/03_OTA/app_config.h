/*******************************************************************************
 * Copyright (c) 2021-2024, Buildstorm Pvt Ltd
 *
 * Filename:     app_config.h
 *
 * Description:
 *               Contains macros for the application
 *******************************************************************************/

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define APP_VERSION "v0.1.3"

#define TASK_APP_PRIORITY 4
#define TASK_APP_STACK_SIZE (2 * 1024) // reduce it to 4096

#define TEST_WIFI_SSID "your SSID"
#define TEST_WIFI_PASSWORD "wifi password"

#define THINGSBOARD_URI "mqtt://cloud.thingsboard.io"
#define THINGSBOARD_PORT 1883


#define PROVISION_DEVICE_KEY "your provision device key"
#define PROVISION_DEVICE_SECRET "your provision device secret"
#define PROVISION_DEVICE_NAME "test_UI"

#define THINGSBOARD_TELEMETRY_TOPIC "v1/devices/me/telemetry"

#ifdef __cplusplus
}
#endif

#endif
