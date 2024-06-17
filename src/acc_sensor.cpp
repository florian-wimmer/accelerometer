#include "acc_sensor.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
#include <cstring>

Acceleration_Sensor::Acceleration_Sensor(const std::string &spi_device, int cs_pin)
    : spi(spi_device), csPin(cs_pin, GPIO_Pin::Direction::OUTPUT) {}

Acceleration_Sensor::~Acceleration_Sensor()
{
}

// Chip Select - sensor - select
void Acceleration_Sensor::select()
{
    // Pull CS low
    csPin.set_state(GPIO_Pin::State::LOW);
}

// Chip Select - sensor - deselect
void Acceleration_Sensor::deselect()
{
    // Pull CS high
    csPin.set_state(GPIO_Pin::State::HIGH);
}

// write to sensor - single regsiter
bool Acceleration_Sensor::readRegister(uint8_t reg_addr, uint8_t &value)
{
    reg_addr = reg_addr | 0x80;
    bool com_flag;

    select();

    com_flag = spi.read(reg_addr, rx_buffer, 2);

    deselect();

    // if communication was succesfull save value, otherwise set value to 0
    if (com_flag && rx_buffer[0] == 0xFF)
    {
        value = rx_buffer[1];
    }
    else
    {
        com_flag = false;
        value = 0;
    }

    return com_flag;
}

// read form sensor - single regsiter
bool Acceleration_Sensor::writeRegister(uint8_t reg_addr, uint8_t value)
{
    uint8_t tx[2];
    bool com_flag;

    tx[0] = reg_addr;
    tx[1] = value;

    select();

    com_flag = spi.write(tx, 2);

    deselect();

    return com_flag;
}

// read from sensor - variable length
bool Acceleration_Sensor::readMultipleRegister(uint8_t *reg_addr, uint8_t *value, uint16_t length)
{
    bool com_flag;

    reg_addr[0] = reg_addr[0] | 0x80;

    select();

    com_flag = spi.transfer(reg_addr, rx_buffer, length);

    deselect();

    // if communication was succesfull save value, otherwise set value to 0
    if (com_flag && rx_buffer[0] == 0xFF)
    {
        std::memcpy(value, rx_buffer, length * sizeof(uint8_t));
    }
    else
    {
        com_flag = false;
        value = 0;
    }

    return com_flag;
}

// check if device is connected, this register should return always the same value
bool Acceleration_Sensor::is_connected()
{
    bool com_flag;
    uint8_t value;

    com_flag = readRegister(LSM6DSO_WHO_AM_I, value);

    if (value == LSM6DSO_ID || com_flag)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// configure int1 pin
void Acceleration_Sensor::write_int1_ctrl(INT_CTRL int_config)
{
    uint8_t value = (uint8_t)int_config;

    writeRegister(LSM6DSO_INT1_CTRL, value);

    return;
}

// configure int2 pin
void Acceleration_Sensor::write_int2_ctrl(INT_CTRL int_config)
{
    uint8_t value = (uint8_t)int_config;

    writeRegister(LSM6DSO_INT2_CTRL, value);

    return;
}

// configure xl sensor
// speed -> measurement frequency
// scale -> sensor range
void Acceleration_Sensor::write_ctrl1_xl(XL_ODR speed_config, XL_FS scale_config)
{
    uint8_t value = ((uint8_t)speed_config << 4) | ((uint8_t)scale_config << 2);

    switch (scale_config)
    {
    case LSM6DSO_XL_UI_2g:
        xl_gain = 2;
        break;
    case LSM6DSO_XL_UI_4g:
        xl_gain = 4;
        break;
    case LSM6DSO_XL_UI_8g:
        xl_gain = 8;
        break;
    case LSM6DSO_XL_UI_16g:
        xl_gain = 16;
        break;
    default:
        xl_gain = 1;
        break;
    }

    writeRegister(LSM6DSO_CTRL1_XL, value);

    return;
}

// configure gy sensor
// speed -> measurement frequency
// scale -> sensor range
void Acceleration_Sensor::write_ctrl2_g(GY_ODR speed_config, GY_FS scale_config)
{
    uint8_t value = ((uint8_t)speed_config << 4) | ((uint8_t)scale_config << 1);

    switch (scale_config)
    {
    case LSM6DSO_GY_UI_125dps:
        gy_gain = 125;
        break;
    case LSM6DSO_GY_UI_250dps:
        gy_gain = 250;
        break;
    case LSM6DSO_GY_UI_500dps:
        gy_gain = 500;
        break;
    case LSM6DSO_GY_UI_1000dps:
        gy_gain = 1000;
        break;
    case LSM6DSO_GY_UI_2000dps:
        gy_gain = 2000;
        break;
    default:
        gy_gain = 1;
        break;
    }

    writeRegister(LSM6DSO_CTRL2_G, value);
}

// read the current xl value (g)
bool Acceleration_Sensor::read_xl_data(Vector_3D &vec)
{
    bool com_flag = true;

    uint8_t reg_array[7] = {0};
    uint8_t value_array[7];

    reg_array[0] = LSM6DSO_OUTX_L_A;

    com_flag = readMultipleRegister(reg_array, value_array, 7);

    vec.x = (int16_t)(value_array[2] << 8 | value_array[1]);
    vec.y = (int16_t)(value_array[4] << 8 | value_array[3]);
    vec.z = (int16_t)(value_array[6] << 8 | value_array[5]);

    vec.x = vec.x * LSB_16BIT * 2 * xl_gain;
    vec.y = vec.y * LSB_16BIT * 2 * xl_gain;
    vec.z = vec.z * LSB_16BIT * 2 * xl_gain;

    return com_flag;
}

// read the current gy value (dps)
bool Acceleration_Sensor::read_gy_data(Vector_3D &vec)
{
    bool com_flag = true;

    uint8_t reg_array[7] = {0};
    uint8_t value_array[7];

    reg_array[0] = LSM6DSO_OUTX_L_G;

    com_flag = readMultipleRegister(reg_array, value_array, 7);

    vec.x = (int16_t)(value_array[2] << 8 | value_array[1]);
    vec.y = (int16_t)(value_array[4] << 8 | value_array[3]);
    vec.z = (int16_t)(value_array[6] << 8 | value_array[5]);

    vec.x = vec.x * LSB_16BIT * 2 * gy_gain;
    vec.y = vec.y * LSB_16BIT * 2 * gy_gain;
    vec.z = vec.z * LSB_16BIT * 2 * gy_gain;

    return com_flag;
}

bool Acceleration_Sensor::read_xl_gy_data(Vector_3D &vec_xl, Vector_3D &vec_gy)
{
    bool com_flag = true;

    uint8_t reg_array[13] = {0};
    uint8_t value_array[13];

    reg_array[0] = LSM6DSO_OUTX_L_G;

    com_flag = readMultipleRegister(reg_array, value_array, 13);

    vec_gy.x = (int16_t)(value_array[2] << 8 | value_array[1]);
    vec_gy.y = (int16_t)(value_array[4] << 8 | value_array[3]);
    vec_gy.z = (int16_t)(value_array[6] << 8 | value_array[5]);

    vec_gy.x = vec_gy.x * LSB_16BIT * 2 * gy_gain;
    vec_gy.y = vec_gy.y * LSB_16BIT * 2 * gy_gain;
    vec_gy.z = vec_gy.z * LSB_16BIT * 2 * gy_gain;

    vec_xl.x = (int16_t)(value_array[8] << 8 | value_array[7]);
    vec_xl.y = (int16_t)(value_array[10] << 8 | value_array[9]);
    vec_xl.z = (int16_t)(value_array[12] << 8 | value_array[11]);

    vec_xl.x = vec_xl.x * LSB_16BIT * 2 * xl_gain;
    vec_xl.y = vec_xl.y * LSB_16BIT * 2 * xl_gain;
    vec_xl.z = vec_xl.z * LSB_16BIT * 2 * xl_gain;

    return com_flag;
}

// read the current xl value (g)
// reads each regsiter seperatly, therefore slow
bool Acceleration_Sensor::read_single_xl_data(Vector_3D &vec)
{
    bool com_flag = true;

    uint8_t x_l, x_h;
    uint8_t y_l, y_h;
    uint8_t z_l, z_h;

    readRegister(LSM6DSO_OUTX_L_A, x_l);
    readRegister(LSM6DSO_OUTX_H_A, x_h);
    readRegister(LSM6DSO_OUTY_L_A, y_l);
    readRegister(LSM6DSO_OUTY_H_A, y_h);
    readRegister(LSM6DSO_OUTZ_L_A, z_l);
    readRegister(LSM6DSO_OUTZ_H_A, z_h);

    vec.x = (int16_t)(x_h << 8 | x_l);
    vec.y = (int16_t)(y_h << 8 | y_l);
    vec.z = (int16_t)(z_h << 8 | z_l);

    vec.x = vec.x * LSB_16BIT * 2 * xl_gain;
    vec.y = vec.y * LSB_16BIT * 2 * xl_gain;
    vec.z = vec.z * LSB_16BIT * 2 * xl_gain;

    return com_flag;
}
