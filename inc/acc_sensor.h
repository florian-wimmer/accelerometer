#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "spi.h"
#include "gpio_pin.h"
#include <lsm6dso_reg.h> // here are all registeres and their according values are defined

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

    void write_int1_ctrl(INT_CTRL int_config);
    void write_int2_ctrl(INT_CTRL int_config);

    void write_ctrl1_xl(XL_ODR speed_config, XL_FS scale_config);
    void write_ctrl2_g(GY_ODR speed_config, GY_FS scale_config);

    bool read_xl_data(Vector_3D &vec);
    bool read_gy_data(Vector_3D &vec);

    bool read_single_xl_data(Vector_3D &vec);

private:
    SPI_Handler spi;
    GPIO_Pin csPin;

    double xl_gain = 1;
    double gy_gain = 1;

    uint8_t rx_buffer[BUFFER_SIZE];
    uint8_t tx_buffer[BUFFER_SIZE];

    void select();
    void deselect();
    bool readRegister(uint8_t reg_addr, uint8_t &value);
    bool writeRegister(uint8_t reg_addr, uint8_t value);
    bool readMultipleRegister(uint8_t *reg_addr, uint8_t *value, uint16_t length);
};

#endif // ACCELERATION_SENSOR_H