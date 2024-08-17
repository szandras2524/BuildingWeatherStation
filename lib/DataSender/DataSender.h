#ifndef DATASENDER_H
#define DATASENDER_H


#include <Arduino.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WifiClient.h>
#include <PubSubClient.h>
#include <queue>
#include <time.h>

#include "Config.h"
#include "Singleton.h"  // Include the Singleton template
#include "Observer.h"
#include "MQTTTopics.h"


// Define constants used in the cpp
#define SYSTEM_NAME "weatherStation"  // Define your system's name or retrieve from config
#define MAX_MQRTT_RETRY_TIMEOUT_SEC 30  // Maximum MQTT retry timeout in seconds

struct MQTTData {
    String topic;
    String payload;
    unsigned long timestamp;
    unsigned int qos;
    unsigned int retain;
};

struct wifiConfig {
    String ssid = WIFI_SSID;
    String password = WIFI_PASSWORD;
};

struct mqttConfig {
    const char* broker = MQTT_BROKER_IP;
    const int port = 1883;
    const char* clientId = "weatherStation";
    const char* username = "";
    const char* password = "";
    const char* willTopic = SYSTEM_WILL_TOPIC;
};

class DataSender : public Singleton<DataSender> {
    friend class Singleton<DataSender>;  // This allows Singleton to access the protected constructor

private:

    long receivedPackets = 0;
    long sentPackets = 0;
    unsigned long lastMQTTConnectionCheckTime = 0;
    unsigned long lastWiFiConnectionCheckTime = 0;
    unsigned long recheckTimeout = 10000;
    unsigned long lastNTPCheckTime = 0;
    unsigned long ntpCheckTimeout = 8 * 60 * 60 * 1000;  // 8 hours in milliseconds

    // Reorder these declarations to match the initialization order in the constructor
    WiFiClient espClient;
    PubSubClient mqttClient;
    //AsyncWebServer server;

    mqttConfig config;
    wifiConfig m_wifiConfig;
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = 3600; // Adjust according to your timezone
    const int   daylightOffset_sec = 3600; // Adjust if daylight saving time is in effect   

    std::vector<Observer*> observers;  // List of observers

public:
    DataSender();  // Constructor is private to prevent external instantiation

    IPAddress localIP;

    static DataSender* instance;        
    static void mqttCallback(char* topic, byte* payload, unsigned int length);


        bool isWiFiConnected() {
            // Check if the WiFi is connected
            unsigned long currentTime = millis();
            if ((currentTime - lastWiFiConnectionCheckTime > recheckTimeout) && WiFi.status()!= WL_CONNECTED) {
                lastWiFiConnectionCheckTime = currentTime;
                return WiFi.status() == WL_CONNECTED;
            } 
            return true;
            //return WiFi.status() == WL_CONNECTED;
        }

        bool isMQTTConnected() {
            // Check if the MQTT is connected
            return mqttClient.connected();
        } 



    bool ConnectMQTT();

    void sendData(const MQTTData& data);
    //static void onConnectionEstablished();  // This can remain static if client is static
    void setup();  // Setup must be static to initialize the static client

    void loop(); // Loop must be static to wor

    void setup_wifi();

    void publish(const char* topic, const char* payload);

    void getTimeFromServer();

    void registerObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void unregisterObserver(Observer* observer) {
        // Remove observer from the list
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

void notifyObservers(const char* topic, byte* payload, unsigned int length) {
    for (Observer* obs : observers) {
        // Concatenate and convert to C-style string
        obs->notify(topic, payload, length);
    }
}


};

#endif // DATASENDER_H
