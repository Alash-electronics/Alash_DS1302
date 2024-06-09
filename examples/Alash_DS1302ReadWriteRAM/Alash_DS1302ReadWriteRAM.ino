/*
 *    Пример использования памяти RAM DS1302 на Arduino
 *    Источник: https://github.com/Alash-electronics/Alash_DS1302
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

// Создание объекта DS1302 ds1302
Alash_DS1302 ds1302 = Alash_DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

void setup(){
    uint8_t dataByte;
    // Инициализация последовательного порта
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nПример использования RAM DS1302 RTC от Alash-electronics\n"));
    // Инициализация RTC
    while (!ds1302.begin()) {
        Serial.println(F("RTC не обнаружен"));
        delay(3000);
    }
    // Вывод содержимого RAM DS1302 после включения питания
    printRAM();
    // Заполнение всей памяти RAM DS1302 данными
    fillRAM();
    // Проверка данных в памяти RAM DS1302
    checkRAM();
    // Запись байта в RAM
    Serial.println(F("Запись значения 0xA9 по адресу 0x02..."));
    ds1302.writeByteRAM(0x02, 0x11);
    // Чтение байта из RAM
    Serial.print(F("Чтение из адреса 0x02: "));
    dataByte = ds1302.readByteRAM(0x02);
    // Проверка записанного байта
    if (dataByte == 0x11) {
        Serial.println(F("Успешно"));
    } else {
        Serial.println(F("ОШИБКА"));
    }
    // Вывод содержимого RAM DS1302
    printRAM();
}
void loop(){
}
void fillRAM(){
    uint8_t buf[DS1302_NUM_RAM_REGS];
    Serial.println(F("Заполнение RAM DS1302..."));
    // Заполнение буфера данными
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 1 + i;
    }
    // Запись буфера в RAM DS1302
    ds1302.writeBufferRAM(buf, sizeof(buf));
}
void checkRAM(){
    uint8_t buf[DS1302_NUM_RAM_REGS];
    Serial.print(F("Проверка RAM DS1302: "));
    // Чтение RAM DS1302
    ds1302.readBufferRAM(buf, sizeof(buf));
    // Проверка содержимого RAM DS1302
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        if (buf[i] != 1 + i) {
            // По крайней мере, один байт неверен
            Serial.println(F("ОШИБКА"));
            return;
        }
    }

    // Тест пройден
    Serial.println(F("Успешно"));
}
void printRAM(){
    uint8_t buf[DS1302_NUM_RAM_REGS];
    Serial.print(F("RAM DS1302: "));
    // Чтение RAM DS1302
    ds1302.readBufferRAM(buf, sizeof(buf));
    // Вывод буфера RAM
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        if (buf[i] < 0x10) {
            Serial.print(F("0"));
        }
        Serial.print(buf[i], HEX);
        Serial.print(F(" "));
    }
    Serial.println();
}
