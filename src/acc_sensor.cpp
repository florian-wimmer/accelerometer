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
void Acceleration_Sensor::select()
{
    // Pull CS low
    csPin.set_state(GPIO_Pin::State::LOW);
}

void Acceleration_Sensor::deselect()
{
    // Pull CS high
    csPin.set_state(GPIO_Pin::State::HIGH);
}

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

bool Acceleration_Sensor::configure_fifo()
{
    uint8_t value = 0;

    // watermark 0-7
    value = 250;
    writeRegister(LSM6DSO_FIFO_CTRL1, value);

    // watermark 8
    value = 0;
    writeRegister(LSM6DSO_FIFO_CTRL2, value);

    value = ((uint8_t)LSM6DSO_GY_NOT_BATCHED << 4) | ((uint8_t)LSM6DSO_XL_BATCHED_AT_6667Hz);
    writeRegister(LSM6DSO_FIFO_CTRL3, value);

    value = ((uint8_t)LSM6DSO_NO_DECIMATION << 6) | ((uint8_t)LSM6DSO_TEMP_NOT_BATCHED << 4) | ((uint8_t)LSM6DSO_STREAM_MODE);
    writeRegister(LSM6DSO_FIFO_CTRL4, value);

    return true;
}

void Acceleration_Sensor::write_int1_ctrl()
{
    uint8_t value = 1;

    writeRegister(LSM6DSO_INT1_CTRL, value);

    return;
}

void Acceleration_Sensor::write_int2_ctrl()
{
    uint8_t value = 2;

    writeRegister(LSM6DSO_INT2_CTRL, value);

    return;
}

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