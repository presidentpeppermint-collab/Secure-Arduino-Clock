

#⚠️Warning This entire project is in C++ do not edit unless you know C++. 

#-Good luck PresidentPeppermint

# Secure Arduino Clock — RFID Access Panel

An RFID-locked clock display built on an Arduino Uno R4 WiFi. Scan an authorized card to unlock the display and show the live time (synced automatically over WiFi). Scan an unauthorized card and it gets rejected with a buzz.

---

## Parts List

| Part | Qty | Notes |
|---|---|---|
| Arduino Uno R4 WiFi | 1 | Main controller — has built-in WiFi and RTC |
| RC522 RFID Reader Module | 1 | Runs on 3.3V, not 5V |
| 16x2 I2C LCD Display | 1 | Shows locked/unlocked status and time |
| RFID Key Cards/Tags | 2+ | One (or more) authorized, extras to test rejection |
| Small Speaker or Piezo Buzzer | 1 | Wired directly to a digital pin |
| Jumper Wires (male-to-female) | ~12 | For direct pin connections, no breadboard required |
| USB Cable | 1 | For programming and power |

No separate power supply is needed — the reader, display, and speaker all draw power from the Arduino itself.

---

## Wiring

### RC522 → Arduino

⚠️ **VCC must go to 3.3V, not 5V — connecting it to 5V can damage the module.**

| RC522 Pin | Arduino Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| RST | Pin 9 |
| MISO | Pin 12 |
| MOSI | Pin 11 |
| SCK | Pin 13 |
| SDA (labeled SS) | Pin 10 |
| IRQ | Not connected |

### LCD → Arduino

| LCD Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | SDA (dedicated pin near AREF, or A4) |
| SCL | SCL (dedicated pin near AREF, or A5) |

### Speaker → Arduino

| Speaker Wire | Arduino Pin |
|---|---|
| Signal | Pin 8 |
| Other | GND |

Polarity doesn't matter for this type of speaker — either wire can go to either pin.

---

## Software Setup

### 1. Install the Arduino IDE

1. Go to [arduino.cc/en/software](https://www.arduino.cc/en/software) and download the IDE for your operating system (Windows, macOS, or Linux).
2. Run the installer and follow the prompts, keeping the default options.
3. Open the Arduino IDE once installed.

### 2. Install board support for the Uno R4 WiFi

1. In the Arduino IDE, go to **Tools → Board → Boards Manager** (or click the boards icon in the left sidebar).
2. Search for **"Uno R4"**.
3. Install the package called **Arduino UNO R4 Boards**.
4. Once installed, plug in your Arduino via USB, then go to **Tools → Board** and select **Arduino UNO R4 WiFi**.
5. Go to **Tools → Port** and select the COM port your Arduino shows up as.

### 3. Install required libraries

1. Open the **Library Manager** (icon on the left sidebar, looks like stacked books).
2. Search for and install each of these:
   - **MFRC522** by GithubCommunity
   - **LiquidCrystal I2C** by Frank de Brabander
3. The **RTC** and **WiFiS3** libraries are already included with the Uno R4 board package — no separate install needed.

---

## Getting the Code

This repo contains two sketches:

- **`find_uid.ino`** — a helper sketch that reads and prints any scanned card's unique ID. Use this first to find your own card's UID.
- **`rfid_lock.ino`** — the main sketch that runs the actual lock, display, and buzzer logic.

### To copy the code:

1. Click on either file above in this repository.
2. Click the **copy icon** in the top-right corner of the code view (or select all the text and copy it manually).
3. In the Arduino IDE, go to **File → New Sketch**.
4. Delete the placeholder code in the editor, then paste in the copied code.
5. Save the sketch. The IDE will ask to put it in a matching folder name — click **OK**, this is normal.

---

## Step 1: Find Your Card's UID

1. Open `find_uid.ino` in the Arduino IDE (using the copy/paste method above).
2. Upload it (click the arrow button in the top-left).
3. Open the **Serial Monitor** (magnifying glass icon, top-right), and set the baud rate to **9600**.
4. If nothing appears, press the small reset button on the Arduino board.
5. Hold your card against the RC522's antenna coil (the square area in the middle of the board).
6. Copy the UID that prints out (e.g. `66 7D A1 04`) — you'll need it next.

---

## Step 2: Set Up and Upload the Main Sketch

1. Open `rfid_lock.ino` in the Arduino IDE.
2. Find this line near the top:
   ```cpp
   byte authorizedUID[4] = {0x00, 0x00, 0x00, 0x00};
   ```
   Replace the four values with your card's UID from Step 1 (format each byte as `0x` followed by the hex pair, e.g. `0x66, 0x7D, 0xA1, 0x04`).
3. Find these lines:
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
   Replace with your own WiFi network name and password. **Do not commit your real credentials back to a public repo** — keep them only in your local copy on the Arduino.
4. Also check the UTC offset line matches your timezone:
   ```cpp
   const long utcOffsetSeconds = -5 * 3600;
   ```
5. Upload the sketch.

---

## Testing

- On boot, the LCD shows a WiFi connection screen with a loading bar and beeps, then the time syncs automatically over the internet.
- Once ready, the display shows **"Locked / Scan your card."**
- Scan your authorized card → display shows **"Unlocked!"** with the live time.
- Scan it again → goes back to **Locked**.
- Scan any other card → display shows **"Access Denied"** with a low buzz, then returns to whatever state it was in.
- Every hour on the hour, the speaker plays a short chime.

---

## License

See [LICENSE](./LICENSE) for details.
