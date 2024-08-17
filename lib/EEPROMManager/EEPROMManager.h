#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <EEPROM.h>
#include "Singleton.h"
#include "Observer.h"
#include "Tools.h"
#include "MQTTTopics.h"
#include "DataSender.h"

#define EEPROM_SIZE 512 // Define the size of EEPROM to use ( bsed on docs it can be up to 11264 )

class EEPROMManager : public Singleton<EEPROMManager>, public Observer {
    friend class Singleton<EEPROMManager>;

protected:
    EEPROMManager() {
        // Initialize EEPROM size
        EEPROM.begin(EEPROM_SIZE); // Adjust size as needed
    }

    private: 

    Tools& tools = Tools::getInstance();
    DataSender& dataSender = DataSender::getInstance();


    void OnEvent(const char* topic, byte* payload, unsigned int length) 
    {

        MQTTMessageObject messageObject = tools.DecodeMessage(payload, length,1);

        // Check if payloadStr is not empty
        if (length > 0) 
        {
            // Decode the JSON message
                String command = messageObject.command;;
                String parameter = messageObject.parameter;

                if (command == "display" && parameter == "get") {
                    // Handle the high temperature command
                    displayMemory();
                    
                }
                else if (command == "clear" && parameter == "set") {
                    // Handle the low temperature command
                    clear();
                }
                else {
                    // Handle unknown command
                    Serial.println("Command not recognized.");
                }
        }




    }    

public:

    bool hadBeenCleared = false;

    ~EEPROMManager() {
        EEPROM.end();
    }

    void begin() {
        EEPROM.begin(EEPROM_SIZE); // Adjust size as needed
        hadBeenCleared = false;
        int getClearState = load(0);
        
        if (getClearState != 0xFFFF) {
            for (int i = 0; i < EEPROM_SIZE; i++) {
                EEPROM.write(i, 0);
            }
            EEPROM.commit(); // Important to commit changes to make them permanent
            EEPROM.put(0, 0x0000);
            EEPROM.commit(); // Important to commit changes to make them permanent
            hadBeenCleared = true;
        }
    }

    // Function to save a value at a specified index
    void save(uint16_t index, uint16_t value) {
        if (index >= 0 && index < (EEPROM_SIZE/sizeof(uint16_t))) { // Ensure the index is within the range

            int addr = index * sizeof(uint16_t); // Calculate the byte address
            EEPROM.put(addr, value);
            EEPROM.commit(); // Important to commit changes to make them permanent
        }
    }

    // Function to load a value from a specified index
    uint16_t load(int index) {
        if (index >= 0 && index < 256) {
            int addr = index * sizeof(uint16_t); // Calculate the byte address
            int value = 0;
            EEPROM.get(addr, value);
            return value; // Return the value
        }
        return 0; // Return a default value if index is out of range
    }

    void clear() {
        for (int i = 0; i < EEPROM_SIZE; i++) {
            EEPROM.write(i, 0);
        }
        EEPROM.commit(); // Important to commit changes to make them permanent
        EEPROM.end();
    // Publish the payload
    String payload = "{\"data\":[memory cleared]}"; // Create the JSON payload
    String topic = String(CENTRAL_SYSTEM) + "/" + String(EEPROM_MANAGER_TOPIC); // Create the topic string
    dataSender.publish(topic.c_str(), payload.c_str()); // Make sure to replace "memory/topic" with your actual topic
    }


void saveStringToEEPROM(int start, const String &value) {
    for (unsigned int i = 0; i < value.length(); ++i) {
        EEPROM.write(start + i, value[i]);
    }
    EEPROM.write(start + value.length(), '\0'); // Null-terminator for the string
    EEPROM.commit();
}

void loadStringFromEEPROM(int start, String &value) {
    char ch;
    value = "";
    for (int i = start; i < EEPROM_SIZE; ++i) {
        ch = EEPROM.read(i);
        if (ch == '\0') break;
        value += ch;
    }
}


void displayMemory() {
    String payloadStr = ""; // Initialize the payload string
    for (int i = 0; i < 10; i++) {
        // Read a byte from EEPROM
        uint16_t value = load(i);  // Read a byte from EEPROM

        // Append the integer value to the string, convert byte to String
        if (i > 0) {
            payloadStr += ",";  // Add a comma before each number after the first
        }
        payloadStr += String(value);  // Convert the byte to a String and append it
    }

    // Publish the payload
    String payload = "{\"data\":[" + payloadStr + "]}"; // Create the JSON payload
    String topic = String(CENTRAL_SYSTEM) + "/" + String(EEPROM_MANAGER_TOPIC); // Create the topic string
    dataSender.publish(topic.c_str(), payload.c_str()); // Make sure to replace "memory/topic" with your actual topic
}



    void notify(const char* topic, byte* payload, unsigned int length) override {
    // Handle the notification, e.g., update temperature controls
        if (strstr(topic, EEPROM_MANAGER_TOPIC ) != NULL ) { 
            Serial.println("EEPROMManager notified - topic: " + String(topic));
            OnEvent(topic, payload, length);
        }
    }

};

#endif // EEPROM_MANAGER_H
