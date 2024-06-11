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

// Interrupt service routine (ISR)
void handle_event(struct gpiod_line_event *event)
{
    if (event->event_type == GPIOD_LINE_EVENT_RISING_EDGE)
    {
        std::cout << "Rising edge detected!" << std::endl;
    }
}

// Function to handle GPIO events in a separate thread
void event_listener(gpiod_line *line)
{
    while (true)
    {
        struct gpiod_line_event event;
        int ret = gpiod_line_event_wait(line, NULL);

        if (ret > 0)
        {
            gpiod_line_event_read(line, &event);
            handle_event(&event);
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
    GPIO_Pin int_Pin_1(5, GPIO_Pin::Direction::INPUT);
    GPIO_Pin int_Pin_2(6, GPIO_Pin::Direction::INPUT);

    // Start event listener thread
    std::thread listener_thread1(event_listener, int_Pin_1.get_line());
    std::thread listener_thread2(event_listener, int_Pin_2.get_line());

    Acceleration_Sensor sensor(device, 17);

    if (sensor.is_connected())
    {
        printf("Sensor available!\n");
    }
    else
    {
        printf("Sensor not connected!\n");
    }

    sensor.write_ctrl1_xl(XL_ODR::LSM6DSO_XL_UI_6667Hz_HP, XL_FS::LSM6DSO_XL_UI_4g);
    sensor.write_ctrl2_g(GY_ODR::LSM6DSO_GY_UI_6667Hz_HP, GY_FS::LSM6DSO_GY_UI_125dps);

    // sensor.configure_fifo();

    Vector_3D data;

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

    while (true)
    {

        /*for (int i = 0; i < 20000; i++)
        {
            sensor.read_xl_data(data);

            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> duration = end - start;

            writer.writeValues(duration.count(), data, data);
        }*/

        std::this_thread::sleep_for(waitDuration);

        if (sensor.is_connected())
        {
            sensor.read_xl_data(data);
            printf("XL data: x:%f, y:%f, z:%f\n", data.x, data.y, data.z);

            // com_flag = sensor.read_fifo_status();

            sensor.read_gy_data(data);
            printf("GY data: x:%f, y:%f, z:%f\n", data.x, data.y, data.z);
        }
        else
        {
            printf("Sensor not connected!\n");
        }

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;

        std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
    }

    return 0;
}