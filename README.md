# SINKSAVER - Compost Mixer System

Automated compost mixing system for San Sebastian Recoletos de Cavite SHS. Schedules mixing every 6 hours, monitors soil moisture, and shows real-time status on OLED. Features AUTO/MANUAL modes with safety stops for low moisture. Uses DS3231 RTC for scheduling and buzzer alerts for notifications.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Pin Configuration](#pin-configuration)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [System Modes](#system-modes)
- [Display Information](#display-information)
- [Serial Monitor](#serial-monitor)
- [Troubleshooting](#troubleshooting)
- [System Behavior](#system-behavior)
- [Safety Information](#safety-information)
- [License](#license)

## Overview

The SINKSAVER Compost Mixer System is an automated solution designed to maintain optimal compost conditions through scheduled mixing and continuous moisture monitoring. Developed for San Sebastian Recoletos de Cavite SHS students, this system ensures consistent compost quality while providing real-time feedback and safety features to prevent operation under unsafe conditions.

## Features

- Real-time Monitoring - Continuous moisture level tracking
- Automated Scheduling - Mixes every 6 hours for 2 minutes
- Dual-Mode Operation - AUTO with safety features or full MANUAL control
- OLED Display - Visual feedback for time, status, and moisture levels
- RTC Integration - Accurate timekeeping with DS3231 module
- Audio Alerts - Buzzer notifications for mode changes and warnings
- Serial Logging - Detailed status information for debugging
- Safety Features - Automatic stop when moisture too low (AUTO mode only)

## Hardware Requirements

### Components

| Component | Specification | Quantity |
|-----------|--------------|----------|
| Microcontroller | Arduino Uno or compatible | 1 |
| OLED Display | 1.3" SH1106, 128x64, I2C | 1 |
| RTC Module | DS3231 Real-Time Clock | 1 |
| Moisture Sensor | Analog soil moisture sensor | 1 |
| Relay Module | 5V single channel relay | 1 |
| Buzzer | Active or passive buzzer | 1 |
| Push Buttons | Momentary tactile switches | 2 |
| Motor/Actuator | For mixing mechanism | 1 |
| Power Supply | 5V/12V (depending on motor) | 1 |
| Jumper Wires | Male-to-male/female | As needed |

### Optional Components

- Weatherproof enclosure
- Heat sinks for relay/motor
- External power adapter
- Mounting brackets

## Pin Configuration

```
Arduino Uno Pinout:
┌─────────────────────────────┐
│                             │
│  D2  ──────  Relay          │  Motor control
│  D3  ──────  Mode Button    │  AUTO/MANUAL toggle
│  D4  ──────  Start/Stop     │  Control button
│  D6  ──────  Buzzer         │  Audio alerts
│  A0  ──────  Moisture       │  Analog sensor
│  A4  ──────  SDA (I2C)      │  OLED + RTC data
│  A5  ──────  SCL (I2C)      │  OLED + RTC clock
│  GND ──────  Common Ground  │  All components
│  5V  ──────  Power          │  Logic power
│                             │
└─────────────────────────────┘
```

### Pin Details

| Pin | Function | Description |
|-----|----------|-------------|
| D2 | RELAY_PIN | Controls mixing motor via relay |
| D3 | BUTTON_MODE | Toggle between AUTO and MANUAL modes |
| D4 | BUTTON_START_STOP | Start/stop mixing operation |
| D6 | BUZZER_PIN | Audio feedback for alerts |
| A0 | SOIL_MOISTURE_PIN | Analog moisture sensor input |
| A4 | SDA | I2C data line (OLED + RTC) |
| A5 | SCL | I2C clock line (OLED + RTC) |

### Wiring Diagram

```
Moisture Sensor:
  VCC  →  5V (Arduino)
  GND  →  GND (Arduino)
  AO   →  A0 (Arduino)

OLED Display (I2C):
  VCC  →  5V (Arduino)
  GND  →  GND (Arduino)
  SDA  →  A4 (Arduino)
  SCL  →  A5 (Arduino)

RTC Module (I2C):
  VCC  →  5V (Arduino)
  GND  →  GND (Arduino)
  SDA  →  A4 (Arduino)
  SCL  →  A5 (Arduino)

Relay Module:
  VCC  →  5V (Arduino)
  GND  →  GND (Arduino)
  IN   →  D2 (Arduino)
  COM  →  Motor power source
  NO   →  Motor positive

Buzzer:
  +    →  D6 (Arduino)
  -    →  GND (Arduino)

Buttons:
  Mode Button     → D3 to GND (with pullup)
  Start/Stop      → D4 to GND (with pullup)
```

## Installation

### 1. Arduino IDE Setup

1. Download and install [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.19 or later)
2. Launch Arduino IDE
3. Verify installation by checking `Help → About Arduino`

### 2. Install Required Libraries

Install via `Sketch → Include Library → Manage Libraries`:

- **U8g2** by oliver - OLED display driver
- **RTClib** by Adafruit - Real-time clock support
- **Wire** - I2C communication (included by default)

**Installation steps:**
1. Open Library Manager: `Sketch → Include Library → Manage Libraries`
2. Search for "U8g2"
3. Click "Install" on U8g2 by oliver
4. Search for "RTClib"
5. Click "Install" on RTClib by Adafruit
6. Close Library Manager

### 3. Hardware Assembly

1. **Connect I2C devices:**
   - Wire OLED and RTC to A4 (SDA) and A5 (SCL)
   - Both devices share the same I2C bus

2. **Connect sensors and outputs:**
   - Moisture sensor to A0
   - Relay to D2
   - Buzzer to D6
   - Buttons to D3 and D4

3. **Verify connections:**
   - Check all power (5V) and ground (GND) connections
   - Ensure no shorts between pins
   - Verify I2C pullup resistors (usually built into modules)

### 4. Set RTC Time

**Important:** Only do this once during initial setup

1. Open the sketch in Arduino IDE
2. Locate this line in `setup()`:
   ```cpp
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   ```

3. Uncomment by removing `//`:
   ```cpp
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   ```

4. Upload to Arduino (this sets current time)
5. Comment the line back out:
   ```cpp
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   ```

6. Upload again (prevents time reset on every boot)

### 5. Upload Sketch

1. Connect Arduino to computer via USB
2. Select board: `Tools → Board → Arduino Uno`
3. Select port: `Tools → Port → [Your COM/Serial Port]`
4. Click **Upload** button (→)
5. Wait for "Done uploading" message

### 6. Verify Installation

1. Open Serial Monitor: `Tools → Serial Monitor`
2. Set baud rate to **9600**
3. Press Arduino reset button
4. Verify startup messages appear:
   ```
   ========================================
      SINKSAVER COMPOST MIXER SYSTEM
   ========================================
   ✓ OLED initialized
   ✓ RTC initialized
   System Ready!
   ```

## Configuration

### Timing Settings

Edit these constants in the code to adjust operation:

```cpp
// Mixing schedule (default: 6 hours)
const unsigned long MIXING_INTERVAL = 6UL * 60 * 60 * 1000;

// Mixing duration (default: 2 minutes)
const unsigned long MIXING_DURATION = 2UL * 60 * 1000;
```

**Examples:**

```cpp
// Mix every 4 hours for 3 minutes
const unsigned long MIXING_INTERVAL = 4UL * 60 * 60 * 1000;
const unsigned long MIXING_DURATION = 3UL * 60 * 1000;

// Mix every 12 hours for 90 seconds
const unsigned long MIXING_INTERVAL = 12UL * 60 * 60 * 1000;
const unsigned long MIXING_DURATION = 90UL * 1000;
```

### Moisture Thresholds

Adjust moisture sensor limits based on your sensor calibration:

```cpp
// Moisture sensor range (analog values 0-1023)
const int MOISTURE_MIN = 300;  // Below this = too dry
const int MOISTURE_MAX = 700;  // Above this = too wet
```

**Calibration steps:**
1. Place sensor in dry compost, note reading
2. Place sensor in wet compost, note reading
3. Set MIN to 20% above dry reading
4. Set MAX to 80% of wet reading

## Usage

### First Time Setup

1. **Power on the system**
   - Connect Arduino via USB or external power
   - OLED displays "Starting..."
   - System runs self-check

2. **Boot sequence**
   ```
   ========================================
      SINKSAVER COMPOST MIXER SYSTEM
   ========================================
   
   Initializing I2C...
   Initializing OLED...
   ✓ OLED initialized
   Initializing RTC...
   ✓ RTC initialized
   
   Settings:
     Mixing Interval: 6 hours
     Mixing Duration: 2 minutes
     Moisture MIN: 300
     Moisture MAX: 700
   
   ========================================
   System Ready!
   Mode: AUTO
   ========================================
   ```

3. **System ready indicators**
   - Two short beeps (1000Hz, 1500Hz)
   - OLED shows current time and status
   - Serial Monitor displays system information
   - Mode set to AUTO by default

### Daily Operation

**Normal Monitoring (AUTO Mode):**
- System checks moisture every 100ms
- OLED displays current status
- Waits for scheduled mixing interval
- Shows countdown to next mix

**Automatic Mixing Cycle:**
1. 6 hours after last mix or system start
2. Buzzer beeps once (start signal)
3. Motor turns on for 2 minutes
4. System monitors moisture during mixing
5. Stops if moisture drops below minimum
6. Buzzer beeps once when complete

**Manual Override:**
1. Press Mode button to switch to MANUAL
2. Two-tone beep confirms mode change
3. Press Start/Stop to control motor
4. No automatic timing or safety stops
5. Press Mode button to return to AUTO

### Button Operations

**Mode Button (D3):**
- **Short press:** Toggle AUTO ↔ MANUAL
- **Feedback:** Two beeps (high for AUTO, low for MANUAL)
- **Display:** Mode indicator updates on OLED

**Start/Stop Button (D4):**
- **In AUTO mode:** 
  - Start: Begin mixing cycle immediately
  - Stop: Stop current cycle, reset timer
- **In MANUAL mode:**
  - Toggle: Turn motor ON/OFF
  - No time limits
- **Feedback:** Single beep on action

## System Modes

### AUTO Mode

**Operation:**
- Automatic mixing every 6 hours
- 2-minute mixing duration
- Moisture safety enabled
- Scheduled operation

**Safety Features:**
- Stops if moisture < MOISTURE_MIN
- Alarm sounds on safety stop
- Cannot restart until moisture increases
- Timer resets after safety stop

**Display Shows:**
- Time until next mix
- Current moisture level
- Moisture status (OPTIMAL/TOO DRY/TOO WET)
- Time remaining during mix

**LED Indicator:** `[AUTO]` on OLED

### MANUAL Mode

**Operation:**
- User controls all mixing
- No automatic scheduling
- No time limits
- Direct motor control

**Safety Features:**
- Moisture safety DISABLED
- No automatic stops
- User responsible for monitoring
- Warning displayed on OLED

**Display Shows:**
- "MANUAL CTRL" message
- Current moisture level (informational only)
- Motor status (ON/OFF)

**LED Indicator:** `[MAN]` on OLED

### Mode Transition

**AUTO → MANUAL:**
1. Press Mode button
2. System stops automatic scheduling
3. Buzzer: 1000Hz → 800Hz beep
4. If motor running, it continues
5. User gains full control

**MANUAL → AUTO:**
1. Press Mode button
2. System resumes automatic operation
3. Buzzer: 1500Hz → 1800Hz beep
4. Timer resets to 6-hour interval
5. Safety features re-enabled

## Display Information

### OLED Layout

```
┌────────────────────────┐
│ 12:34:56      [AUTO]   │  Time + Mode
│ Motor: ON              │  Motor status
│ Moist: 450             │  Moisture value
│ ████████░░░░░░         │  Moisture bar
│ OPTIMAL                │  Status message
│ Nxt:5h30m              │  Next mix timer
└────────────────────────┘
```

### Status Messages

| Message | Meaning | Action Required |
|---------|---------|-----------------|
| OPTIMAL | Moisture in range | None |
| TOO DRY | Below minimum | Add water (AUTO stops) |
| TOO WET | Above maximum | Allow drying |
| MANUAL CTRL | Manual mode active | User controls system |

### Timer Display

**AUTO Mode - Idle:**
- `Nxt:5h30m` - Next mix in 5 hours 30 minutes

**AUTO Mode - Mixing:**
- `Left:90s` - 90 seconds remaining in cycle

**MANUAL Mode:**
- `MANUAL CTRL` - No timer shown

## Serial Monitor

### Status Output

System prints status every 5 seconds at 9600 baud:

```
[12:34:56] Mode: AUTO | Motor: OFF | Moisture: 450 (OPTIMAL) | Next mix in: 5h 30m
[12:35:01] Mode: AUTO | Motor: OFF | Moisture: 448 (OPTIMAL) | Next mix in: 5h 29m
```

### Event Notifications

**Mixing Started:**
```
▶▶▶ MIXING STARTED
    Mode: AUTO
    Start time: 12:00:00
    Duration: 2 minutes
```

**Mixing Stopped:**
```
■■■ MIXING STOPPED
    Stop time: 12:02:00
```

**Mode Changed:**
```
========================================
>>> MODE CHANGED: MANUAL
    Manual control via button
    Moisture safety: DISABLED
========================================
```

**Moisture Alarm:**
```
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!! ALARM: MOISTURE TOO DRY !!!
!!! AUTO MODE SAFETY STOP       !!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
```

### System Information

On startup, Serial Monitor displays:

```
========================================
    SINKSAVER COMPOST MIXER SYSTEM
========================================

Initializing I2C...
Initializing OLED...
✓ OLED initialized
Initializing RTC...
✓ RTC initialized

Pin Configuration:
  Pin 2: Relay (Motor)
  Pin 3: Button - Mode Toggle (AUTO/MANUAL)
  Pin 4: Button - Start/Stop Mixing
  Pin 6: Buzzer
  A0:    Soil Moisture Sensor
  A4:    I2C SDA (OLED + RTC)
  A5:    I2C SCL (OLED + RTC)

Settings:
  Mixing Interval: 6 hours
  Mixing Duration: 2 minutes
  Moisture MIN: 300
  Moisture MAX: 700

Safety Features:
  AUTO mode: Moisture safety enabled
  MANUAL mode: Moisture safety DISABLED

========================================
System Ready!
Mode: AUTO
========================================
```

## Troubleshooting

### OLED Display Issues

**Problem:** OLED shows nothing or displays incorrectly

**Solutions:**
- Verify I2C connections (A4=SDA, A5=SCL)
- Check display power (5V and GND)
- Confirm display is SH1106 (not SSD1306)
- Try different I2C address if needed
- Test with I2C scanner sketch

**Code fix for SSD1306:**
```cpp
// Change this line:
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// To this:
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
```

### RTC Errors

**Problem:** "RTC FAILED!" message, continuous beeping

**Solutions:**
- Check RTC connections to A4/A5
- Verify RTC module has backup battery
- Ensure battery is not dead
- Check solder joints on RTC module
- Try different RTC module
- Verify I2C address (usually 0x68)

**Reset time:**
```cpp
// Uncomment, upload, then comment out again
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
```

### Moisture Sensor Issues

**Problem:** False readings or no response

**Solutions:**
- Clean sensor probes with soft cloth
- Remove corrosion with fine sandpaper
- Calibrate sensor in dry and wet conditions
- Check analog connection to A0
- Verify sensor power (5V)
- Replace sensor if damaged

**Calibration:**
1. Place in completely dry compost
2. Note Serial Monitor reading
3. Place in wet compost
4. Note reading
5. Adjust MOISTURE_MIN and MOISTURE_MAX

### Motor Not Running

**Problem:** Relay clicks but motor doesn't turn

**Solutions:**
- Check relay connections (COM, NO)
- Verify motor power supply voltage
- Test motor with direct power
- Check relay contacts with multimeter
- Ensure relay rated for motor current
- Replace relay if contacts burned

**Problem:** No relay click at all

**Solutions:**
- Verify D2 connection to relay IN
- Check relay power (5V, GND)
- Test relay LED (should light when active)
- Measure D2 voltage (should be 5V when ON)
- Replace relay module

### Button Problems

**Problem:** Buttons don't respond

**Solutions:**
- Check button connections to D3/D4
- Verify buttons connect pin to GND
- Test buttons with multimeter
- Clean button contacts
- Check for loose wiring
- Replace buttons if damaged

**Problem:** Multiple triggers on single press

**Solutions:**
- Already has 50ms debounce in code
- Add 0.1µF capacitor across button
- Use better quality switches
- Check for electrical noise

### Serial Monitor Issues

**Problem:** No output or garbled text

**Solutions:**
- Set baud rate to 9600
- Select correct COM port
- Press Arduino reset button
- Check USB cable quality
- Install CH340/CP2102 drivers
- Try different USB port

## System Behavior

### Timing Diagram

```
Time:     0h    1h    2h    3h    4h    5h    6h    6h2m  12h
          │     │     │     │     │     │     │     │     │
Mode:     ├─────────────────AUTO──────────────┤     ├─────►
          │                                   │     │
Motor:    ───────────────────────────────────▐█████▐──────►
          │                                   │ 2min│
Event:    Start                             Mix    End   Next
```

### State Machine

```
┌──────────────┐
│   STARTUP    │
│  Initialize  │
└──────┬───────┘
       │
       ▼
┌──────────────┐
│   READY      │ ◄──────────────┐
│  AUTO/MANUAL │                │
└──────┬───────┘                │
       │                        │
       │ Timer expired          │
       │ or button press        │
       ▼                        │
┌──────────────┐                │
│   MIXING     │                │
│  Motor ON    │                │
└──────┬───────┘                │
       │                        │
       │ Duration complete      │
       │ or moisture too low    │
       │ or button stop         │
       │                        │
       └────────────────────────┘
```

### Sensor Logic

```
Moisture Reading:
  0-299:   TOO DRY  (Emergency stop in AUTO)
  300-700: OPTIMAL  (Normal operation)
  701-1023: TOO WET (Warning only)

AUTO Mode Response:
  TOO DRY  → Stop motor, sound alarm
  OPTIMAL  → Allow operation
  TOO WET  → Allow operation, show warning

MANUAL Mode Response:
  (Any)    → Informational display only
```

## Safety Information

### Electrical Safety

- Use proper power supply voltage
- Ensure motor current within relay rating
- Protect electronics from moisture
- Use insulated enclosure
- Ground system if using AC motor
- Never modify while powered on

### Installation Guidelines

- Mount controller above compost bin
- Keep electronics dry and ventilated
- Secure all wiring away from moving parts
- Use strain relief on cables
- Protect sensor from direct water spray
- Ensure emergency shutoff accessible

### Operational Safety

**AUTO Mode:**
- System prevents operation when too dry
- Automatic safety stop protects compost
- Alarm alerts user to condition
- Resume only when moisture adequate

**MANUAL Mode:**
- User responsible for all safety
- No automatic protection
- Monitor moisture closely
- Use only when necessary

### Maintenance Schedule

**Weekly:**
- Check moisture readings
- Verify motor operation
- Clean sensor if needed
- Test buzzer functionality

**Monthly:**
- Inspect all connections
- Check relay contacts
- Verify time accuracy
- Test both modes

**Quarterly:**
- Deep clean sensor probes
- Check battery backup (RTC)
- Inspect wiring integrity
- Calibrate moisture sensor

**Annually:**
- Replace moisture sensor (preventive)
- Service motor/actuator
- Update firmware if available
- Full system test

## Notes

- Internal pull-up resistors used for buttons (no external resistors needed)
- I2C devices can share same bus (parallel connection)
- RTC maintains time even without power (battery backup)
- OLED using page buffer mode (memory efficient)
- All timestamps handled by RTC hardware
- Buzzer frequency: 800Hz-2000Hz range
- System uses F() macro for PROGMEM strings (saves RAM)

## Support

For issues not covered in troubleshooting:

1. Check Serial Monitor for error messages
2. Verify all hardware connections match diagram
3. Test individual components separately
4. Ensure all libraries installed correctly
5. Review code comments for additional details
6. Check component datasheets for specifications

## License

This project is provided as-is for educational purposes.

---

**Project:** Compost Mixer System  
**Institution:** San Sebastian Recoletos de Cavite SHS  
**Platform:** Arduino Uno  
**Version:** 1.0  
**Last Updated:** January 2026