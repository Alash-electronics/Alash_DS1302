#
# Источник:         https://github.com/Alash-electronics/Alash_DS1302
#

import datetime  # Импорт модуля для работы с датой и временем
import serial  # Импорт модуля для работы с последовательным портом
import sys  # Импорт модуля для работы с системой

SERIAL_PORT = '/dev/ttyACM0'  # Последовательный порт, к которому подключено устройство
BAUDRATE = 115200  # Скорость передачи данных через последовательный порт

def read_line(line):
    line = line.decode('ascii')  # Декодирование строки в ASCII
    line = line.strip()  # Удаление пробелов и символов новой строки из строки
    return line

def set_date(ser):
    # Получение текущей даты и времени системы
    now = datetime.datetime.now()

    # Формирование строки для установки даты
    date_str = 'set date {} {}-{}-{}\n'.format(
        now.weekday() + 1, now.day, now.month, now.year)

    # Запись команды установки даты в последовательный порт
    ser.write(str.encode(date_str))

def set_time(ser):
    # Ожидание смены секунды
    last = datetime.datetime.now()
    while 1:
        now = datetime.datetime.now()
        if last.second != now.second:
            break
    now += datetime.timedelta(0, 1)

    # Запись команды установки времени в последовательный порт
    ser.write(str.encode('set time {}:{}:{}'.format(now.hour, now.minute, now.second)))

    # Ожидание смены секунды
    newline = str.encode('\n')
    last = datetime.datetime.now()
    while 1:
        now = datetime.datetime.now()
        if last.second != now.second:
            break

    # Запись символа новой строки в последовательный порт
    ser.write(newline)

def main():
    print('Пример установки даты и времени RTC DS1302 от Alash-electronics через терминал')

    # Инициализация последовательного порта
    ser = serial.Serial()
    ser.baudrate = int(BAUDRATE)
    ser.bytesize = 8
    ser.stopbits = 1
    ser.parity = serial.PARITY_NONE
    ser.timeout = 0.01
    ser.port = SERIAL_PORT
    try:
        ser.open()
    except serial.SerialException:
        print('Ошибка: Не удается открыть последовательный порт {}'.format(SERIAL_PORT))
        sys.exit(1)

    while 1:
        # Чтение строки из последовательного порта
        line = read_line(ser.readline())
        if line:
            # Вывод каждой строки
            print(line)

            # Ожидание строки старта терминала
            if line.find('Пример работы с терминалом RTC DS1302 от Alash-electronics') == 0:
                # Установка даты
                set_date(ser)

                # Установка времени
                set_time(ser)

                # Включение непрерывных выводов
                ser.write(str.encode('print\n'))

if __name__ == '__main__':
    main()