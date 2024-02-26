/**
 * \copyright Copyright (c) 2021-2024, Buildstorm Pvt Ltd
 *
 * \file app_config.h
 * \brief app_config header file.
 *
 * The app_config Contains macros for the application.
 *
 */

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define APP_VERSION "v0.1.0"

#define TEST_WIFI_SSID "your SSID"
#define TEST_WIFI_PASSWORD "wifi password"

#define THINGSBOARD_URI "mqtt://cloud.thingsboard.io"
#define THINGSBOARD_PORT 1883

#define PROVISION_DEVICE_KEY "your provision device key"
#define PROVISION_DEVICE_SECRET "your provision device secret"

#ifdef __cplusplus
}
#endif

#endif //_APP_CONFIG_H_
