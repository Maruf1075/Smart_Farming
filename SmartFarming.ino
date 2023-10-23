
#include "LiquidCrystal_I2C.h"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

//Initialize the LCD display
LiquidCrystal_I2C lcd(0x3F, 16, 2);


char auth[] = "PP9-yRCLLCBd0042ZLopjRnE0IRPvAvq";  //Enter your Blynk Auth token... new
//char auth[] = "k9i7CrcooU4NW1Iu149y0RxF3-n_JMQK";
char ssid[] = "mrBeast";  //Enter your WIFI SSID
char pass[] = "maruf1075";  //Enter your WIFI Password

DHT dht(D4, DHT11);//(DHT sensor pin,sensor type)  D4 DHT11 Temperature Sensor
BlynkTimer timer;

//Define component pins
#define soil D5    //A0 Soil Moisture Sensor


void checkPhysicalButton();
int relay1State = LOW;
int pushButton1State = HIGH;
#define RELAY_PIN_1       D3   //D3 Relay
#define PUSH_BUTTON_1     D7   //D7 Button
#define VPIN_BUTTON_1    V12 

//Create three variables for pressure
double T, P;
char status;



void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  
  pinMode(RELAY_PIN_1, OUTPUT);
  digitalWrite(RELAY_PIN_1, LOW);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
//  digitalWrite(RELAY_PIN_1, relay1State);


  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("  Initializing  ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(11, 1);
  lcd.print("W:OFF");
  //Call the function
  timer.setInterval(100L, soilMoistureSensor);    //100L
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(500L, checkPhysicalButton);
}


//Get the DHT11 sensor values
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

}


//Get the soil moisture values
void soilMoistureSensor() {
  int value = digitalRead(soil);
  value = value * 30 ;
//  value = map(value, 0, 1024, 0, 100);
//  value = (value - 100) * -1;
//  Serial.println(value);
//
//if(digitalRead(sensorPin) == HIGH)
//  {
//    digitalWrite(ledPin, HIGH);
//  } 
//  else 
//  {
//    digitalWrite(ledPin, LOW);
//    delay(1000);
//  }

  Blynk.virtualWrite(V3, value);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(value);
  lcd.print(" ");

}


BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  Serial.println(relay1State);
  if(relay1State==1){
      digitalWrite(RELAY_PIN_1, LOW);
  }
  else{
      digitalWrite(RELAY_PIN_1, HIGH);
  }

//  digitalWrite(RELAY_PIN_1, relay1State);
}

void checkPhysicalButton()
{
  if (digitalRead(PUSH_BUTTON_1) == LOW) {
    // pushButton1State is used to avoid sequential toggles
    if (pushButton1State != LOW) {

      // Toggle Relay state
      relay1State = !relay1State;
      digitalWrite(RELAY_PIN_1, relay1State);

      // Update Button Widget
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
    }
    pushButton1State = LOW;
  } else {
    pushButton1State = HIGH;
  }
}


void loop() {

  if (relay1State == HIGH)
  {
    lcd.setCursor(11, 1);
    lcd.print("W:ON ");
    }
    else if (relay1State == LOW)
    {
      lcd.setCursor(11, 1);
      lcd.print("W:OFF");
      }
     
      
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer

  }
