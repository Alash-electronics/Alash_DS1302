/*
            Пример работы с высокоточным часами реального времени DS1302 через последовательный терминал для Arduino
            Исходники: https://github.com/Alash-electronics/Alash_DS1302
*/

#include <Alash_DS1302.h>
#include <AlashSerialTerminal.h>

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

// Создание объекта DS1302 RTC
Alash_DS1302 rtc = Alash_DS1302(DS1302_CLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);

// Символ новой строки '\r' или '\n'
char newlineChar = '\n';
// Символ-разделитель между командами и аргументами
char delimiterChar = ' ';

// Создание объекта последовательного терминала
SerialTerminal term(newlineChar, delimiterChar);

// Определение дней недели во флеше
const char day_0[] PROGMEM = "Sunday";
const char day_1[] PROGMEM = "Monday";
const char day_2[] PROGMEM = "Tuesday";
const char day_3[] PROGMEM = "Wednesday";
const char day_4[] PROGMEM = "Thursday";
const char day_5[] PROGMEM = "Friday";
const char day_6[] PROGMEM = "Saturday";

const char* const dayWeekTable[] PROGMEM = {
  day_0, day_1, day_2, day_3, day_4, day_5, day_6
};

// Определение месяцев во флеше
const char month_0[] PROGMEM = "January";
const char month_1[] PROGMEM = "February";
const char month_2[] PROGMEM = "March";
const char month_3[] PROGMEM = "April";
const char month_4[] PROGMEM = "May";
const char month_5[] PROGMEM = "June";
const char month_6[] PROGMEM = "July";
const char month_7[] PROGMEM = "August";
const char month_8[] PROGMEM = "September";
const char month_9[] PROGMEM = "October";
const char month_10[] PROGMEM = "November";
const char month_11[] PROGMEM = "December";

const char* const monthTable[] PROGMEM = {
  month_0, month_1, month_2, month_3, month_4, month_5,
  month_6, month_7, month_8, month_9, month_10, month_11
};

bool periodicPrintEnable = false;
bool outputClockEnable = false;


bool getArgumentInt(int *argumentValue) {
  int val;
  char *arg;

  // Получение следующей строки аргумента в десятичном или шестнадцатеричном формате
  arg = term.getNext();
  if (strncmp(arg, "0x", 2) == 0) {
    if (sscanf(arg, "0x%x", &val) != 1) {
      return false;
    }
  } else {
    if (sscanf(arg, "%d", &val) != 1) {
      return false;
    }
  }

  *argumentValue = val;

  return true;
}

void cmdUnknown(const char *command) {
  // Вывод неизвестной команды
  Serial.print(F("Неизвестная команда: "));
  Serial.println(command);
}

void cmdHelp() {
  // Вывод справки
  Serial.println(F("Использование терминала DS1302 RTC:"));
  Serial.println(F("  help или ?          Вывести это сообщение"));
  Serial.println();
  Serial.println(F(" Функции даты/времени:"));
  Serial.println(F("  print              Включить периодический вывод эпохи/даты/времени/температуры"));
  Serial.println(F("  dt                 Вывести короткую дату и время"));
  Serial.println(F("  date               Вывести полную дату"));
  Serial.println(F("  time               Вывести время"));
  Serial.println(F("  epoch              Вывести эпоху"));
  Serial.println(F("  set date <д w-d-m-Y> Установить день недели и дату"));
  Serial.println(F("  set time <Ч:М:С>   Установить время"));
  Serial.println(F("  set epoch <значение> Установить эпоху"));
  Serial.println();
  Serial.println(F(" Функции часов:"));
  Serial.println(F("  stop               Остановить генератор RTC"));
  Serial.println(F("  start              Запустить генератор RTC"));
  Serial.println();
  Serial.println(F(" Диагностика:"));
  Serial.println(F("  w [рег] [знач]     Записать в регистр"));
  Serial.println(F("  r [рег]            Прочитать регистр"));
}

void cmdTogglePrint() {
  // Включение/выключение вывода
  periodicPrintEnable ^= true;

  Serial.print(F("Периодический вывод: "));
  Serial.println(periodicPrintEnable ? F("Включен") : F("Выключен"));
  Serial.println();
}

void cmdPrintDate() {
  struct tm dt;
  char buf[10];

  Serial.print(F("Дата: "));

  // Чтение даты/времени из RTC
  if (!rtc.read(&dt)) {
    Serial.println(F("Ошибка: Не удалось прочитать дату/время"));
    return;
  }

  // Вывод дня недели, дня месяца, месяца и года
  strncpy_P(buf, (char *)pgm_read_dword(&(dayWeekTable[dt.tm_wday])), sizeof(buf));
  Serial.print(buf);
  Serial.print(F(" "));
  Serial.print(dt.tm_mday);
  Serial.print(F(" "));
  strncpy_P(buf, (char *)pgm_read_dword(&(monthTable[dt.tm_mon])), sizeof(buf));
  Serial.print(buf);
  Serial.print(F(" "));
  Serial.println(dt.tm_year + 1900);
}

void cmdPrintTime() {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  char buf[9];

  Serial.print(F("Время: "));

  // Чтение времени из RTC
  if (!rtc.getTime(&hour, &minute, &second)) {
    Serial.println(F("Ошибка: Не удалось прочитать время"));
    return;
  }

  // Вывод времени
  snprintf(buf, sizeof(buf), "%d:%02d:%02d", hour, minute, second);
  Serial.println(buf);
}

void cmdPrintDateTime() {
  struct tm dt;
  char buf[11];

  // Чтение даты/времени из RTC
  if (!rtc.read(&dt)) {
    Serial.println(F("Ошибка: Не удалось прочитать дату/время"));
    return;
  }

  Serial.print(F("Дата:  "));
  snprintf(buf, sizeof(buf), "%d-%d-%d", dt.tm_mday, dt.tm_mon + 1, dt.tm_year + 1900);
  Serial.println(buf);
  Serial.print(F("Время:  "));
  snprintf(buf, sizeof(buf), "%d:%02d:%02d", dt.tm_hour, dt.tm_min, dt.tm_sec);
  Serial.println(buf);
}

void cmdPrintEpoch() {
  // Вывод 32-битной эпохи
  Serial.print(F("Эпоха: "));
  Serial.println((uint32_t)rtc.getEpoch());
}

void cmdSetDateTime() {
  struct tm dt;
  int second;
  int minute;
  int hour;
  int dayWeek;
  int dayMonth;
  int month;
  int year;
  long unsigned int epoch;
  char *arg;

  arg = term.getNext();
  if (strncmp(arg, "date", 4) == 0) {
    arg = term.getRemaining();
    if (arg != NULL) {
      if (sscanf(arg, "%d %d-%d-%d", &dayWeek, &dayMonth, &month, &year) == 4) {
        rtc.read(&dt);
        dt.tm_wday = dayWeek;
        dt.tm_mday = dayMonth;
        dt.tm_mon = month - 1;
        dt.tm_year = year - 1900;
        rtc.write(&dt);
        Serial.print(F("Установлена дата: "));
        Serial.println(arg);
      } else {
        Serial.println(F("Неверный формат времени"));
      }
    }
  } else if (strncmp(arg, "time", 4) == 0) {
    arg = term.getRemaining();
    if (arg != NULL) {
      if (sscanf(arg, "%d:%d:%d", &hour, &minute, &second) == 3) {
        rtc.setTime(hour, minute, second);
        Serial.print(F("Установлено время: "));
        Serial.println(arg);
      } else {
        Serial.println(F("Неверный формат времени"));
      }
    }
  } else if (strncmp(arg, "epoch", 5) == 0) {
    arg = term.getRemaining();
    if (arg != NULL) {
      if (sscanf(arg, "%lu", &epoch) == 1) {
        rtc.setEpoch((time_t)epoch);
        Serial.print(F("Установлена эпоха: "));
        Serial.println((uint32_t)epoch);
      } else {
        Serial.println(F("Неверный формат времени"));
      }
    }
  } else {
    Serial.println(F("Недопустимый аргумент"));
  }
}

void cmdOscillatorStop() {
  Serial.println(F("Остановить генератор"));

  rtc.clockEnable(false);
}

void cmdOscillatorStart() {
  Serial.println(F("Запустить генератор"));

  rtc.clockEnable(true);
}

void cmdWriteRegister() {
  int reg;
  int val;
  char buf[13];

  if (getArgumentInt(&reg) != true) {
    Serial.println(F("Ошибка: Неверный формат регистра"));
    return;
  }

  if (getArgumentInt(&val) != true) {
    Serial.println(F("Ошибка: Неверный формат значения"));
    return;
  }

  rtc.writeRegister(reg, val);
  Serial.print(F("Запись в регистр "));
  snprintf(buf, sizeof(buf), "0x%02x: 0x%02x", reg, val);
  Serial.println(buf);
}

void cmdReadRegister() {
  int reg;
  char buf[13];

  if (getArgumentInt(&reg) != true) {
    Serial.println(F("Ошибка: Неверный формат регистра"));
    return;
  }

  Serial.print(F("Чтение из регистра "));
  snprintf(buf, sizeof(buf), "0x%02x: 0x%02x", reg, rtc.readRegister(reg));
  Serial.println(buf);
}

void printDateTime() {
  struct tm dt;
  char buf[32];

  // Чтение даты/времени из RTC
  if (!rtc.read(&dt)) {
    Serial.println(F("Ошибка: Не удалось прочитать дату/время"));
    return;
  }

  snprintf(buf, sizeof(buf), "%d %02d-%02d-%d %d:%02d:%02d",
           dt.tm_wday,
           dt.tm_mday, dt.tm_mon + 1, dt.tm_year + 1900,
           dt.tm_hour, dt.tm_min, dt.tm_sec);
  Serial.println(buf);
}

void setup() {
  // Инициализация последовательного порта
  delay(500);
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println(F("\nПример работы с DS1302 RTC через терминал Alash-electronics"));
  Serial.println(F("Выберите правильный вариант окончание строки"));
  Serial.println(F("Вместо \"Нет конца строки\" выберите \"NL+CR\" "));
  Serial.println(F("Введите 'help', чтобы увидеть справку."));

  // Установка обработчика по умолчанию для неизвестных команд
  term.setDefaultHandler(cmdUnknown);

  // Добавление обработчиков команд
  term.addCommand("?", cmdHelp);
  term.addCommand("help", cmdHelp);

  term.addCommand("print", cmdTogglePrint);
  term.addCommand("dt", cmdPrintDateTime);
  term.addCommand("date", cmdPrintDate);
  term.addCommand("time", cmdPrintTime);
  term.addCommand("epoch", cmdPrintEpoch);
  term.addCommand("set", cmdSetDateTime);

  term.addCommand("stop", cmdOscillatorStop);
  term.addCommand("start", cmdOscillatorStart);

  term.addCommand("w", cmdWriteRegister);
  term.addCommand("r", cmdReadRegister);

  // Инициализация RTC
  while (!rtc.begin()) {
    Serial.println(F("DS1302 RTC не найден"));
    delay(3000);
  }

  // Проверка статуса генератора
  if (!rtc.isRunning()) {
    Serial.println(F("Предупреждение: Часы RTC остановлены."));

    // Включение генератора
    rtc.clockEnable(true);
  }
}

void loop() {
  static unsigned long printTimestamp;

  // Чтение из последовательного порта и обработка обратных вызовов команд
  term.readSerial();

  // Печать каждую секунду
  if (periodicPrintEnable) {
    if ((millis() - printTimestamp) > 1000) {
      printTimestamp = millis();

      // Печать эпохи/даты/времени/температуры
      cmdPrintEpoch();
      cmdPrintDateTime();
      Serial.println();
    }
  }
}
