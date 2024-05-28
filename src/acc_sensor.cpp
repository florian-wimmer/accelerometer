#include "acc_sensor.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

// Register addresses for LSM6DSO (example, adjust as needed)
#define WHO_AM_I_REG 0x0F
#define CTRL1_XL 0x10
#define OUTX_L_XL 0x28
#define OUTX_H_XL 0x29
#define OUTY_L_XL 0x2A
#define OUTY_H_XL 0x2B
#define OUTZ_L_XL 0x2C
#define OUTZ_H_XL 0x2D

Acceleration_Sensor::Acceleration_Sensor(const std::string &spi_device, int cs_pin)
    : spi(spi_device), csPin(cs_pin) {}

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

uint8_t Acceleration_Sensor::readRegister(uint8_t reg_addr)
{
    reg_addr = reg_addr | 0x80;

    select();

    spi.read(reg_addr, rx_buffer, 2);

    deselect();

    return rx_buffer[1];
}

void Acceleration_Sensor::writeRegister(uint8_t reg_addr, uint8_t value)
{
    uint8_t tx[2];

    tx[0] = reg_addr;
    tx[1] = value;

    select();

    spi.write(tx, 2);

    deselect();
}

bool Acceleration_Sensor::is_connected()
{
    uint8_t read_reg;

    read_reg = readRegister(WHO_AM_I_REG);

    if (read_reg == LSM6DSO_ID)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Acceleration_Sensor::write_ctrl1(XL_ODR speed_config, XL_FS scale_config)
{
    uint8_t value = ((uint8_t)speed_config << 4) + ((uint8_t)scale_config << 2);

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

bool Acceleration_Sensor::read_xl_data(Vector_3D &vec)
{
    uint8_t x_l = readRegister(OUTX_L_XL);
    uint8_t x_h = readRegister(OUTX_H_XL);
    uint8_t y_l = readRegister(OUTY_L_XL);
    uint8_t y_h = readRegister(OUTY_H_XL);
    uint8_t z_l = readRegister(OUTZ_L_XL);
    uint8_t z_h = readRegister(OUTZ_H_XL);

    vec.x = (int16_t)(x_h << 8 | x_l);
    vec.y = (int16_t)(y_h << 8 | y_l);
    vec.z = (int16_t)(z_h << 8 | z_l);

    vec.x = vec.x * LSB_16BIT * 2 * xl_gain;
    vec.y = vec.y * LSB_16BIT * 2 * xl_gain;
    vec.z = vec.z * LSB_16BIT * 2 * xl_gain;

    return true;
}