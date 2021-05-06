#include <WiFi.h>

#define LED_PIN 2

const char* ssid     = "your-ssid";
const char* password = "your-pass";
const uint16_t zip   = 10001;

const char* host = "umbrella-hook.herokuapp.com";
const uint16_t port = 80;

void setup() {
  // Init Board
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Start WiFi
  // TODO pick a WiFi provisioning flow
  // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/provisioning/provisioning.html
  WiFi.begin(ssid, password);

  // Wait for connection
  Serial.print("Waiting for WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
}

void loop() {
  Serial.print("Connecting to ");
  Serial.println(host);

  // Start HTTP Client
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection failed.");
    delay(5000);
    return;
  }

  // Send request to Server via Client
  client.print(String("GET ") + "http://" + host + "/" + zip + " HTTP/1.0\r\n" +
     "Host: " + host + "\r\n" +
     "Connection: close\r\n\r\n");

  // Wait for response from Server
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Parse response from Server and glow blue if "rain"
  digitalWrite(LED_PIN, LOW);
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
    if (line == "rain") {
      digitalWrite(LED_PIN, HIGH);
    }
  }
  
  Serial.println("Waiting 30 seconds before restarting...");
  delay(30000);
}
