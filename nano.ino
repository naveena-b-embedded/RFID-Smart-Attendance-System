#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial rfid(2, 3); // RX, TX

const char* cardIDs[] = {
  "5900871BA96C",  // NAVEENA
  "59007B4C87E9",  // ARUN
  "123456789ABC"   // PRIYA
};

const char* names[] = {
  "NAVEENA",
  "ARUN",
  "PRIYA"
};

bool present[3] = {false, false, false};

String cardBuffer = "";
unsigned long lastByteTime = 0;

void setup() {
  Serial.begin(9600);
  rfid.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("SMART ATTENDANCE");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card");

  Serial.println("Smart Attendance Ready");
}

void loop() {
  while (rfid.available()) {
    char c = rfid.read();

    if ((c >= '0' && c <= '9') ||
        (c >= 'A' && c <= 'F') ||
        (c >= 'a' && c <= 'f')) {

      if (c >= 'a' && c <= 'f') {
        c = c - 32;
      }

      cardBuffer += c;
      lastByteTime = millis();

      // EM-18 card ID is usually 12 characters
      if (cardBuffer.length() == 12) {
        processCard(cardBuffer);
        cardBuffer = "";
      }

      if (cardBuffer.length() > 12) {
        cardBuffer = "";
      }
    }
  }
}

void processCard(String id) {
  Serial.print("Card ID: ");
  Serial.println(id);

  int studentIndex = -1;

  for (int i = 0; i < 3; i++) {
    if (id == cardIDs[i]) {
      studentIndex = i;
      break;
    }
  }

  lcd.clear();

  if (studentIndex >= 0) {
    lcd.setCursor(0, 0);
    lcd.print(names[studentIndex]);

    if (!present[studentIndex]) {
      present[studentIndex] = true;

      lcd.setCursor(0, 1);
      lcd.print("Attendance: YES");

      Serial.print(names[studentIndex]);
      Serial.println(" - PRESENT");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Already Present");

      Serial.print(names[studentIndex]);
      Serial.println(" - ALREADY PRESENT");
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Unknown Card");
    lcd.setCursor(0, 1);
    lcd.print("Not Registered");

    Serial.println("Unknown Card - Not Registered");
  }

  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SMART ATTENDANCE");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card");
}
