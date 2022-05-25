// RFID Tag: ABC123456789 (Valid)   ABC123456780 (Invalid)

/* Libraries */
#include <SPI.h>                // Arduino SPI library (needed for the SD card)
#include <SD.h>                 // Arduino SD library
#include <Wire.h>               // Arduino Wire library (needed for I2C LCD)
#include <LiquidCrystal_I2C.h>  // Arduino LCD library

/* Macros */
#define pinSensorFront 2
#define pinSensorBack 3
#define pinMotorGate 6
#define pinMotorReceipt 7

/* Set LCD address to 0x27 for a 20 chars and 4 lines display */
LiquidCrystal_I2C lcd(0x27, 20, 4);

/* Global Variables */
// SD card and file
File dataLog;
boolean sd_ok = 0;
// Sensor value
int frontCm = 0, backCm = 0; 
// RFID
int count = 0;
char c;
String id;
// Vehicle data
int small = 0, large = 0;

/* Function Declarations */
long readUltrasonicDistance(int, int);

/* Setup */
void setup() {
  Serial.begin(9600); // Open serial communications
  while(!Serial) {} // Wait for serial port to connect
  
  Serial.print("Initializing SD card... ");
  if(!SD.begin()) { // Initialize SD card
    Serial.println("Initialization failed!"); // Initialiation error
  } else { // Initialization success
    sd_ok = 1;
    Serial.println("Initialization success");
    if(SD.exists("Log.txt") == 0) { // Test if file with name "Log.txt" already exists
      Serial.print("\r\nCreate 'Log.txt' file... ");
      dataLog = SD.open("Log.txt", FILE_WRITE); // Create (&open) file "Log.txt"
      if(dataLog) { // If the file opened okay, write to it:
        Serial.println("File successfully created");
        dataLog.close(); // Close the file
      } else {
        Serial.println("Error creating file");
      }
    }
  }
  Serial.println("");

  if(sd_ok) {
    dataLog = SD.open("Log.txt", FILE_WRITE);
    dataLog.print("Small Vehicle = ");
    dataLog.println(small);
    dataLog.print("Large Vehicle = ");
    dataLog.println(large);
    dataLog.println("");
    dataLog.close();
  }

  pinMode(pinMotorGate, OUTPUT);
  pinMode(pinMotorReceipt, OUTPUT);

  // Initialize the LCD then turn on backlight
  lcd.init();
  lcd.backlight();
  lcd.home();

  lcd.setCursor(0, 0);
  lcd.print("Small Vehicle: ");
  lcd.setCursor(0, 1);
  lcd.print(small);
  lcd.setCursor(0, 2);
  lcd.print("Large Vehicle: ");
  lcd.setCursor(0, 3);
  lcd.print(large);
}

/* Loop */
void loop() {
  frontCm = 0.01723 * readUltrasonicDistance(pinSensorFront, pinSensorFront);
  backCm = 0.01723 * readUltrasonicDistance(pinSensorBack, pinSensorBack);

  // Small vehicle
  if(frontCm <= 50 && backCm > 50) {
    Serial.println("Small vehicle identified");
    Serial.println("Please input your ID");
    
    while(Serial.available() > 0) {
      c = Serial.read();
      count++;
      id += c;
      if(count == 12) {
        // Serial.print(id);
        // break;
        if(id == "ABC123456789") {
          Serial.println("Card is valid. You may proceed");
          
          digitalWrite(pinMotorGate, HIGH);
          digitalWrite(pinMotorReceipt, HIGH);
          delay(3000);
          digitalWrite(pinMotorGate, LOW);
          digitalWrite(pinMotorReceipt, LOW);
          
          small++;
          
          lcd.setCursor(0, 1);
          lcd.print(small);
          lcd.print(" ");
          
          if(sd_ok) {
            dataLog = SD.open("Log.txt", FILE_WRITE);
            dataLog.print("Small Vehicle = ");
            dataLog.println(small);
            dataLog.print("Large Vehicle = ");
            dataLog.println(large);
            dataLog.println("");
            dataLog.close();
          }
          
        } else {
          Serial.println("Card is invalid. Please retry");
          digitalWrite(pinMotorGate, LOW);
          digitalWrite(pinMotorReceipt, LOW);
        }
      }
    }
    Serial.println("");

  // Large vehicle
  } else if(frontCm <= 50 && backCm <= 50) {
    Serial.println("Large vehicle identified");
    Serial.println("Please input your ID");
    
    while(Serial.available() > 0) {
      c = Serial.read();
      count++;
      id += c;
      if(count == 12) {
        // Serial.print(id);
        // break;
        if(id == "ABC123456789") {
          Serial.println("Card is valid. You may proceed");
          
          digitalWrite(pinMotorGate, HIGH);
          digitalWrite(pinMotorReceipt, HIGH);
          delay(3000);
          digitalWrite(pinMotorGate, LOW);
          digitalWrite(pinMotorReceipt, LOW);
          
          large++;
          
          lcd.setCursor(0, 3);
          lcd.print(large);
          lcd.print(" ");
          
          if(sd_ok) {
            dataLog = SD.open("Log.txt", FILE_WRITE);
            dataLog.print("Small Vehicle = ");
            dataLog.println(small);
            dataLog.print("Large Vehicle = ");
            dataLog.println(large);
            dataLog.println("");
            dataLog.close();
          }
          
        } else {
          Serial.println("Card is invalid. Please retry");
          digitalWrite(pinMotorGate, LOW);
          digitalWrite(pinMotorReceipt, LOW);
        }
      }
    }
    Serial.println("");

  // No vehicle
  } else {
    Serial.println("No vehicle identified");
    Serial.println("");
    digitalWrite(pinMotorGate, LOW);
    digitalWrite(pinMotorReceipt, LOW);
  }
  
  count = 0;
  id = "";
  delay(5000);
}

/* Function Definitions */
long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}
