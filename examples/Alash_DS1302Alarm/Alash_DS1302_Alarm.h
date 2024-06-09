/* Пример использования будильника RTC DS1302 на Arduino
 *
 * Требуемая библиотека:
 *   https://github.com/Alash-electronics/Alash_DS1302
 */
#include <Arduino.h>
class Alarm{   //! Класс будильника
public:
    /*!
     * \brief Конструктор будильника
     * \param hour Час срабатывания будильника
     * \param minute Минута срабатывания будильника
     * \param second Секунда срабатывания будильника
     * \param handler Обработчик будильника
     */
    Alarm(uint8_t hour, uint8_t minute, uint8_t second, void (*handler)()) :
            _hour(hour), _minute(minute), _second(second), _handler(handler)
    {
    }

    /*!
     * \brief Обработка будильника при изменении каждой секунды
     * \param hour Текущий час
     * \param minute Текущая минута
     * \param second Текущая секунда
     */
    void tick(uint8_t hour, uint8_t minute, uint8_t second) {
        if ((hour == _hour) && (minute == _minute) && (second == _second)) {
            (*_handler)();
        }
    }

private:
    uint8_t _hour;        //!< Час срабатывания будильника
    uint8_t _minute;      //!< Минута срабатывания будильника
    uint8_t _second;      //!< Секунда срабатывания будильника

    void (*_handler)(); //!< Обработчик будильника
};
