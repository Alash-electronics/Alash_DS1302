/*!
 *    Пример использования будильника DS1302 RTC для Arduino
 *    Источник:         https://github.com/Alash-electronics/Alash_DS1302
 */

#include <Alash_DS1302.h>
#include "Alash_DS1302_Alarm.h"

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

// Создание объекта RTC DS1302
Alash_DS1302 rtc = Alash_DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);


// Обработчик срабатывания будильника
void alarmOn(){
    Serial.println("Будильник ВКЛ");
    //Добавьте здесь свои функции
}

// Обработчик выключения будильника
void alarmOff(){
    Serial.println("Будильник ВЫКЛ");
    //Добавьте здесь свои функции
}

// Определение хотя бы одного будильника (час, минута, секунда, обработчик)
Alarm alarms[] = {
    Alarm(12, 0, 5, &alarmOn),
    Alarm(12, 0, 15, &alarmOff),
    Alarm(12, 0, 30, &alarmOn),
    Alarm(12, 1, 0, &alarmOff)
};

// Вывод времени (часы, минуты, секунды)
void printTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    char buf[10];

    // Вывод времени
    snprintf(buf, sizeof(buf), "%d:%02d:%02d", hour, minute, second);
    Serial.println(buf);
}

void setup()
{
    // Инициализация последовательного порта
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nПример использования будильника DS1307 на Arduino\n"));

    // Инициализация RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC не обнаружен"));
        delay(3000);
    }

    // Включение RTC
    rtc.clockEnable(true);

    // Установка начального времени
    rtc.setTime(12, 0, 0);
}

void loop()
{
    static uint8_t secondLast = 0xff;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    // Чтение времени RTC
    if (!rtc.getTime(&hour, &minute, &second)) {
        Serial.println(F("Ошибка: чтение DS1302 не удалось"));
    } else {
        // Вывод времени RTC каждую секунду
        if (second != secondLast) {
            secondLast = second;

            // Вывод времени RTC
            printTime(hour, minute, second);

            // Обработка будильников
            for (uint8_t i = 0; i < sizeof(alarms) / sizeof(Alarm); i++) {
                alarms[i].tick(hour, minute, second);
            }
        }
    }

    // Ожидание 100 мс
    delay(100);
}
