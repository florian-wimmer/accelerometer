#include "gpio_pin.h"
#include <stdio.h>
#include <iostream>

// Initialize static member of class
gpiod_chip *GPIO_Pin::chip;
const char *GPIO_Pin::chipName = "gpiochip4";
bool GPIO_Pin::chip_initialized = false;

GPIO_Pin::GPIO_Pin(int pinNumber)
    : pinNumber(pinNumber)
{
    if (!chip_initialized)
    {
        initialize_chip();
    }

    line = gpiod_chip_get_line(chip, pinNumber);
    if (!line)
    {
        std::cerr << "Could not get line." << std::endl;
        gpiod_chip_close(chip);
        return;
    }

    if (gpiod_line_request_output(line, "blinktest", 0) < 0)
    {
        std::cerr << "Could not set line as output." << std::endl;
        gpiod_chip_close(chip);
        return;
    }
}

GPIO_Pin::~GPIO_Pin()
{
}

bool GPIO_Pin::initialize_chip()
{
    // initialize gpio chip

    chip = gpiod_chip_open_by_name(chipName);
    if (!chip)
    {
        std::cerr << "Could not open chip." << std::endl;
        return false;
    }

    chip_initialized = true;

    return true;
}

bool GPIO_Pin::set_state(State value)
{
    gpiod_line_set_value(line, (int)value);

    return true;
}