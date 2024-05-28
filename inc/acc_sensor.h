#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "spi.h"
#include "gpio_pin.h"
#include <lsm6dso_reg.h>

#define BUFFER_SIZE 9216
#define LSB_16BIT 1.0 / 65536.0

typedef struct
{
    double x;
    double y;
    double z;
} Vector_3D;

class Acceleration_Sensor
{
public:
    Acceleration_Sensor(const std::string &spi_device, int cs_pin);
    ~Acceleration_Sensor();

    bool is_connected();

    bool configure();
    void write_ctrl1(XL_ODR speed_config, XL_FS scale_config);

    bool read_xl_data(Vector_3D &vec);

private:
    SPI_Handler spi;
    GPIO_Pin csPin;

    double xl_gain = 1;

    uint8_t rx_buffer[BUFFER_SIZE];
    uint8_t tx_buffer[BUFFER_SIZE];

    void select();
    void deselect();
    uint8_t readRegister(uint8_t reg_addr);
    void writeRegister(uint8_t reg_addr, uint8_t value);
};

#endif // ACCELERATION_SENSOR_H