#include <WiFiS3.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTC.h"

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 8

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte authorizedUID[4] = {0x00, 0x00, 0x00, 0x00}; // replace with your card's UID from find_uid.ino

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Austin, TX = Central Time. -5 hrs during Daylight Saving (Mar-Nov), -6 hrs otherwise.
const long utcOffsetSeconds = -5 * 3600;

WiFiUDP ntpUDP;
const char* ntpServer = "pool.ntp.org";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

bool isUnlocked = false;
unsigned long lastClockUpdate = 0;
int lastBuzzedHour = -1;

void sendNTPpacket() {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  ntpUDP.beginPacket(ntpServer, 123);
  ntpUDP.write(packetBuffer, NTP_PACKET_SIZE);
  ntpUDP.endPacket();
}

void syncTimeFromNTP() {
  ntpUDP.begin(2390);

  for (int attempt = 0; attempt < 10; attempt++) {
    sendNTPpacket();
    delay(1000);

    int cb = ntpUDP.parsePacket();
    if (cb) {
      ntpUDP.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = (highWord << 16) | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears + utcOffsetSeconds;

      RTCTime timeToSet(epoch);
      RTC.setTime(timeToSet);
      Serial.println("Time synced!");
      return;
    }
    Serial.print("NTP attempt ");
    Serial.print(attempt + 1);
    Serial.println(" failed, retrying...");
  }
  Serial.println("NTP sync failed after 10 attempts");
}

void showLockedScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Locked");
  lcd.setCursor(0, 1);
  lcd.print("Scan your card");
}

void showRejectedScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");
  lcd.setCursor(0, 1);
  lcd.print("Wrong card");

  tone(BUZZER_PIN, 300, 800); // low buzz for rejection, different from hourly beep

  delay(1500);

  // go back to whatever screen was showing before
  isUnlocked ? showUnlockedScreen() : showLockedScreen();
}

void showUnlockedScreen() {
  RTCTime now;
  RTC.getTime(now);

  int hour24 = now.getHour();
  int hour12 = hour24 % 12;
  if (hour12 == 0) hour12 = 12;
  const char* ampm = (hour24 < 12) ? "AM" : "PM";

  char timeStr[17];
  sprintf(timeStr, "%2d:%02d:%02d %s", hour12, now.getMinutes(), now.getSeconds(), ampm);

  char line0[17];
  char line1[17];
  sprintf(line0, "%-16s", "Unlocked!");
  sprintf(line1, "%-16s", timeStr);

  lcd.setCursor(0, 0);
  lcd.print(line0);
  lcd.setCursor(0, 1);
  lcd.print(line1);
}

bool isAuthorized() {
  if (rfid.uid.size != 4) return false;
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != authorizedUID[i]) return false;
  }
  return true;
}

void checkHourlyBuzz() {
  RTCTime now;
  RTC.getTime(now);

  if (now.getMinutes() == 0 && now.getSeconds() == 0 && now.getHour() != lastBuzzedHour) {
    lastBuzzedHour = now.getHour();
    tone(BUZZER_PIN, 1000, 500); // 1000Hz beep for 500ms
  }
}

void setup() {
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Restarting...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait");
  delay(1500);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  lcd.setCursor(0, 1);

  WiFi.begin(ssid, password);
  int barPos = 0;
  unsigned long wifiScreenStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(barPos % 16, 1);
    lcd.print("#");
    tone(BUZZER_PIN, 1200, 80);
    delay(150);
    Serial.print(".");

    barPos++;
    if (barPos % 16 == 0) {
      lcd.setCursor(0, 1);
      lcd.print("                "); // bar filled the row, clear and restart
    }
  }

  // Keep the "Connecting WiFi" screen up for at least 3 seconds total,
  // even if the connection itself was faster than that.
  while (millis() - wifiScreenStart < 3000) {
    lcd.setCursor(barPos % 16, 1);
    lcd.print("#");
    tone(BUZZER_PIN, 1200, 80);
    delay(150);
    barPos++;
    if (barPos % 16 == 0) {
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }

  Serial.println("WiFi connected");
  lcd.clear();

  RTC.begin();
  syncTimeFromNTP();

  showLockedScreen();
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (isAuthorized()) {
      isUnlocked = !isUnlocked;
      isUnlocked ? showUnlockedScreen() : showLockedScreen();
    } else {
      showRejectedScreen();
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  if (isUnlocked && millis() - lastClockUpdate >= 1000) {
    lastClockUpdate = millis();
    showUnlockedScreen();
  }

  checkHourlyBuzz();
}
