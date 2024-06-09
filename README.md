# Библиотека DS1302 RTC (Real Time Clock) для Arduino



Это библиотека DS1302 RTC (Real Time Clock) с интерфейсом на 3 провода для Arduino.


## Особенности библиотеки

* Совместима с библиотекой libc `<time.h>`
* Чтение/запись даты/времени в структуру `struct tm`
* Установка/получение Unix эпохи UTC `time_t`
* Установка/получение времени (часы, минуты, секунды)
* Установка/получение даты и времени (час, мин, сек, число, месяц, год, день недели)
* Чтение/запись 31 байта батарейно-запоминаемой RTC памяти.
* Программируемое зарядное устройство для батареек/конденсаторов.
* Оптимизированный интерфейс для взаимодействия с Atmel AVR платформой.

## Спецификации DS1302

**ВАЖНЫЕ ЗАМЕЧАНИЯ:** 

* Время DS1302 RTC может отклоняться на более чем 1 минуту в день, поэтому этот девайс не рекомендуется для использования в проектах с высокими требованиями к точности. 
* Для новых проектов рекомендуется использовать [высокоточные DS3231 I2C RTC](https://github.com/Alash-electronics/Alash_DS1302).
* Интерфейс на 3 провода **НЕ** совместим с SPI.

## Примеры

Arduino IDE | Файл | Примеры | Alash-electronics DS1302 RTC:

* [Alarm](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302Alarm/Alash_DS1302Alarm.ino): Программирование одного или нескольких программных будильников
* [RAM](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302RAM/Alash_DS1302RAM.ino): Чтение/запись RTC RAM.
* [SetGetDateTime](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302SetGetDateTime/Alash_DS1302SetGetDateTime.ino): Установка/получение даты и времени
* [Terminal](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302Terminal/Alash_DS1302Terminal.ino) и [скрипт Python](https://github.com/Alash-electronics/Alash_DS1302/blob/master/examples/Alash_DS1302Terminal/Alash_DS1302Terminal.py) для установки даты и времени

## Использование

**Инициализация**

```c++
#include <Alash_DS1302.h>

/*
DS1302 IC      Atmel AVR             ESP8266       ESP32
GND            GND                     GND          GND
VCC2           5V(или 3.3V)            3V3          3V3
SCLK (CLK)     2(цифровой пин)         D4           0
I/O (DAT)      3(цифровой пин)         D2           5
CE (RST)       4(цифровой пин)         D2           4
*/

// Подключение пина данных DS1302 к цифровому пину Arduino
#define DS1302_CLK_PIN      2
#define DS1302_IO_PIN       3
#define DS1302_CE_PIN       4

// Создание объекта RTC
Alash_DS1302 rtc = Alash_DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

void setup()
{
    // Инициализация RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }
}
```

**Проверка состояния осциллятора при старте**

```c++
// Проверка состояния осциллятора
if (!rtc.isRunning()) {
    // Ошибка: Остановлен осциллятор RTC. Дата/время не может быть доверенным. 
    // Необходимо установить новую дату/время перед чтением.
    
    // Включение осциллятора
    rtc.clockEnable(true);
}
```

**Установка времени**

```c++
// Запись времени в RTC
if (!rtc.setTime(12, 0, 0)) {
    // Ошибка: Не удалось установить время
}
```

**Получение времени**

```c++
uint8_t hour;
uint8_t minute;
uint8_t second;

// Чтение времени из RTC
if (!rtc.getTime(&hour, &minute, &second)) {
    // Ошибка: Не удалось прочитать время RTC
}
```

**Установка даты и времени**

```c++
// Запись даты/времени в RTC: 13:45:09  31 декабря 2019  0=Воскресенье, 2=Вторник
if (!rtc.setDateTime(13, 45, 9,  31, 12, 2019,  2) {
    // Ошибка: Не удалось записать дату/время
}
```

**Получение даты и времени**

```c++
uint8_t hour;
uint8_t min;
uint8_t sec;
uint8_t mday;
uint8_t mon;
uint16_t year;
uint8_t wday;

// Чтение даты/времени из RTC
if (!rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday) {
    // Ошибка: Не удалось прочитать дату/время RTC
}

// hour: 0..23
// min: 0..59
// sec: 0..59
// mday: 1..31
// mon: 1..12
// year: 2000..2099
// wday: 0..6 (0=Воскресенье .. 6=Суббота)
```

**Запись/чтение структуры времени struct tm**

```c++
struct tm dt;

dt.tm_hour = 12;
dt.tm_min = 34;
dt.tm_sec = 56;
dt.tm_mday = 29;
dt.tm_mon = 1; // 0=Январь
dt.tm_year = 2024-1900;
dt.tm_wday = 6; // 0=Воскресенье

if (!rtc.write(&dt)) {
    // Ошибка: Не удалось записать RTC
}
```

```c++
struct tm dt;

// Чтение даты/времени из RTC
if (!rtc.read(&dt)) {
    // Ошибка: Не удалось прочитать дату/время RTC
}
```

**Чтение Unix эпохи UTC**

```c++
time_t t;

// Чтение Unix эпохи UTC из RTC
if (!rtc.getEpoch(&t)) {
    // Ошибка: Не удалось прочитать UTC эпоху RTC
}
```

**Запись Unix эпохи UTC**

```c++
// Запись Unix эпохи UTC в RTC
if (!rtc.setEpoch(1599416430UL)) {
    // Ошибка: Не удалось установить эпоху
}
```

**Запись в RTC RAM**

```c++
// Запись байта в RTC RAM
rtc.writeByteRAM(0x02, 0xA9);

// Запись буфера в RTC RAM
uint8_t buf[NUM_DS1302_RAM_REGS] = { 0x00 };
rtc.writeBufferRAM(buf, sizeof(buf));
```

**Чтение из RTC RAM**

```c++
// Чтение байта из RTC RAM
uint8_t dataByte = rtc.read

ByteRAM(0x02);

// Чтение буфера из RTC RAM
uint8_t buf[NUM_DS1302_RAM_REGS];
rtc.readBufferRAM(buf, sizeof(buf));
```

**Установка зарядного устройства**

Пожалуйста, обратитесь к даташиту, чтобы узнать, как настроить зарядное устройство.

```c++
// Отключение (по умолчанию)
rtc.writeRegister(DS1302_REG_TC, DS1302_TCS_DISABLE);

// Минимум 2 диода, 8кОм
rtc.writeRegister(DS1302_REG_TC, 0xAB);

// Максимум 1 диод, 2кОм
rtc.writeRegister(DS1302_REG_TC, 0xA5);
```

**Установка даты и времени RTC с использованием Python**


Запустите Python скрипт:

```c++
// Установите Pyserial
python3 pip -m pyserial

// Установка даты и времени RTC
python3 Terminal.py
```

## Конфигурация пинов

**Примечание:** Пин D4 на ESP8266 высокий во время перезагрузки/сброса/программирования, что может повредить регистры RTC. По этой причине пины D2 и D4 поменяны местами.

| Чип DS1302 |    Atmel AVR    | ESP8266 | ESP32 |
| :--------: | :-------------: | :-----: | :---: |
|    GND     |       GND       |   GND   |  GND  |
|    VCC2    |  5V (или 3.3V)  |   3V3   |  3V3  |
| SCLK (CLK) | 2 (Цифровой пин)|   D4    |   0   |
| I/O (DAT)  | 3 (Цифровой пин)|   D2    |   5   |
|  CE (RST)  | 4 (Цифровой пин)|   D2    |   4   |


## Зависимости библиотеки

* `Wire.h`
* `Terminal.ino` требует библиотеку `AlashSerialTerminal`.

## Другие библиотеки для Arduino от Alash-electronics

* [Библиотеки Alash-electronics](https://github.com/Alash-electronics)