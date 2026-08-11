# Secure-Arduino-Clock
## Parts Needed

- Arduino Uno R4 WiFi
- QAPASS 16x2 LCD (I2C backpack version)
- RFID-RC522 reader module
- 2x RFID key cards/tags
- Small speaker or piezo buzzer (from kit)
- Jumper wires (male-to-female, ~12 needed)
- USB cable (for programming/power)

---

## Wiring: RFID-RC522 Reader

⚠️ **This module runs on 3.3V — do NOT connect it to 5V, it can damage the module.**

| RC522 Pin | Connects to Arduino Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| RST | Pin 9 |
| MISO | Pin 12 |
| MOSI | Pin 11 |
| SCK | Pin 13 |
| SDA (labeled SS) | Pin 10 |
| IRQ | Not connected |

---

## Wiring: LCD (I2C 16x2)

| LCD Pin | Connects to Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | SDA (dedicated pin near AREF, or A4) |
| SCL | SCL (dedicated pin near AREF, or A5) |

---

## Wiring: Speaker/Buzzer

| Speaker Wire | Connects to Arduino Pin |
|---|---|
| Signal wire | Pin 8 |
| Other wire | GND |

Polarity doesn't matter for this type of speaker — either wire can go to either pin.

---

## Power Notes

- The Arduino itself is powered via USB cable to your computer (or a USB power adapter).
- The RC522 and LCD both draw their power *from* the Arduino — no separate power source needed.
- Double check the RC522 is on the 3.3V pin specifically, not 5V, before plugging in USB.

---

## Pin Summary (Arduino side)

| Arduino Pin | Used For |
|---|---|
| 3.3V | RC522 VCC |
| 5V | LCD VCC |
| GND | RC522 GND, LCD GND, Speaker GND (shared) |
| Pin 8 | Speaker signal |
| Pin 9 | RC522 RST |
| Pin 10 | RC522 SDA/SS |
| Pin 11 | RC522 MOSI |
| Pin 12 | RC522 MISO |
| Pin 13 | RC522 SCK |
| SDA (or A4) | LCD SDA |
| SCL (or A5) | LCD SCL |
