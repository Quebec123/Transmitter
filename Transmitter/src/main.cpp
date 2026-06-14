/*******************************************************************************
 * @file    [main].[cpp]
 * @brief   [Główna pętla czujnika upadku]
 * * @project [Czujnik updaku oraz alarm]
 * @author  [Wiktor Sobczyński]
 * @date    [2026-06-14]
 * @version V1.0
 * *******************************************************************************/

#include <Arduino.h>
#include "ServerManager.h"
#include "Pins.h"
#include <DFRobot_HumanDetection.h>

ServerManager server;
DFRobot_HumanDetection hu(&Serial1);

void initSensor(String ceilingHeight){
    ceilingHeight.trim();
    if (ceilingHeight.length() == 0 || !ceilingHeight.toInt()) {
        Serial.println("Invalid ceiling height!");
        return;
    }

    long height = ceilingHeight.toInt();
    if (height < 0 || height > 65535) {
        Serial.println("Height out of range!");
        return;
    }

    uint16_t validHeight = static_cast<uint16_t>(height);

    Serial.println("Start initialization");
    while (hu.begin() != 0) {
        Serial.println("init error!!!");
        delay(1000);
    }
    while (hu.configWorkMode(hu.eFallingMode) != 0) {
        Serial.println("error!!!");
        delay(1000);
    }
    Serial.println("1");
    hu.configLEDLight(hu.eFALLLed, 1);
    Serial.println("2");
    hu.configLEDLight(hu.eHPLed, 1);
    Serial.println("3");
    Serial.println("Valid height: " + String(validHeight) + " cm");
    hu.dmInstallHeight(validHeight);
        Serial.println("4");
    hu.dmFallTime(5);
    hu.dmUnmannedTime(1);
    hu.dmFallConfig(hu.eResidenceTime, 200);
    hu.dmFallConfig(hu.eFallSensitivityC, 3);

    hu.sensorRet();
}
void reset(){
    server.Reset();
    ESP.restart();

}
void setup() {
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, /*rx =*/44, /*tx =*/43);

    delay(10);
    Serial.println("Sleep for 30 seconds if soemthing with deepsleep breaks");
    pinMode(IO1,INPUT);
    pinMode(Button,INPUT);
    attachInterrupt(digitalPinToInterrupt(Button), reset, FALLING);


    server.startAP();

    while (server.loopAP()) {
        Serial.println(server.getIP());
        yield();
    }
    Serial.println(server.getCeilingHeight());
    initSensor(server.getCeilingHeight());
    if(hu.getFallData(hu.eFallState)){
        Serial.println("Fall detected!");
    }
    else{
    }
    Serial.print("Radar installation height: ");
    Serial.print(hu.dmGetInstallHeight());
    Serial.println(" cm");
    Serial.print("Fall duration: ");
    Serial.print(hu.getFallTime());
    Serial.println(" seconds");
    Serial.print("Unattended duration: ");
    Serial.print(hu.getUnmannedTime());
    Serial.println(" seconds");
    Serial.print("Dwell duration: ");
    Serial.print(hu.getStaticResidencyTime());
    Serial.println(" seconds");
    Serial.print("Fall sensitivity: ");
    Serial.print(hu.getFallData(hu.eFallSensitivity));
    Serial.println(" seconds");
    Serial.println("===============================");
}
bool last_presence = 0;
bool last_fall = 0;
void loop() {
    hu.smHumanData(hu.eHumanMovingRange);

    bool current = hu.smHumanData(hu.eHumanPresence);
    bool fall = hu.getFallData(hu.eFallState);

    if (current != last_presence) {
        server.sendPresenceToHomeAssistant();
        Serial.println("Toggle");
    }

    if (current && fall != last_fall) {
        server.sendFallToHomeAssistant();
        Serial.println("Fall");
    }
    last_fall = fall;
    last_presence = current;

}