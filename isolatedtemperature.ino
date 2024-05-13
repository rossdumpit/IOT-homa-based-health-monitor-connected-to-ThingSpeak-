#define BLYNK_TEMPLATE_ID "TMPL5JINgfPpf"
#define BLYNK_TEMPLATE_NAME "nodemcu"
#define BLYNK_AUTH_TOKEN "zmomWflbTekcWvkANA9MuYcVBSvyABqf"

#include <DallasTemperature.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

// Define constants
#define ONE_WIRE_BUS 2 // D4 pin of NodeMCU
#define RX_PIN D6        // SRX = D2
#define TX_PIN D5        // STX = D1
#define TEMPERATURE_INTERVAL 2000 // 2 seconds delay

char ssid[] = "AndroidAP3b82"; // Your network SSID (name)
char pass[] = "jppb8534"; // Your network password
char auth[] = "zmomWflbTekcWvkANA9MuYcVBSvyABqf"; // Authentication Token Sent by Blynk


SoftwareSerial SUART(RX_PIN, TX_PIN);


OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature.

unsigned long lastTempRequest = 0;
int delayInMillis = 2000; // 2 seconds delay
int heartRate; // Declare heartRate outside the if block
int spo2;      // Declare spo2 outside the if block

void setup(void)
{
  Serial.begin(9600);
  SUART.begin(9600);
  sensors.begin();
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  
  WiFi.mode(WIFI_STA);

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
    Serial.println("\nConnected to WiFi");
  }

  Blynk.begin(auth, ssid, pass);

}

void loop(void)
{
  Blynk.run();


  if (millis() - lastTempRequest >= delayInMillis)
  {
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("Temperature is: ");
    Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire

    lastTempRequest = millis();

    delay(500); // Additional delay if needed

    if (SUART.available() > 0)
    {
      // Read the heart rate from Arduino
      heartRate = SUART.parseInt();
      SUART.read(); // Consume the comma
      // Read the SpO2 from Arduino
      spo2 = SUART.parseInt();

      // Now you can use 'heartRate' and 'spo2' in your NodeMCU logic
      Serial.print("Received Heart Rate: ");
      Serial.print(heartRate);
      Serial.print(" bpm / SpO2: ");
      Serial.println(spo2);
    }
  }
    // Read sensor values
  int sensorValue1 = sensors.getTempCByIndex(0);
  int sensorValue2 = heartRate;
  int sensorValue3 = spo2;

  // Clear the LCD screen
  lcd.clear();
  // Print sensor values on the LCD
  lcd.setCursor(0, 0);
  lcd.print("T: " + String(sensorValue1) + " O2: " + String(sensorValue3));
  

  lcd.setCursor(0, 1);
  lcd.print("HR: ");
  lcd.print(sensorValue2);

  Blynk.virtualWrite(V0, sensorValue2);
  Blynk.virtualWrite(V1, sensorValue3);
  Blynk.virtualWrite(V2, sensorValue1);

  if(sensorValue1 > 39){
   // Blynk.email("shameer50@gmail.com", "Alert", "Temperature over 28C!");
    Blynk.logEvent("temp_alert1","Temp above 39 degree");
  }
  if(sensorValue3 < 50){
   // Blynk.email("shameer50@gmail.com", "Alert", "Temperature over 28C!");
    Blynk.logEvent("saturation_alert1","Oxygen saturation below 50%");
  }
  if(sensorValue2 < 25){
   // Blynk.email("shameer50@gmail.com", "Alert", "Temperature over 28C!");
    Blynk.logEvent("heartrate_alert1","Heart rate below 40");
  }
       




  delay(500); // Additional delay if needed

}