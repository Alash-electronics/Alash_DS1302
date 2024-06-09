/*
 *    Пример выгрузки регистров RTC DS1302 для Arduino
 *    Исходный код: https://github.com/Alash-electronics/Alash_DS1302
 */

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

// Создание объекта DS1302
Alash_DS1302 rtc = Alash_DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

void setup(){
    // Инициализация последовательного порта
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nПример выгрузки регистров DS1302 от Alash-electronics\n"));
    // Инициализация RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC не обнаружен"));
        delay(3000);
    }
    // Включение генератора RTC
    if (!rtc.isRunning()) {
        rtc.clockEnable();
    }
}

void loop(){
    uint8_t buf[DS1302_NUM_CLOCK_REGS + 1];
    // Чтение всех регистров
    if (!rtc.readBuffer(0, buf, sizeof(buf))) {
        Serial.println(F("Ошибка чтения буферов"));
        return;
    }
    // Вывод всех регистров
    Serial.println(F("Регистры: "));
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        Serial.print(F("  "));
        Serial.print(i);
        Serial.print(F(": 0x"));
        if (buf[i] < 0x10) {
            Serial.print(F("0"));
        }
        Serial.println(buf[i], HEX);
    }

    delay(1000);
}
