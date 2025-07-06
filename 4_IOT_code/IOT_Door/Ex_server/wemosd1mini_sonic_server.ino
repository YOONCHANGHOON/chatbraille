#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "password.h"

#define TRIG 16 // D0 (GPIO16)
#define ECHO 14 // D5 (GPIO14)

// Wi-Fi 정보
const char* ssid = "pixar 2.4G";
const char* password = "ehowl8282";

// 서버 주소
const char* serverUrl = "http://192.168.0.42:3000/distance";

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Wi-Fi 연결
  WiFi.begin(ssid, password);
  Serial.print("Wi-Fi 연결 중...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi 연결 완료!");
  Serial.print("IP 주소: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  long duration, distance;

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration * 17 / 1000;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");

    String json = "{\"distance\":" + String(distance) + "}";

    int httpResponseCode = http.POST(json);
    if (httpResponseCode > 0) {
      Serial.print("POST 응답 코드: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("POST 실패, 에러: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  }

  delay(1000);

}