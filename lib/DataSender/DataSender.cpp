#include "DataSender.h"


DataSender* DataSender::instance = nullptr; // Correct initialization outside of class methods

DataSender::DataSender() : mqttClient(espClient)  {
    // Empty constructor
    DataSender::instance = this;
}




void DataSender::sendData(const MQTTData& data) {
}

void DataSender::mqttCallback(char* topic, byte* payload, unsigned int length) {

    instance->notifyObservers(topic, payload, length);    
}


bool DataSender::ConnectMQTT()
{
    if (!isWiFiConnected()) return false;

    lastMQTTConnectionCheckTime = millis();
    if (mqttClient.connected() == true) {
        return true;
    }

    // Connect to MQTT broker
    mqttClient.setServer(config.broker, config.port);
    mqttClient.setCallback(DataSender::mqttCallback);
    mqttClient.setBufferSize(512);
    delay(1000);
    bool result = mqttClient.connect(config.clientId, config.username, config.password);
    delay(2000);
    if( result == true) {
        mqttClient.subscribe(config.willTopic);
    }
    delay(2000);
    return mqttClient.connected();
}



void DataSender::setup_wifi() {

  delay(500);
  WiFi.begin(m_wifiConfig.ssid, m_wifiConfig.password);
  WiFi.setHostname(SYSTEM_NAME);
  delay(1000);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if ((millis() - start) > 20000) {
        break;
    }
  }
  lastWiFiConnectionCheckTime = millis();

  if (WiFi.status() == WL_CONNECTED)  {
    localIP = WiFi.localIP();
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    getTimeFromServer();
  }
  else {
  }
}

void DataSender::publish(const char* topic, const char* payload)
{   
    unsigned long currentMillis = millis();
    
    // Check WiFi connection and attempt to reconnect if necessary
    if (!isWiFiConnected() && (currentMillis - lastWiFiConnectionCheckTime > recheckTimeout)) {
        lastWiFiConnectionCheckTime = currentMillis;  // Update the last check time
        setup_wifi();
    }

    // Check MQTT connection and attempt to reconnect if necessary
    if (isWiFiConnected() && !isMQTTConnected() && (currentMillis - lastMQTTConnectionCheckTime > recheckTimeout)) {
        lastMQTTConnectionCheckTime = currentMillis;  // Update the last check time
        ConnectMQTT();
    }

    // If both WiFi and MQTT are connected, publish the message
    if (isWiFiConnected() && isMQTTConnected()) {
        mqttClient.publish(topic, payload);
        mqttClient.loop();
    }
}

void DataSender::getTimeFromServer() {

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Wait a bit for time to be set
  delay(2000); // Wait 2 seconds to ensure NTP response

  // Check if time was set
  if(time(nullptr) > 0) {
    Serial.println("Time set successfully");
  } else {
    Serial.println("Failed to set time");
  }


    struct timeval tv;
    gettimeofday(&tv, NULL);

    lastNTPCheckTime = millis();  // Update the last check time
}   




void DataSender::setup() {

    setup_wifi();
    ConnectMQTT();
    publish("system/startup",SYSTEM_NAME);


}













void DataSender::loop() {
    unsigned long currentMillis = millis();
    if (isWiFiConnected() && (currentMillis - lastNTPCheckTime > ntpCheckTimeout )) {
        getTimeFromServer();
    }
    mqttClient.loop();
}




