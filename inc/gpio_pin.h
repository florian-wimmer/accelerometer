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

    GPIO_Pin(int pinNumber);
    ~GPIO_Pin();

    static bool initialize_chip();

    bool set_state(State value);

private:
    static gpiod_chip *chip;
    static const char *chipName;
    static bool chip_initialized;
    int pinNumber;

    gpiod_line *line;
};

#endif