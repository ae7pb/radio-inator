#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "ssd1306.h"
#include "pfleury/i2cmaster.h"
#include "glcdfont.c"

/*
 * Go through and set up some default values
 * that work with the OEL Display Module 
 * Univision Technology UG-2864HSWEG01
 */

void ssd1306Setup(void) {
    // arduino led for errors
    DDRB |= (1 << PINB5);
    PORTB |= (1 << PINB5);
    for (int x = 0; x < 3; x++) {
        PORTB |= (1 << PINB5);
        _delay_ms(50);
        PORTB &= ~(1 << PINB5);
        _delay_ms(50);
    }
    ssd1306SetCommandMode();

    // start the setup

    // https://github.com/thaletterb/ssd1306_avr8/blob/master/test_i2cmaster.c
    // Init sequence for 128x64 OLED module
    i2c_write(SSD1306_DISPLAY_SLEEP);

    i2c_write(SSD1306_SET_CONTRAST);
    i2c_write(0xCE); // adjust this to set contrast

    i2c_write(SSD1306_SEGMENT_REMAP_1);
    i2c_write(SSD1306_COM_DIRECTION_HIGH);
    i2c_write(SSD1306_NORMAL_DISPLAY);
    i2c_write(SSD1306_DISPLAY_ON);

    i2c_write(SSD1306_SET_MULTIPLEX_RATIO);
    i2c_write(0x3F); // 1/64 duty cycle? Don't know how this is...

    i2c_write(SSD1306_SET_DISPLAY_OFFSET);
    i2c_write(0x00);
    i2c_write(SSD1306_SET_DISPLAY_CLOCK_OSCILLATOR);
    i2c_write(0x80);

    i2c_write(SSD1306_SET_PRE_CHARGE_PERIOD);
    i2c_write(0xF1);

    i2c_write(SSD1306_SET_COM_PINS);
    i2c_write(0x12);

    i2c_write(SSD1306_SET_VCOM_DESELECT_LEVEL);
    i2c_write(0x40);

    i2c_write(SSD1306_SET_ADDRESSING);
    i2c_write(SSD1306_HORIZ_ADDR);

    i2c_write(SSD1306_SET_RAM_DISPLAY_START | 0x00);

    i2c_write(SSD1306_SET_CHARGE_PUMP);
    i2c_write(SSD1306_CHARGE_PUMP_ENABLE);
    i2c_write(SSD1306_DISPLAY_ON);
    i2c_write(SSD1306_DISPLAY_WAKE);

    i2c_stop();

}

void ssd1306SetCommandMode(void) {
    unsigned int returnValue;
    returnValue = i2c_start(SSD1306_ADDRESS + I2C_WRITE);
    if (returnValue) {
        i2c_stop();
        ssd1306Error();
    }
    i2c_write(SSD1306_CONTROL_STREAM);

}

void ssd1306SendCommand(unsigned char command[], unsigned int numCommands) {
    ssd1306SetCommandMode();
    int x;
    for (x = 0; x < numCommands; x++) {
        i2c_write(command[x]);
    }
    i2c_stop();
}

void ssd1306SetDataMode(void) {
    unsigned int returnValue;
    returnValue = i2c_start(SSD1306_ADDRESS + I2C_WRITE);
    if (returnValue) {
        i2c_stop();
        ssd1306Error();
    }
    i2c_write(SSD1306_DATA_STREAM);

}

void ssd1306SendData(unsigned char data[], unsigned int dataSize) {
    ssd1306SetDataMode();
    int x;
    for (x = 0; x < dataSize; x++) {
        i2c_write(data[x]);
    }
    i2c_stop();

}

void ssd1306Error(void) {
    while (1) {
        PORTB |= (1 << PINB5);
        _delay_ms(500);
        PORTB &= ~(1 << PINB5);
        _delay_ms(500);
    }
}

void ssd1306DrawBuffer(uint8_t columnAddress, uint8_t pageAddress, uint8_t *buff) {
    unsigned int returnValue;
    returnValue = i2c_start(SSD1306_ADDRESS + I2C_WRITE);
    if (returnValue) {
        i2c_stop();
        ssd1306Error();
    }
    unsigned char command[3];
    command[0] = SSD1306_SET_COLUMN;
    command[1] = 0;
    command[2] = 127;
    ssd1306SendCommand(command, 3);

    command[0] = SSD1306_SET_PAGE;
    command[2] = 7;
    ssd1306SendCommand(command, 3);

    ssd1306SendData(buff, 1024);
}

void ssd1306DrawPage(uint8_t columnAddress, uint8_t pageAddress, uint8_t *buff, uint16_t size) {
    unsigned int returnValue;
    returnValue = i2c_start(SSD1306_ADDRESS + I2C_WRITE);
    if (returnValue) {
        i2c_stop();
        ssd1306Error();
    }
    unsigned char command[3];
    command[0] = SSD1306_PAGE | pageAddress;
    ssd1306SendCommand(command, 1);

    command[0] = ((columnAddress >> 4) & 0x07) | SSD1306_PAGE_COL_LOW_NIBBLE;
    ssd1306SendCommand(command, 1);

    command[0] = (columnAddress & 0x07) | SSD1306_PAGE_COL_HIGH_NIBBLE;
    ssd1306SendCommand(command, 1);

    ssd1306SendData(buff, size);
}

void ssd1306AddCharToBuffer(uint16_t column, uint8_t page, uint8_t letter, uint8_t *buffer, uint8_t offset) {
    int x = 0;
    for (x = 0; x < 5; x++) {
        buffer[x + ((page * 128) + column)] = pgm_read_byte(font + ((letter - offset)*5) + x);
    }
    buffer[5 + ((page * 128) + column)] = 0;
}

void ssd1306AddStringToBuffer(uint16_t column, uint8_t page, char *string, uint8_t *buffer, uint8_t offset) {
    int y = 0;
    while (string[0] != 0) {
        ssd1306AddCharToBuffer(column + (6 * y), page, string[0], buffer, offset);
        string++;
        y++;
    }
}

void ssd1306AddCharToBufferDoubleSize(uint16_t column, uint8_t page, uint8_t letter, uint8_t *buffer, uint8_t offset) {
    int x = 0;
    char thisChar = 0;
    char doubleCharTop = 0;
    char doubleCharBottom = 0;
    for (x = 0; x < 5; x++) {
        doubleCharTop = 0;
        doubleCharBottom = 0;
        thisChar = pgm_read_byte(font + ((letter - offset)*5) + x);
        if ((thisChar & 0x01) == 0x01)
            doubleCharTop |= 0x03;
        if ((thisChar & 0x02) == 0x02)
            doubleCharTop |= 0x0c;
        if ((thisChar & 0x04) == 0x04)
            doubleCharTop |= 0x30;
        if ((thisChar & 0x08) == 0x08)
            doubleCharTop |= 0xc0;
        if ((thisChar & 0x10) == 0x10)
            doubleCharBottom |= 0x03;
        if ((thisChar & 0x20) == 0x20)
            doubleCharBottom |= 0x0c;
        if ((thisChar & 0x40) == 0x40)
            doubleCharBottom |= 0x30;
        if ((thisChar & 0x80) == 0x80)
            doubleCharBottom |= 0xc0;

        buffer[x * 2 + ((page * 128) + column)] = doubleCharTop;
        buffer[x * 2 + 1 + ((page * 128) + column)] = doubleCharTop;
        buffer[(x * 2 + ((page * 128) + column)) + 128] = doubleCharBottom;
        buffer[(x * 2 + 1 + ((page * 128) + column)) + 128] = doubleCharBottom;
    }
    buffer[10 + ((page * 128) + column)] = 0;
    buffer[(10 + ((page * 128) + column)) + 128] = 0;

}

void ssd1306AddStringToBufferDoubleSize(uint16_t column, uint8_t page, char *string, uint8_t *buffer, uint8_t offset) {
    int y = 0;
    while (string[0] != 0) {
        ssd1306AddCharToBufferDoubleSize(column + (11 * y), (page), string[0], buffer, offset);
        string++;
        y++;
    }
}

void ssd1306AddCharToBufferQuadSize(uint16_t column, uint8_t page, uint8_t letter, uint8_t *buffer, uint8_t offset) {
    int x = 0;
    int y = 0;
    char thisChar = 0;
    char quadCharTop, quadCharSecond, quadCharThird, quadCharBottom;
    for (x = 0; x < 5; x++) {
        quadCharTop = 0;
        quadCharSecond = 0;
        quadCharThird = 0;
        quadCharBottom = 0;
        thisChar = pgm_read_byte(font + ((letter - offset)*5) + x);
        if ((thisChar & 0x01) == 0x01)
            quadCharTop |= 0x0f;
        if ((thisChar & 0x02) == 0x02)
            quadCharTop |= 0xf0;
        if ((thisChar & 0x04) == 0x04)
            quadCharSecond |= 0x0f;
        if ((thisChar & 0x08) == 0x08)
            quadCharSecond |= 0xf0;
        if ((thisChar & 0x10) == 0x10)
            quadCharThird |= 0x0f;
        if ((thisChar & 0x20) == 0x20)
            quadCharThird |= 0xf0;
        if ((thisChar & 0x40) == 0x40)
            quadCharBottom |= 0x0f;
        if ((thisChar & 0x80) == 0x80)
            quadCharBottom |= 0xf0;
        for (y = 0; y < 4; y++) {
            buffer[(x * 4 + ((page * 128) + column)) + y] = quadCharTop;
            buffer[(x * 4 + ((page * 128) + column)) + 128 + y] = quadCharSecond;
            buffer[(x * 4 + ((page * 128) + column)) + 256 + y] = quadCharThird;
            buffer[(x * 4 + ((page * 128) + column)) + 384 + y] = quadCharBottom;
        }
    }
    buffer[20 + ((page * 128) + column)] = 0;
    buffer[(20 + ((page * 128) + column)) + 128] = 0;
    buffer[(20 + ((page * 128) + column)) + 256] = 0;
    buffer[(20 + ((page * 128) + column)) + 384] = 0;

}

void ssd1306AddStringToBufferQuadSize(uint16_t column, uint8_t page, char *string, uint8_t *buffer, uint8_t offset) {
    int y = 0;
    while (string[0] != 0) {
        ssd1306AddCharToBufferQuadSize(column + (21 * y), (page), string[0], buffer, offset);
        string++;
        y++;
    }
}
