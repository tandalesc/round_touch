#include "device/hw/drivers/network/ArduinoNetwork.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>

#include "config/NetworkConfig.h"

void ArduinoNetwork::init() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed.");
  }
}

bool ArduinoNetwork::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

HttpResponse ArduinoNetwork::get(const char *url, const char *authHeader) {
  HttpResponse response;
  if (!isConnected()) return response;

  HTTPClient http;
  http.begin(url);
  if (authHeader) {
    http.addHeader("Authorization", authHeader);
  }

  response.statusCode = http.GET();
  if (response.statusCode > 0) {
    response.body = http.getString();
  }
  http.end();
  return response;
}

HttpResponse ArduinoNetwork::post(const char *url, const char *body,
                                  const char *contentType,
                                  const char *authHeader) {
  HttpResponse response;
  if (!isConnected()) return response;

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", contentType);
  if (authHeader) {
    http.addHeader("Authorization", authHeader);
  }

  response.statusCode = http.POST(body);
  if (response.statusCode > 0) {
    response.body = http.getString();
  }
  http.end();
  return response;
}
