#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "gpio_pin.h"
#include "spi.h"
#include "acc_sensor.h"
#include <thread>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static const char *device = "/dev/spidev0.0";

int main(int argc, char *argv[])
{
    // SPI_Handler spi(device);
    // GPIO_Pin csPin(17);

    Acceleration_Sensor sensor(device, 17);

    if (sensor.is_connected())
    {
        printf("Sensor available!\n");
    }
    else
    {
        printf("Sensor not connected!\n");
    }

    sensor.write_ctrl1(XL_ODR::LSM6DSO_XL_UI_6667Hz_HP, XL_FS::LSM6DSO_XL_UI_4g);

    if (sensor.is_connected())
    {
        printf("Sensor available!\n");
    }

    Vector_3D data;

    std::chrono::milliseconds waitDuration(100);

    while (true)
    {

        auto start = std::chrono::high_resolution_clock::now();

        /*for (int i = 0; i < 6000; i++)
        {
            sensor.read_xl_data(data);
        }*/

        std::this_thread::sleep_for(waitDuration);

        if (sensor.is_connected())
        {
            sensor.read_xl_data(data);
            printf("data: x:%f, y:%f, z:%f\n", data.x, data.y, data.z);
        }
        else
        {
            printf("Sensor not connected!\n");
        }

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;

        // std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
    }

    return 0;
}