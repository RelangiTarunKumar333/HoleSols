#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Define Wi-Fi and Firebase credentials
#define WIFI_SSID "123456789"
#define WIFI_PASSWORD "123456789"
#define API_KEY "AIzaSyCqMdGJNvmMin0y6jyE7f1GYZYBIeJhwR0"
#define DATABASE_URL "https://holesols-default-rtdb.firebaseio.com/"

// Define pins
#define TRIGGER_PIN  D1
#define ECHO_PIN     D2
#define VIBRATOR_PIN D3
#define BUZZER_PIN   D4
#define LED_PIN      D5

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup() {
  // Initialize pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(VIBRATOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Firebase configuration
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signup successful");
    signupOK = true;
  } else {
    Serial.printf("Firebase signup failed: %s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Measure distance with ultrasonic sensor
  long duration, distance;
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1; // Convert to cm
  Serial.print("Ultrasonic Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Read vibration sensor
  int vibration = !digitalRead(VIBRATOR_PIN);
  Serial.print("Vibration Sensor: ");
  Serial.println(vibration);

  // Control buzzer and LED based on sensor readings
  if (distance < 20) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Buzzer is on");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Buzzer is off");
  }

  if (vibration == HIGH) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED is ON");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED is OFF");
  }

  // Send data to Firebase
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    if (Firebase.RTDB.setInt(&fbdo, "manhole/distance", distance)) {
      Serial.println("Distance data sent successfully");
    } else {
      Serial.printf("Failed to send distance data: %s\n", fbdo.errorReason().c_str());
    }

    if (Firebase.RTDB.setInt(&fbdo, "manhole/vibration", vibration)) {
      Serial.println("Vibration data sent successfully");
    } else {
      Serial.printf("Failed to send vibration data: %s\n", fbdo.errorReason().c_str());
    }
  }

  delay(1000); // Add a 1-second delay
}
