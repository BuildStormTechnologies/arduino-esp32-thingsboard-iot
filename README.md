# Arduino ESP32 ThingsBoard IoT
The Buildstorm platform simplifies the integration of IoT core features onto ESP32 Hardware, spanning from basic IoT functionalities to advanced features like OTA updates and provisioning.

The architecture is based on the core IDF APIs, complemented by a C++ wrapper tailored for application use, guaranteeing non-blocking operation of all APIs. Each user action, including HTTP requests, MQTT publishing, Telemetry/Arrribute updates, and OTA, is logged as an event and seamlessly managed in the background. To maintain this seamless operation, the platform effectively runs a system task.

This establishes a robust foundation for your upcoming IoT project.

Supported features:

1. Device provisioning
2. Telemetry update
3. Shared/Cleint Attributes
4. Web OTA
5. Local websever OTA
6. BLE Device Provisioning

## Solution

1. [ThingsBoard IoT Platform](https://buildstorm.com/solutions/esp32-thingsboard-iot-platform/)

## References

1. [ThingsBoard IoT Platform](https://buildstorm.com/solutions/esp32-thingsboard-iot-platform/)
1. [Device Provisioning Setup](https://buildstorm.com/blog/thingsboard-iot-device-provisioning-setup/)
2. [OTA Updates](https://buildstorm.com/blog/thingsboard-ota-updates/)
3. [Mobile App: User Manual](https://buildstorm.com/blog/thingsboard-mobile-app-user-manual/)


---
## Example Setup
1. Generate the Provision Device Key/Secret following the article [Device Provisioning Setup](https://buildstorm.com/blog/thingsboard-iot-device-provisioning-setup/).
2. Update the following WiFi and ThingsBoard parameters in `app_config.h` of the example
3. This configuration will be same accross all examples

```
#define TEST_WIFI_SSID     "YOUR WIFI SSID"
#define TEST_WIFI_PASSWORD "YOUR WIFI PWD"

#define PROVISION_DEVICE_KEY    "your provision device key"
#define PROVISION_DEVICE_SECRET "your provision device secret"
```

---
## SoC Compatibility

| Name            | BLE           | OTA           |
|-----------------|---------------|---------------|
| ESP32           | Not Supported | Not Supported |
| ESP32 S3        | Supported     | Supported     |

While our Arduino IoT platform currently supports BLE and OTA solely on ESP32S3, these capabilities are accessible on our ESP-IDF platform for all ESP32 SoCs. These functionalities will soon be integrated into Arduino. Until then, you can explore the IDF version of our platform [ESP32-idf ThingsBoard IoT Platform](https://github.com/BuildStormTechnologies/esp32-thingsboard-iot).

---
## Arduino setting
1. ESP32 settings

    ![esp32 arduino settings](<images/esp32 arduino settings.png>)
<br />
<br />
<br />

2. ESP32S3 settings

   ![esp32s3 arduino settings](<images/esp32s3 arduino settings.png>)


