/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

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
#include <gpiod.h>
#include <unistd.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
    perror(s);
    abort();
}

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 5e6; // 5e5;
static uint16_t delay;

static void transfer(int fd)
{

    gpiod_chip *chip;
    gpiod_line *line;
    // For Raspberry Pi 5 use gpiochip4 (For Raspberry Pi 4 use gpiochip0)
    const char *chipname = "gpiochip4";
    const unsigned int line_offset = 17;

    chip = gpiod_chip_open_by_name(chipname);
    if (!chip)
    {
        std::cerr << "Could not open chip." << std::endl;
        return;
    }

    line = gpiod_chip_get_line(chip, line_offset);
    if (!line)
    {
        std::cerr << "Could not get line." << std::endl;
        gpiod_chip_close(chip);
        return;
    }

    if (gpiod_line_request_output(line, "blinktest", 0) < 0)
    {
        std::cerr << "Could not set line as output." << std::endl;
        gpiod_chip_close(chip);
        return;
    }

    long unsigned int ret;
    uint8_t tx[] = {
        0x0F | 0x80,
        0x00,
    };

    // response should be 0xFF 0x6C

    uint8_t rx[ARRAY_SIZE(tx)] = {
        0,
    };

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = ARRAY_SIZE(tx),
        .speed_hz = speed,
        .delay_usecs = delay,
        .bits_per_word = bits,
    };

    // Set CS line low (assert CS)
    // gpio_set_value(CS_PIN, LOW);
    gpiod_line_set_value(line, 0);

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send spi message");

    // Set CS line high (de-assert CS)
    // gpio_set_value(CS_PIN, HIGH);
    gpiod_line_set_value(line, 1);

    for (ret = 0; ret < ARRAY_SIZE(tx); ret++)
    {
        if (!(ret % 6))
            puts("");
        printf("%.2X ", rx[ret]);
    }

    //-------------------------------------------------------- for test
    tx[0] = 0x10 | 0x00;
    tx[1] = 0x40;

    gpiod_line_set_value(line, 0);

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send spi message");

    // Set CS line high (de-assert CS)
    // gpio_set_value(CS_PIN, HIGH);
    gpiod_line_set_value(line, 1);

    for (ret = 0; ret < ARRAY_SIZE(tx); ret++)
    {
        if (!(ret % 6))
            puts("");
        printf("%.2X ", rx[ret]);
    }

    //-------------------------------------------------------- for test
    tx[0] = 0x29 | 0x80;
    tx[1] = 0x00;

    gpiod_line_set_value(line, 0);

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send spi message");

    // Set CS line high (de-assert CS)
    // gpio_set_value(CS_PIN, HIGH);
    gpiod_line_set_value(line, 1);

    for (ret = 0; ret < ARRAY_SIZE(tx); ret++)
    {
        if (!(ret % 6))
            puts("");
        printf("%.2X ", rx[ret]);
    }

    puts("");

    gpiod_chip_close(chip);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int fd;

    fd = open(device, O_RDWR);
    if (fd < 0)
        pabort("can't open device");

    /*
     * spi mode
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz");

    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);

    transfer(fd);

    close(fd);

    /*
    while (true)
    {
        printf("test a\n");

        gpiod_line_set_value(line, 1);
        usleep(500000); // 500ms delay
        // gpiod_line_set_value(line, 0);
        // usleep(500000); // 500ms delay
    }
    */

    return 0;
}
