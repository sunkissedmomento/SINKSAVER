#include <Wire.h>
#include <U8g2lib.h>
#include <RTClib.h>

// Pin definitions
#define RELAY_PIN 2
#define BUTTON_MODE 3
#define BUTTON_START_STOP 4
#define BUZZER_PIN 6
#define SOIL_MOISTURE_PIN A0

// FIXED: Use SH1106 for 1.3" OLED (fixes offset)
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

RTC_DS3231 rtc;

// Timing settings
const unsigned long MIXING_INTERVAL = 6UL * 60 * 60 * 1000;
const unsigned long MIXING_DURATION = 2UL * 60 * 1000;
const int MOISTURE_MIN = 300;
const int MOISTURE_MAX = 700;

// State variables
bool isAutoMode = true;
bool isRelayOn = false;
unsigned long lastMixTime = 0;
unsigned long mixStartTime = 0;

// Button states
bool lastButton1 = HIGH;
bool lastButton2 = HIGH;

void setup() {
  Serial.begin(9600);
  
  Serial.println(F("========================================"));
  Serial.println(F("    COMPOST MIXER SYSTEM"));
  Serial.println(F("========================================"));
  Serial.println();
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_START_STOP, INPUT_PULLUP);
  digitalWrite(RELAY_PIN, LOW);
  
  Serial.println(F("Initializing I2C..."));
  Wire.begin();
  
  Serial.println(F("Initializing OLED..."));
  u8g2.begin();
  showMessage(F("Starting..."));
  Serial.println(F("✓ OLED initialized"));
  
  Serial.println(F("Initializing RTC..."));
  if (!rtc.begin()) {
    Serial.println(F("✗ RTC FAILED!"));
    showMessage(F("RTC ERROR!"));
    while (1) {
      tone(BUZZER_PIN, 1000, 200);
      delay(1000);
    }
  }
  Serial.println(F("✓ RTC initialized"));
  
  // Uncomment to set time
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  Serial.println();
  Serial.println(F("Pin Configuration:"));
  Serial.println(F("  Pin 2: Relay (Motor)"));
  Serial.println(F("  Pin 3: Button - Mode Toggle (AUTO/MANUAL)"));
  Serial.println(F("  Pin 4: Button - Start/Stop Mixing"));
  Serial.println(F("  Pin 6: Buzzer"));
  Serial.println(F("  A0:    Soil Moisture Sensor"));
  Serial.println(F("  A4:    I2C SDA (OLED + RTC)"));
  Serial.println(F("  A5:    I2C SCL (OLED + RTC)"));
  Serial.println();
  
  Serial.println(F("Settings:"));
  Serial.print(F("  Mixing Interval: "));
  Serial.print(MIXING_INTERVAL / 3600000);
  Serial.println(F(" hours"));
  Serial.print(F("  Mixing Duration: "));
  Serial.print(MIXING_DURATION / 60000);
  Serial.println(F(" minutes"));
  Serial.print(F("  Moisture MIN: "));
  Serial.println(MOISTURE_MIN);
  Serial.print(F("  Moisture MAX: "));
  Serial.println(MOISTURE_MAX);
  Serial.println();
  
  Serial.println(F("Safety Features:"));
  Serial.println(F("  AUTO mode: Moisture safety enabled"));
  Serial.println(F("  MANUAL mode: Moisture safety DISABLED"));
  Serial.println();
  
  tone(BUZZER_PIN, 1000, 100);
  delay(150);
  tone(BUZZER_PIN, 1500, 100);
  
  lastMixTime = millis();
  
  Serial.println(F("========================================"));
  Serial.println(F("System Ready!"));
  Serial.println(F("Mode: AUTO"));
  Serial.println(F("========================================"));
  Serial.println();
  
  delay(1000);
}

void loop() {
  DateTime now = rtc.now();
  int moisture = analogRead(SOIL_MOISTURE_PIN);
  unsigned long currentTime = millis();
  
  // Check buttons
  if (checkButton(BUTTON_MODE, lastButton1)) {
    toggleMode();
  }
  
  if (checkButton(BUTTON_START_STOP, lastButton2)) {
    toggleMixing();
  }
  
  // Auto mode logic - WITH moisture safety
  if (isAutoMode) {
    if (currentTime - lastMixTime >= MIXING_INTERVAL && !isRelayOn) {
      startMixing();
    }
    
    if (isRelayOn) {
      if (currentTime - mixStartTime >= MIXING_DURATION) {
        stopMixing();
        lastMixTime = currentTime;
      } else if (moisture < MOISTURE_MIN) {
        // AUTO mode: Stop if too dry
        stopMixing();
        alarm();
        lastMixTime = currentTime;
      }
    }
  }
  // MANUAL mode: No moisture checking - user has full control
  
  updateDisplay(now, moisture, currentTime);
  
  // Print status every 5 seconds
  static unsigned long lastPrint = 0;
  if (currentTime - lastPrint >= 5000) {
    printStatus(now, moisture, currentTime);
    lastPrint = currentTime;
  }
  
  delay(100);
}

bool checkButton(int pin, bool &lastState) {
  bool current = digitalRead(pin);
  bool pressed = false;
  
  if (current == LOW && lastState == HIGH) {
    delay(50); // Debounce
    if (digitalRead(pin) == LOW) {
      pressed = true;
    }
  }
  lastState = current;
  return pressed;
}

void toggleMode() {
  isAutoMode = !isAutoMode;
  
  Serial.println();
  Serial.println(F("========================================"));
  if (isAutoMode) {
    Serial.println(F(">>> MODE CHANGED: AUTO"));
    Serial.println(F("    Automatic mixing every 6 hours"));
    Serial.println(F("    Moisture safety: ENABLED"));
    tone(BUZZER_PIN, 1500, 100);
    delay(100);
    tone(BUZZER_PIN, 1800, 100);
    if (isRelayOn) lastMixTime = millis();
  } else {
    Serial.println(F(">>> MODE CHANGED: MANUAL"));
    Serial.println(F("    Manual control via button"));
    Serial.println(F("    Moisture safety: DISABLED"));
    tone(BUZZER_PIN, 1000, 100);
    delay(100);
    tone(BUZZER_PIN, 800, 100);
  }
  Serial.println(F("========================================"));
  Serial.println();
}

void toggleMixing() {
  if (isRelayOn) {
    stopMixing();
    if (isAutoMode) lastMixTime = millis();
  } else {
    startMixing();
  }
}

void startMixing() {
  isRelayOn = true;
  mixStartTime = millis();
  digitalWrite(RELAY_PIN, HIGH);
  tone(BUZZER_PIN, 1200, 200);
  
  Serial.println();
  Serial.println(F("▶▶▶ MIXING STARTED"));
  Serial.print(F("    Mode: "));
  Serial.println(isAutoMode ? F("AUTO") : F("MANUAL"));
  Serial.print(F("    Start time: "));
  DateTime now = rtc.now();
  Serial.print(now.hour());
  Serial.print(F(":"));
  Serial.print(now.minute());
  Serial.print(F(":"));
  Serial.println(now.second());
  
  if (isAutoMode) {
    Serial.print(F("    Duration: "));
    Serial.print(MIXING_DURATION / 60000);
    Serial.println(F(" minutes"));
  } else {
    Serial.println(F("    Duration: Until manually stopped"));
  }
  Serial.println();
}

void stopMixing() {
  isRelayOn = false;
  digitalWrite(RELAY_PIN, LOW);
  tone(BUZZER_PIN, 800, 200);
  
  Serial.println();
  Serial.println(F("■■■ MIXING STOPPED"));
  Serial.print(F("    Stop time: "));
  DateTime now = rtc.now();
  Serial.print(now.hour());
  Serial.print(F(":"));
  Serial.print(now.minute());
  Serial.print(F(":"));
  Serial.println(now.second());
  Serial.println();
}

void alarm() {
  Serial.println();
  Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
  Serial.println(F("!!! ALARM: MOISTURE TOO DRY !!!"));
  Serial.println(F("!!! AUTO MODE SAFETY STOP       !!!"));
  Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
  Serial.println();
  
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 2000, 100);
    delay(200);
  }
}

void showMessage(const __FlashStringHelper* msg) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_9x15B_tr);
    u8g2.drawStr(15, 30, (const char*)msg);
  } while (u8g2.nextPage());
}

void printStatus(DateTime now, int moisture, unsigned long currentTime) {
  Serial.print(F("["));
  if (now.hour() < 10) Serial.print(F("0"));
  Serial.print(now.hour());
  Serial.print(F(":"));
  if (now.minute() < 10) Serial.print(F("0"));
  Serial.print(now.minute());
  Serial.print(F(":"));
  if (now.second() < 10) Serial.print(F("0"));
  Serial.print(now.second());
  Serial.print(F("] "));
  
  Serial.print(F("Mode: "));
  Serial.print(isAutoMode ? F("AUTO ") : F("MANUAL "));
  
  Serial.print(F("| Motor: "));
  Serial.print(isRelayOn ? F("ON  ") : F("OFF "));
  
  Serial.print(F("| Moisture: "));
  Serial.print(moisture);
  Serial.print(F(" ("));
  
  if (moisture < MOISTURE_MIN) {
    Serial.print(F("TOO DRY"));
    if (isAutoMode) {
      Serial.print(F(" - ALARM!"));
    }
  } else if (moisture > MOISTURE_MAX) {
    Serial.print(F("TOO WET"));
  } else {
    Serial.print(F("OPTIMAL"));
  }
  Serial.print(F(")"));
  
  if (isAutoMode && !isRelayOn) {
    unsigned long left = MIXING_INTERVAL - (currentTime - lastMixTime);
    Serial.print(F(" | Next mix in: "));
    Serial.print(left / 3600000);
    Serial.print(F("h "));
    Serial.print((left % 3600000) / 60000);
    Serial.print(F("m"));
  } else if (isRelayOn && isAutoMode) {
    unsigned long left = MIXING_DURATION - (currentTime - mixStartTime);
    Serial.print(F(" | Time left: "));
    Serial.print(left / 1000);
    Serial.print(F("s"));
  }
  
  Serial.println();
}

void updateDisplay(DateTime now, int moisture, unsigned long currentTime) {
  char buf[20];
  
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    
    // Time
    sprintf(buf, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    u8g2.drawStr(0, 10, buf);
    
    // Mode
    u8g2.drawStr(70, 10, isAutoMode ? "[AUTO]" : "[MAN]");
    
    // Motor status
    u8g2.drawStr(0, 22, "Motor:");
    if (isRelayOn) {
      u8g2.setFont(u8g2_font_9x15B_tr);
      u8g2.drawStr(45, 24, "ON");
      u8g2.setFont(u8g2_font_6x10_tr);
    } else {
      u8g2.drawStr(45, 22, "OFF");
    }
    
    // Moisture
    sprintf(buf, "Moist: %d", moisture);
    u8g2.drawStr(0, 34, buf);
    
    // Bar
    int barW = map(moisture, 0, 1023, 0, 120);
    u8g2.drawFrame(0, 38, 122, 8);
    u8g2.drawBox(1, 39, constrain(barW, 0, 120), 6);
    
    // Status - Show warning ONLY in AUTO mode
    if (isAutoMode && moisture < MOISTURE_MIN) {
      u8g2.drawStr(0, 56, "TOO DRY!");
    } else if (moisture > MOISTURE_MAX) {
      u8g2.drawStr(0, 56, "TOO WET");
    } else {
      u8g2.drawStr(0, 56, "OPTIMAL");
    }
    
    // Timer
    if (isAutoMode && !isRelayOn) {
      unsigned long left = MIXING_INTERVAL - (currentTime - lastMixTime);
      sprintf(buf, "Nxt:%luh%lum", left/3600000, (left%3600000)/60000);
      u8g2.drawStr(0, 64, buf);
    } else if (isRelayOn && isAutoMode) {
      unsigned long left = MIXING_DURATION - (currentTime - mixStartTime);
      sprintf(buf, "Left:%lus", left/1000);
      u8g2.drawStr(0, 64, buf);
    } else if (!isAutoMode) {
      u8g2.drawStr(0, 64, "MANUAL CTRL");
    }
    
  } while (u8g2.nextPage());
}