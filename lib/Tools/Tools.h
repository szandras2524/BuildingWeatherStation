#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <Arduino.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <time.h>

#include "Singleton.h"
#include "Config.h"
#include "ArduinoJson.h"



struct MQTTMessageObject{
    String command;
    String parameter;
    String valueString;
    uint8_t value8_t;
    uint16_t value16_t;
    uint32_t value32_t;
    bool valueBool;
};



class Tools : public Singleton<Tools> {
    friend class Singleton<Tools>;  // This allows Singleton to access the protected constructor


    private:
        Tools() {}  // Private so that it can  not be called

    public:
        

    void splitString(const std::string& input, const std::unordered_map<std::string, uint16_t*>& keyMap) {
        std::stringstream ss(input);
        std::string temp;

        while (std::getline(ss, temp, ';')) {
            std::stringstream pairStream(temp);
            std::string key, value;

            if (std::getline(pairStream, key, '=') && std::getline(pairStream, value)) {
                auto it = keyMap.find(key);
                if (it != keyMap.end()) {
                    *(it->second) = std::stoi(value);
                }
            }
        }
    }


    JsonDocument DecodeJson(String jsonString) {
        JsonDocument jsonDoc;

        DeserializationError error = deserializeJson(jsonDoc, jsonString);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return jsonDoc;
        }

        return jsonDoc;
    }


    MQTTMessageObject DecodeMessage(byte* payload, unsigned int length, uint8_t valueType = 0) {
        MQTTMessageObject messageObject;
        messageObject.command = "";
        messageObject.parameter = "";
        messageObject.valueString = "";
        messageObject.value8_t = 0;
        messageObject.value16_t = 0;
        messageObject.value32_t = 0;
        messageObject.valueBool = false;

    String payloadStr = "";
    for (unsigned int i = 0; i < length; i++)
    {
        payloadStr += (char)payload[i];
    }    

    // Check if payloadStr is not empty
    if (payloadStr.length() > 0) 
    {
        // Decode the JSON message
        JsonDocument message = DecodeJson(payloadStr);
        // Check if the message is valid
        Serial.println(message.size());
        if (message.size() > 0)
        {
            messageObject.command = message["command"].as<String>();
            messageObject.parameter = message["parameter"].as<String>();
            if ( valueType == 0) {
                messageObject.valueString = message["value"].as<String>();
            }
            else if ( valueType == 1) {
                messageObject.value8_t = message["value"].as<uint8_t>();
            }
            else if ( valueType == 2) {
                messageObject.value16_t = message["value"].as<uint16_t>();
            }
            else if ( valueType == 3) {
                messageObject.value32_t = message["value"].as<uint32_t>();
            }
            else if ( valueType == 4) {
                messageObject.valueBool = message["value"].as<bool>();
            }
        }
    }
        return messageObject;
    }


bool isCurrentTimeInRange(int currentHour, int currentMinute, int currentSecond, 
                          int startHour, int startMinute, int startSecond, 
                          int stopHour, int stopMinute, int stopSecond) {
  if (currentHour < startHour || currentHour > stopHour) return false;
  if (currentHour == startHour && (currentMinute < startMinute || (currentMinute == startMinute && currentSecond < startSecond))) return false;
  if (currentHour == stopHour && (currentMinute > stopMinute || (currentMinute == stopMinute && currentSecond > stopSecond))) return false;
  return true;
}    

void parseTime(String timeStr, int &hour, int &minute, int &second) {
  hour = timeStr.substring(0, 2).toInt();
  minute = timeStr.substring(2, 4).toInt();
  second = timeStr.substring(4, 6).toInt();
}

void getLocalTime(tm &timeinfo) {
        struct timeval tv;
    // Set the timezone
    setenv("TZ", "Europe/Budapest", 1);
    tzset();

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &timeinfo);
    return;
}

String getLocalTimeString() {
    struct tm timeinfo;
    getLocalTime(timeinfo);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer);
}

String getUptimeString(tm &startTimeinfo) {

    int days, hours, minutes;
    tm end;
    getLocalTime(end);

   time_t startTime = mktime(&startTimeinfo);
    time_t endTime = mktime(&end);

    double difference = difftime(endTime, startTime);

    days = difference / (60 * 60 * 24);
    difference -= days * 60 * 60 * 24;

    hours = difference / (60 * 60);
    difference -= hours * 60 * 60;

    minutes = difference / 60;
    // Create a buffer to hold the formatted string
    char buffer[50];
    sprintf(buffer, "%d napja, %d oraja, %d perce", days, hours, minutes);
    return String(buffer);
}

String getTimeString(tm &timeinfo) {
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer);
}


String getLocalShortTimeString() {
    struct tm timeinfo;
    getLocalTime(timeinfo);
    char buffer[20];
    strftime(buffer, 20, "%H:%M", &timeinfo);
    return String(buffer);
}



bool isCurrentTimeStringInRange(String parameter) {
    /*
    struct timeval tv;
    // Set the timezone
    setenv("TZ", "Europe/Budapest", 1);
    tzset();

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &timeinfo);
    */
    struct tm timeinfo;
    getLocalTime(timeinfo);

    // Split the parameter string by semicolon to get individual time ranges
    int currentHour = timeinfo.tm_hour;
    int currentMinute = timeinfo.tm_min;
    int currentSecond = timeinfo.tm_sec;

    int startHour, startMinute, startSecond;
    int stopHour, stopMinute, stopSecond;

    int semicolonIndex = 0;
    while (semicolonIndex != -1) {
        int nextSemicolonIndex = parameter.indexOf(';', semicolonIndex);
        String timeRange = parameter.substring(semicolonIndex, nextSemicolonIndex == -1 ? parameter.length() : nextSemicolonIndex);
        semicolonIndex = nextSemicolonIndex == -1 ? -1 : nextSemicolonIndex + 1;

        int commaIndex = timeRange.indexOf(',');
        String startTime = timeRange.substring(0, commaIndex);
        String stopTime = timeRange.substring(commaIndex + 1);

        parseTime(startTime, startHour, startMinute, startSecond);
        parseTime(stopTime, stopHour, stopMinute, stopSecond);

        if (isCurrentTimeInRange(currentHour, currentMinute, currentSecond, startHour, startMinute, startSecond, stopHour, stopMinute, stopSecond)) {
            return true;
        }
    }

    return false;
    }


};




#endif