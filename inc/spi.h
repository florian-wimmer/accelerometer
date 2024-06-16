#ifndef SPI_H
#define SPI_H

#include <cstdint>
#include <string>
#include <gpiod.h>

class SPI_Handler
{
public:
    SPI_Handler(const std::string &device, uint32_t speedHz = 5e6, uint8_t bitsPerWord = 8, uint8_t delay = 0);
    ~SPI_Handler();

    bool transfer(const uint8_t *txData, uint8_t *rxData, size_t length);
    bool write(const uint8_t *txData, size_t length);
    bool read(uint8_t ReadAddr, uint8_t *rxData, size_t length);

private:
    int spiFile;
    std::string devicePath; // device path needs to be known on start up
    uint32_t speedHz;       // interface communication speed
    uint8_t bitsPerWord;    // word bit length
    uint8_t delay;          // delay between words (us)

    bool openHandler();
    void closeHandler();
};

#endif // SPI_H