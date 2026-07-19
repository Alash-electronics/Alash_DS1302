# Alash_DS1302 — DS1302 RTC library for Arduino

> 🌐 **English** | [Русский](README_ru.md)

Arduino library for the DS1302 real-time clock chip (3-wire interface).

![wiring](https://github.com/Alash-electronics/Alash_DS1302/assets/171731850/db6a7647-add4-40d0-a586-eb4a393d6035)

## Features

* Compatible with the libc `<time.h>` API
* Read/write date and time as a `struct tm`
* Get/set Unix UTC epoch (`time_t`)
* Get/set time (hour, minute, second) or full date+time (+ day of week)
* Read/write the 31-byte battery-backed RTC RAM
* Programmable trickle charger for backup battery/supercap
* Optimized for Atmel AVR

## Important notes

* The DS1302 can drift by more than a minute a day — not suitable for projects that need high timekeeping accuracy. For a new design, consider a [DS3231 I2C RTC](https://github.com/Alash-electronics/Alash_DS1302) instead.
* The 3-wire interface is **not** SPI-compatible.

## Wiring

**Note:** pin D4 on the ESP8266 is driven high during reset/boot/flashing, which can corrupt the RTC registers — that's why D2/D4 are swapped below.

| DS1302 pin | Atmel AVR | ESP8266 | ESP32 |
|:---:|:---:|:---:|:---:|
| GND | GND | GND | GND |
| VCC2 | 5V (or 3.3V) | 3V3 | 3V3 |
| SCLK (CLK) | 2 (digital pin) | D4 | 0 |
| I/O (DAT) | 3 (digital pin) | D2 | 5 |
| CE (RST) | 4 (digital pin) | D2 | 4 |

## Usage

**Initialization**

```cpp
#include <Alash_DS1302.h>

#define DS1302_CLK_PIN 2
#define DS1302_IO_PIN  3
#define DS1302_CE_PIN  4

Alash_DS1302 rtc = Alash_DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

void setup() {
  while (!rtc.begin()) {
    Serial.println(F("RTC not found"));
    delay(3000);
  }
}
```

**Check the oscillator on startup**

```cpp
if (!rtc.isRunning()) {
  // Oscillator was stopped — date/time cannot be trusted until reset
  rtc.clockEnable(true);
}
```

**Set / get time**

```cpp
if (!rtc.setTime(12, 0, 0)) {
  // failed to set time
}

uint8_t hour, minute, second;
if (!rtc.getTime(&hour, &minute, &second)) {
  // failed to read time
}
```

**Set / get date and time**

```cpp
// 13:45:09, 31 Dec 2019, weekday 2 = Tuesday (0 = Sunday)
if (!rtc.setDateTime(13, 45, 9, 31, 12, 2019, 2)) {
  // failed to write date/time
}

uint8_t hour, min, sec, mday, mon, wday;
uint16_t year;
if (!rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday)) {
  // failed to read date/time
}
// hour: 0..23, min: 0..59, sec: 0..59
// mday: 1..31, mon: 1..12, year: 2000..2099
// wday: 0..6 (0 = Sunday .. 6 = Saturday)
```

**Read/write `struct tm`**

```cpp
struct tm dt;
dt.tm_hour = 12; dt.tm_min = 34; dt.tm_sec = 56;
dt.tm_mday = 29; dt.tm_mon = 1;      // 0 = January
dt.tm_year = 2024 - 1900;
dt.tm_wday = 6;                     // 0 = Sunday

if (!rtc.write(&dt)) { /* failed to write RTC */ }

struct tm dt2;
if (!rtc.read(&dt2)) { /* failed to read RTC */ }
```

**Unix epoch**

```cpp
time_t t;
if (!rtc.getEpoch(&t)) { /* failed to read epoch */ }
if (!rtc.setEpoch(1599416430UL)) { /* failed to set epoch */ }
```

**RTC RAM**

```cpp
rtc.writeByteRAM(0x02, 0xA9);
uint8_t buf[NUM_DS1302_RAM_REGS] = { 0x00 };
rtc.writeBufferRAM(buf, sizeof(buf));

uint8_t dataByte = rtc.readByteRAM(0x02);
uint8_t readBuf[NUM_DS1302_RAM_REGS];
rtc.readBufferRAM(readBuf, sizeof(readBuf));
```

**Trickle charger**

Consult the datasheet before enabling the trickle charger.

```cpp
rtc.writeRegister(DS1302_REG_TC, DS1302_TCS_DISABLE); // off (default)
rtc.writeRegister(DS1302_REG_TC, 0xAB); // min: 2 diodes, 8kΩ
rtc.writeRegister(DS1302_REG_TC, 0xA5); // max: 1 diode, 2kΩ
```

**Setting date/time from a PC via Python**

```bash
pip3 install pyserial
python3 Terminal.py
```

## Examples

* [Alarm](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302Alarm/Alash_DS1302Alarm.ino) — one or more software alarms
* [RAM](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302RAM/Alash_DS1302RAM.ino) — read/write RTC RAM
* [SetGetDateTime](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302SetGetDateTime/Alash_DS1302SetGetDateTime.ino) — set/read date and time
* [Terminal](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302Terminal/Alash_DS1302Terminal.ino) + [Python script](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302Terminal/Alash_DS1302Terminal.py) to set the clock from a computer

## Dependencies

* `Wire.h`
* The `Terminal` example requires the [AlashSerialTerminal](https://github.com/Alash-electronics/AlashSerialTerminal) library.

## Other Arduino libraries from Alash-electronics

[github.com/Alash-electronics](https://github.com/Alash-electronics)
