/***************************************************
 * @author : András Székely
 * @date : 2024-08-17
 * @product : Weather Station and MQTT broker sender
 * @details : Measurement the outside environment parameters 
 *            and sendinvia MQTT broker to house system for further processing
****************************************************/
#include <Arduino.h>
#include <stdlib.h>

#include "Config.h"
#include "Singleton.h"
#include "Observer.h" // Include the Observer header
#include "main.h"
#include "EEPROMManager.h"


DataSender& dataSender = DataSender::getInstance();


#include <Arduino.h>
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}

