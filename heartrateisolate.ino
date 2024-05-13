#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <SoftwareSerial.h>

#define REPORTING_PERIOD_MS 1000
#define DELAY_BEFORE_SENDING 5000  // 5 seconds delay
#define LED_PIN 13 // Assuming you are using built-in LED on pin 13

// Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial SUART(5, 6);

// Create a PulseOximeter object
PulseOximeter pox;

// Time at which the last beat occurred
uint32_t tsLastReport = 0;
uint32_t tsLastSUARTSend = 0;

// Callback routine is executed when a pulse is detected
void onBeatDetected() {
    Serial.println("Beat!");
}

void setup() {
    Serial.begin(9600);
    SUART.begin(9600);

    Serial.print("Initializing pulse oximeter..");

    // Initialize sensor
    if (!pox.begin()) {
        Serial.println("FAILED");
        for (;;)
            ;
    } else {
        Serial.println("SUCCESS");
    }

    // Configure sensor to use 7.6mA for LED drive
    pox.setIRLedCurrent(MAX30100_LED_CURR_11MA);

    // Register a callback routine
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
    // Read from the sensor
    pox.update();

    // Grab the updated heart rate and SpO2 levels
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        int heartRate = pox.getHeartRate();
        int spo2 = pox.getSpO2();

        Serial.print("Heart rate:");
        Serial.print(heartRate);
        Serial.print("bpm / SpO2:");
        Serial.print(spo2);
        Serial.println("%");


        // Check conditions for lighting up the LED
        if (heartRate < 50 || heartRate > 100 || spo2 < 80) {
            digitalWrite(LED_PIN, HIGH); // Turn on the LED
        } else {
            digitalWrite(LED_PIN, LOW); // Turn off the LED
        }

        tsLastReport = millis();

        // Check if it's time to send data to NodeMCU
        if (millis() - tsLastSUARTSend > DELAY_BEFORE_SENDING) {
            // Send heart rate to NodeMCU
            SUART.print(heartRate);
            SUART.print(",");

            // Send SpO2 to NodeMCU
            SUART.println(spo2);

            // Update the timestamp for the last SUART send
            tsLastSUARTSend = millis();
        }
    }
}