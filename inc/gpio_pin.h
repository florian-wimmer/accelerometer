#ifndef GPIO_PIN_H
#define GPIO_PIN_H

#include "gpio_pin.h"
#include <gpiod.h>

class GPIO_Pin
{
public:
    enum class State
    {
        LOW = 0,
        HIGH = 1,
    };
    enum class Direction
    {
        INPUT = 0,
        OUTPUT = 1,
    };

    GPIO_Pin(int pin_number, Direction pin_direction);
    ~GPIO_Pin();

    gpiod_line *get_line();
    int get_pin_number();

    bool set_state(State value);

private:
    static gpiod_chip *chip;
    static const char *chipName;
    static bool chip_initialized;

    int pin_number;
    Direction pin_direction;

    gpiod_line *line;

    static bool initialize_chip();
};

#endif