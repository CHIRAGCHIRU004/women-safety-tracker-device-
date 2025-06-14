#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Initialize the LCD with direct pin connections
LiquidCrystal lcd(13, 12, 11, 10, 9, 8); // RS, E, D4, D5, D6, D7

// GSM module connections
SoftwareSerial gsmSerial(2, 3); // RX, TX

// Create an instance of the TinyGPS++ object
TinyGPSPlus gps;

// Phone numbers
const char* phoneNumbers[] = {
  "+6360214617",
  "+88******70",
  "+73******71",
  "+63*******691",
  "+99*******67"
};

const int numOfPhones = 5;

// Push button and buzzer pins
const int buttonPin = 7;
const int buzzerPin = 6;

void setup() {
  // Initialize serial monitor for debugging
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2); // Set up the LCD's number of columns and rows
  lcd.setCursor(0, 0);
  lcd.print("LCD Initialized");
  Serial.println("LCD Initialized");

  // Start serial communication with GPS module
  Serial.begin(9600); // Using hardware serial for GPS

  // Start serial communication with GSM module
  gsmSerial.begin(9600);

  // Initialize button and buzzer pins
  pinMode(buttonPin, INPUT); // Use external pull-down resistor if needed
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Setup complete");
}

void loop() {
  // Read data from GPS module
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  // Check if the push button is pressed
  if (digitalRead(buttonPin) == HIGH) { // Use LOW if using INPUT_PULLUP
    // Display "Help me!" on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Help me!");
    Serial.println("Button pressed, displaying 'Help me!'");

    if (gps.location.isUpdated()) {
      // Format GPS data as a Google Maps link
      String gpsData = "Lat: " + String(gps.location.lat(), 6) + " Lng: " + String(gps.location.lng(), 6);
      String mapsLink = "https://www.google.com/maps?q=" + String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);

      // Print GPS data to serial monitor for debugging
      Serial.println(gpsData);
      Serial.println(mapsLink);

      // Send GPS data and Google Maps link via GSM module to all phone numbers
      for (int i = 0; i < numOfPhones; i++) {
        sendSMS(phoneNumbers[i], gpsData);
        sendSMS(phoneNumbers[i], mapsLink);
      }

      // Activate buzzer
      digitalWrite(buzzerPin, HIGH);
      delay(5000); // Adjusted delay to 5000ms (5 seconds)
      digitalWrite(buzzerPin, LOW);
      delay(5000); // Adjusted delay to 5000ms (5 seconds)
    }
  }
}

void sendSMS(const char* phoneNumber, const String& message) {
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\""); 
  delay(1000);
  gsmSerial.println(message); // Send message
  delay(1000);
  gsmSerial.write(26); // ASCII code for CTRL+Z to send the SMS
  delay(1000);
}
