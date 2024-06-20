#include <iostream>
#include <iomanip>
#include <sstream>
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
#include <cstdlib>

// Device defintion for SPI Communition, depends on the used SPI interface.
static const char *device = "/dev/spidev0.0";

bool xl_detected = true;
bool gy_detected = true;

CSVWriter csv_file("measurement/data/output.csv", 1024 * 1024 * 1024);

// Interrupt service routine (ISR)
void handle_event(struct gpiod_line_event *event, int pin_number)
{
    if (event->event_type == GPIOD_LINE_EVENT_RISING_EDGE)
    {
        // the according flag is set, whenever a new sensor value is available
        if (pin_number == 5)
        {
            xl_detected = true;
        }
        if (pin_number == 6)
        {
            gy_detected = true;
        }
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
            // this will call the event handle
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

void signalHandler(int signal)
{
    std::cout << "Interrupt signal (" << signal << ") received.\n";
    // Cleanup and close up stuff here
    csv_file.~CSVWriter();
    // Terminate program
    exit(signal);
}

int main(int argc, char *argv[])
{
    // Data Variable
    Vector_3D data_xl, data_gy;

    // Define Interrupt Pins
    GPIO_Pin int_Pin_1(6, GPIO_Pin::Direction::INPUT); // INT_1
    GPIO_Pin int_Pin_2(5, GPIO_Pin::Direction::INPUT); // INT_2

    // Start event listener thread
    std::thread listener_thread1(event_listener, int_Pin_1.get_line(), int_Pin_1.get_pin_number());
    std::thread listener_thread2(event_listener, int_Pin_2.get_line(), int_Pin_2.get_pin_number());

    // Listen to CTRL+C
    signal(SIGINT, signalHandler);

    // Acceleration Sensor
    Acceleration_Sensor sensor(device, 17);

    // Check if device is connected
    if (sensor.is_connected())
    {
        printf("Sensor available!\n");
    }
    else
    {
        printf("Sensor not connected!\n");
    }

    // Configure the Sensor
    // Define the Interrupt Condition
    sensor.write_int1_ctrl(INT_CTRL::INT_DRDY_XL);
    sensor.write_int2_ctrl(INT_CTRL::INT_DRDY_G);
    // Define Sensor Speed and Resolution
    sensor.write_ctrl1_xl(XL_ODR::LSM6DSO_XL_UI_6667Hz_HP, XL_FS::LSM6DSO_XL_UI_16g);
    sensor.write_ctrl2_g(GY_ODR::LSM6DSO_GY_UI_6667Hz_HP, GY_FS::LSM6DSO_GY_UI_2000dps);

    // Initialize save file
    // csv_file = new

    // read current date
    std::time_t date_time = std::time(nullptr);
    std::tm *now = std::localtime(&date_time);
    std::ostringstream oss;
    // format current date
    oss << (now->tm_year + 1900) << '-'
        << std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << '-'
        << std::setw(2) << std::setfill('0') << now->tm_mday << ' '
        << std::setw(2) << std::setfill('0') << now->tm_hour << ':'
        << std::setw(2) << std::setfill('0') << now->tm_min << ':'
        << std::setw(2) << std::setfill('0') << now->tm_sec << std::endl;

    csv_file.writeLine("Date: " + oss.str());
    csv_file.writeLine("Calibration: XL_ODR: 6667Hz, XL_FS: 16g, GY_ODR: 6667Hz, GY_FS: 2000dps\n");

    csv_file.writeRow({
        "Time(s)",
        "Acceleration X (g)",
        "Acceleration Y (g)",
        "Acceleration Z (g)",
        "Angular Momentum X (dps)",
        "Angular Momentum Y (dps)",
        "Angular Momentum Z (dps)",
    });

    // Initialize time measurement
    auto start = std::chrono::high_resolution_clock::now();
    int second_counter = 0;

    while (true)
    {
        // this triggers when a sensor value is ready, for both accelertion and gyro
        if (xl_detected && gy_detected)
        {
            xl_detected = false;
            gy_detected = false;

            // read sensor data and measure time difference
            // sensor.read_xl_gy_data(data_xl, data_gy);
            sensor.read_xl_data(data_xl);
            sensor.read_gy_data(data_gy);
            std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;

            // write data
            csv_file.writeValues(duration.count(), data_xl, data_gy);
        }

        // keep track of duration
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;

        // Output to show how long the measurement is running
        if (duration.count() > second_counter)
        {
            second_counter++;
            std::cout << "Execution time: " << std::round(duration.count()) << " seconds" << std::endl;

            printf("XL data: x:%f, y:%f, z:%f\n", data_xl.x, data_xl.y, data_xl.z);
            printf("GY data: x:%f, y:%f, z:%f\n", data_gy.x, data_gy.y, data_gy.z);
        }
    }

    return 0;
}