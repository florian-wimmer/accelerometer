#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "gpio_pin.h"
#include "spi.h"
#include "acc_sensor.h"
#include <thread>
#include "csv_writer.h"

#include <csignal>
#include <gpiod.h>
#include <unistd.h>
#include <thread>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static const char *device = "/dev/spidev0.0";

bool xl_detected = true;
bool gy_detected = true;

// Interrupt service routine (ISR)
void handle_event(struct gpiod_line_event *event, int pin_number)
{
    if (event->event_type == GPIOD_LINE_EVENT_RISING_EDGE)
    {
        if (pin_number == 5)
        {
            xl_detected = true;
        }
        if (pin_number == 6)
        {
            gy_detected = true;
        }

        // std::cout << "Interrupt detected: " << pin_number << std::endl;
    }
}

// Function to handle GPIO events in a separate thread
void event_listener(gpiod_line *line, int pin_number)
{
    while (true)
    {
        struct gpiod_line_event event;
        int ret = gpiod_line_event_wait(line, NULL);

        if (ret > 0)
        {
            gpiod_line_event_read(line, &event);
            handle_event(&event, pin_number);
        }
        else if (ret < 0)
        {
            std::cerr << "Error waiting for event" << std::endl;
            break;
        }
    }
}

#define CHIP_NAME "/dev/gpiochip4"
#define GPIO_LINE 20

int main(int argc, char *argv[])
{
    // SPI_Handler spi(device);
    GPIO_Pin int_Pin_1(6, GPIO_Pin::Direction::INPUT); // INT_1
    GPIO_Pin int_Pin_2(5, GPIO_Pin::Direction::INPUT); // INT_2

    // Start event listener thread
    std::thread listener_thread1(event_listener, int_Pin_1.get_line(), int_Pin_1.get_pin_number());
    std::thread listener_thread2(event_listener, int_Pin_2.get_line(), int_Pin_2.get_pin_number());

    Acceleration_Sensor sensor(device, 17);

    if (sensor.is_connected())
    {
        printf("Sensor available!\n");
    }
    else
    {
        printf("Sensor not connected!\n");
    }

    sensor.write_int1_ctrl(INT_CTRL::INT_DRDY_XL);
    sensor.write_int2_ctrl(INT_CTRL::INT_DRDY_G);
    sensor.write_ctrl1_xl(XL_ODR::LSM6DSO_XL_UI_6667Hz_HP, XL_FS::LSM6DSO_XL_UI_4g);
    sensor.write_ctrl2_g(GY_ODR::LSM6DSO_GY_UI_6667Hz_HP, GY_FS::LSM6DSO_GY_UI_125dps);

    // sensor.configure_fifo();

    Vector_3D data_xl, data_gy;

    std::chrono::milliseconds waitDuration(100);

    GPIO_Pin test_pin(21, GPIO_Pin::Direction::OUTPUT);

    CSVWriter writer("output.csv");
    writer.writeRow({
        "Time(s)",
        "Acceleration X (g)",
        "Acceleration Y (g)",
        "Acceleration Z (g)",
        "Angular Momentum X (dps)",
        "Angular Momentum Y (dps)",
        "Angular Momentum Z (dps)",
    });

    auto start = std::chrono::high_resolution_clock::now();
    int time = 0;

    while (true)
    {
        if (xl_detected && gy_detected)
        {
            xl_detected = false;
            gy_detected = false;

            sensor.read_xl_data(data_xl);
            sensor.read_gy_data(data_gy);
            std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;

            // std::cout << "Values Saved: " << duration.count() << " seconds" << std::endl;

            writer.writeValues(duration.count(), data_xl, data_gy);
        }

        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;

        if (duration.count() > time)
        {
            time++;
            std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
        }

        /*
        std::this_thread::sleep_for(waitDuration);

        if (sensor.is_connected())
        {
            sensor.read_xl_data(data_xl);
            printf("XL data: x:%f, y:%f, z:%f\n", data_xl.x, data_xl.y, data_xl.z);

            sensor.read_gy_data(data_gy);
            printf("GY data: x:%f, y:%f, z:%f\n", data_gy.x, data_gy.y, data_gy.z);
        }
        else
        {
            printf("Sensor not connected!\n");
        }
        */
    }

    return 0;
}