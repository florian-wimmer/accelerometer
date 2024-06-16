#include "spi.h"
#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <unistd.h>

SPI_Handler::SPI_Handler(const std::string &device, uint32_t speedHz, uint8_t bitsPerWord, uint8_t delay)
    : spiFile(-1), devicePath(device), speedHz(speedHz), bitsPerWord(bitsPerWord), delay(delay)
{
    openHandler();
}

SPI_Handler::~SPI_Handler()
{
    closeHandler();
}

// open SPI Interface
bool SPI_Handler::openHandler()
{
    spiFile = ::open(devicePath.c_str(), O_RDWR);
    if (spiFile < 0)
    {
        std::cerr << "Failed to open the SPI handler: " << devicePath << std::endl;
        return false;
    }

    return true;
}

// close SPI interface
void SPI_Handler::closeHandler()
{
    if (spiFile >= 0)
    {
        ::close(spiFile);
        spiFile = -1;
    }
}

// tansmit and receiver message from interface
bool SPI_Handler::transfer(const uint8_t *txData, uint8_t *rxData, size_t length)
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)txData,
        .rx_buf = (unsigned long)rxData,
        .len = (uint32_t)length,
        .speed_hz = speedHz,
        .delay_usecs = delay,
        .bits_per_word = bitsPerWord,
    };

    if (ioctl(spiFile, SPI_IOC_MESSAGE(1), &tr) < 0)
    {
        std::cerr << "Failed to transfer SPI message." << std::endl;
        return false;
    }

    return true;
}

// write data from interface
bool SPI_Handler::write(const uint8_t *txData, size_t length)
{
    return transfer(txData, nullptr, length);
}

// write data from interface
bool SPI_Handler::read(uint8_t ReadAddr, uint8_t *rxData, size_t length)
{
    uint8_t tx[length];

    tx[0] = ReadAddr;

    return transfer(tx, rxData, length);
}
