// Config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>


// Define whether the environment is HOME or ONSITE
// Comment out this line when compiling for ONSITE
#define HOME_ENVIRONMENT

#ifdef HOME_ENVIRONMENT
    // Settings for Home
    #define WIFI_SSID "HomeNet"
    #define WIFI_PASSWORD "0102030405"
    #define MQTT_BROKER_IP "192.168.252.121"
    #define SIMULATOR_ENVIRONMENT true
#endif

#ifdef HOME_ENVIRONMENT_RASBERRYPI
    // Settings for Home
    #define WIFI_SSID "HomeNet"
    #define WIFI_PASSWORD "0102030405"
    #define MQTT_BROKER_IP "192.168.252.23"
#endif

#ifdef ONSITE_ENVIRONMENT
    // Settings for Onsite
    #define WIFI_SSID "ORE_Main"
    #define WIFI_PASSWORD "0102030405"
    #define MQTT_BROKER_IP "192.168.178.35"
    #define SIMULATOR_ENVIRONMENT false
#endif


#ifdef ONSITE_ENVIRONMENT_IOT_EXT
    // Settings for Onsite
    #define WIFI_SSID "ORE_IoT_Ext"
    #define WIFI_PASSWORD "0102030405"
    #define MQTT_BROKER_IP "192.168.178.35"
    #define SIMULATOR_ENVIRONMENT false
#endif



// Add other configurations that do not change
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""




#endif // CONFIG_H
