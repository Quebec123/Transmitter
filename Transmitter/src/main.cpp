#include <Arduino.h>
#include "ServerManager.h"
#include "Pins.h"
#include <DFRobot_HumanDetection.h>

ServerManager server;
DFRobot_HumanDetection hu(&Serial1);

void initSensor(String ceilingHeight){
    ceilingHeight.trim(); // Remove any leading/trailing whitespace
    if (ceilingHeight.length() == 0 || !ceilingHeight.toInt()) {
        Serial.println("Invalid ceiling height!");
        return;
    }

    // Convert to uint16_t
    long height = ceilingHeight.toInt(); // Use long to handle larger values temporarily
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
    hu.configLEDLight(hu.eFALLLed, 1);         // Set HP LED switch, it will not light up even if the sensor detects a person present when set to 0.
    Serial.println("2");
    hu.configLEDLight(hu.eHPLed, 1);           // Set FALL LED switch, it will not light up even if the sensor detects a person falling when set to 0.
    Serial.println("3");
    Serial.println("Valid height: " + String(validHeight) + " cm");
    hu.dmInstallHeight(validHeight);                   // Set installation height, it needs to be set according to the actual height of the surface from the sensor, unit: CM.
        Serial.println("4");
    hu.dmFallTime(5);                          // Set fall time, the sensor needs to delay the current set time after detecting a person falling before outputting the detected fall, this can avoid false triggering, unit: seconds.
    hu.dmUnmannedTime(1);                      // Set unattended time, when a person leaves the sensor detection range, the sensor delays a period of time before outputting a no person status, unit: seconds.
    hu.dmFallConfig(hu.eResidenceTime, 200);   // Set dwell time, when a person remains still within the sensor detection range for more than the set time, the sensor outputs a stationary dwell status. Unit: seconds.
    hu.dmFallConfig(hu.eFallSensitivityC, 3);  // Set fall sensitivity, range 0~3, the larger the value, the more sensitive.

    hu.sensorRet();
}
void reset(){
    server.Reset();
    ESP.restart();

}
void setup() {
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, /*rx =*/44, /*tx =*/43);
    //WiFi.setSleep(true);              // Enables automatic Modem-sleep
    delay(10);
    Serial.println("Sleep for 30 seconds if soemthing with deepsleep breaks");
    //delay(30000);
    pinMode(IO1,INPUT);
    pinMode(Button,INPUT);
    attachInterrupt(digitalPinToInterrupt(Button), reset, FALLING);
    // Start the Access Point


    server.startAP();

     //Handle AP requests
    while (server.loopAP()) {
        Serial.println(server.getIP());
        yield();
    }
    Serial.println(server.getCeilingHeight());
    initSensor(server.getCeilingHeight());
    if(hu.getFallData(hu.eFallState)){
        Serial.println("Fall detected!");
        //server.sendFallToHomeAssistant(true);
    }
    else{
        //server.sendFallToHomeAssistant(false);
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
    // Check for human movement range (if needed)
    hu.smHumanData(hu.eHumanMovingRange);

    // Check for presence and fall state
    bool current = hu.smHumanData(hu.eHumanPresence);
    bool fall = hu.getFallData(hu.eFallState);

    // Handle presence toggle
    if (current != last_presence) {
        server.sendPresenceToHomeAssistant(); // Notify Home Assistant of presence change
        Serial.println("Toggle");
    }

    // Handle fall detection
    if (current && fall != last_fall) {
        server.sendFallToHomeAssistant(); // Notify Home Assistant of fall event
        Serial.println("Fall");
    }
    // Update last presence state
    last_fall = fall;
    last_presence = current;
    // Delay for 500ms (consider replacing with non-blocking delay if needed)

}