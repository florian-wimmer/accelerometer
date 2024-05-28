#ifndef SPI_H
#define SPI_H

#include <cstdint>
#include <string>
#include <gpiod.h>

class SPI_Handler
{
public:
    SPI_Handler(const std::string &device, uint8_t mode = 0, uint32_t speedHz = 5e6, uint8_t bitsPerWord = 8, uint8_t delay = 0);
    ~SPI_Handler();

    bool transfer(const uint8_t *txData, uint8_t *rxData, size_t length);
    bool write(const uint8_t *txData, size_t length);
    bool read(uint8_t ReadAddr, uint8_t *rxData, size_t length);

private:
    int spiFile;
    std::string devicePath;
    uint8_t mode;
    uint32_t speedHz;
    uint8_t bitsPerWord;
    uint8_t delay;

    bool openHandler();
    void closeHandler();
};

#endif // SPI_H