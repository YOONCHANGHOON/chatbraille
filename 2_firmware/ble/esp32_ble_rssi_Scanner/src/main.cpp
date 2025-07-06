
#include <WiFi.h>
#include <HTTPClient.h>

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>


const char* ssid = "youaregourgers"; // Wi-Fi SSID
const char* password = "ehowl8282"; // Wi-Fi Password

// 서버 URL
const char* serverURL = "http://34.228.165.50:8080/braille/convert";
// const char* serverURL = "http://192.168.230.220:3000/data";

// 임의로 생성할 BLE MAC 주소
String fakeBleMacAddress = "AA:BB:CC:DD:EE:FF";


unsigned long loop_t = 0;
unsigned long timer_0 = 0;
unsigned long timer_1 = 0;
unsigned long timer_2 = 0;
uint8_t motorBits = 1; // 1 = 0b00000001

float latestDistance = -1;
bool targetFound = false;


// millis()를 사용한 스캔 간격Serial.println("uart send.");
unsigned long lastScanTime = 0;
const unsigned long interval = 500; // 1초마다

int scanTime = 1; // 스캔 시간(초)
BLEScan* pBLEScan;

// 타겟 장치 이름 또는 MAC 주소
const std::string targetName = "ESP32_A";
// const std::string targetMac = "XX:XX:XX:XX:XX:XX"; // MAC으로도 가능

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == targetName) {
      int rssi = advertisedDevice.getRSSI();
      Serial.println("🎯 ESP32_A 발견!");
      Serial.print("📶 RSSI: ");
      Serial.print(rssi);
      Serial.print(" dBm");

      // 거리 추정 (간단한 공식)
      float txPower = -59; // 1m 기준 RSSI (환경에 따라 보정 필요)
      float n = 2.0;        // 감쇠 지수
      float distance = pow(10.0, ((txPower - rssi) / (10 * n)));

      latestDistance = distance;

      Serial.print(" 📏 추정 거리: ");
      Serial.print(distance, 2);
      Serial.println(" m");

      if (distance < 0.7) { // 5cm 이하
        uint8_t motorBits = 0b111111;
        Serial1.write(motorBits); // 모든 모터를 정지 상태로 설정
        Serial1.write('\n');
        Serial.println("uart send.");
      } else {

        uint8_t motorBits = 0b000000;
        Serial1.write(motorBits); // 모든 모터를 정지 상태로 설정
        Serial1.write('\n');
        Serial.println("uart send.");
      }
    }
  }
};


void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17); // Serial1 핀 설정 (RX: 16, TX: 17)

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
  Serial.println("ESP32-WROOM-32D PCA9685 Test");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); 
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); 

  Serial.println("🔍 BLE 스캔 시작...");
}

void loop() {
  if(millis() - loop_t > 100){
    loop_t = millis();
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    pBLEScan->clearResults();
    // Serial1.println("iam here~!");
  }
  if(millis() - timer_0 > 1000)
  {
    // timer_0 = millis();
    // motorBits++;
    // if(motorBits > 0b111111) {
    //   motorBits = 0b000000; // 모터 비트 초기화
    // }
    // Serial1.write(motorBits); // 모든 모터를 정지 상태로 설정
    // Serial1.write('\n');
    // Serial.print(motorBits, BIN);
    // Serial.print(" --> ");
    // Serial.println("uart send.");

      // HTTP POST 요청 보내기
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverURL);
      http.addHeader("Content-Type", "application/json");

      String jsonData = "{\"brailleData\":\"" + fakeBleMacAddress + "\"}";

      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("✅ Server Response:");
        Serial.println(response);
      } else {
        Serial.print("❌ HTTP Request failed. Code: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    } else {
      Serial.println("Wi-Fi disconnected. Reconnecting...");
      WiFi.begin(ssid, password);
    }
  }
}
