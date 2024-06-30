#include "gpio_pin.h"
#include <stdio.h>
#include <iostream>

// Initialize static member of class
gpiod_chip *GPIO_Pin::chip;
const char *GPIO_Pin::chipName = "gpiochip4"; // Raspberry Pi specific value
bool GPIO_Pin::chip_initialized = false;

GPIO_Pin::GPIO_Pin(int pin_number, Direction pin_direction)
    : pin_number(pin_number), pin_direction(pin_direction)
{
    // Initialize GPIO chip
    if (!chip_initialized)
    {
        initialize_chip();
    }

    // Get GPIO line
    line = gpiod_chip_get_line(chip, pin_number);
    if (!line)
    {
        std::cerr << "Could not get line." << std::endl;
        gpiod_chip_close(chip);
        return;
    }

    // Set the output direction - can only be hold by one instance
    if (pin_direction == Direction::OUTPUT)
    {
        // Request GPIO line as output
        if (gpiod_line_request_output(line, "blinktest", 0) < 0)
        {
            std::cerr << "Could not set line as output." << std::endl;
            gpiod_chip_close(chip);
            return;
        }
    }
    else if (pin_direction == Direction::INPUT)
    {
        // Request GPIO line as input with both edge detection
        if (gpiod_line_request_both_edges_events(line, "gpio_event_listener") < 0)
        {
            std::cerr << "Failed to request GPIO line as input" << std::endl;
            gpiod_chip_close(chip);
            return;
        }
    }
}

GPIO_Pin::~GPIO_Pin()
{
}

bool GPIO_Pin::initialize_chip()
{
    // Initialize GPIO chip
    chip = gpiod_chip_open_by_name(chipName);
    if (!chip)
    {
        std::cerr << "Could not open chip." << std::endl;
        return false;
    }

    chip_initialized = true;

    return true;
}

gpiod_line *GPIO_Pin::get_line()
{
    return line;
}

// get pin number - as numbered in raspberry pi docu
int GPIO_Pin::get_pin_number()
{
    return pin_number;
}

// change state, only possible when set as output
bool GPIO_Pin::set_state(State value)
{
    gpiod_line_set_value(line, (int)value);

    return true;
}