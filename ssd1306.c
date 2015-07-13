#include <avr/io.h>
#include <util/delay.h>
#include "ssd1306.h"
#include "pfleury/i2cmaster.h"

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
    for(x=0;x<numCommands;x++) {
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
    for(x=0;x<dataSize;x++) {
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


void ssd1306DrawBuffer(uint8_t column_address, uint8_t page_address, uint8_t *buff) {
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
    ssd1306SendCommand(command,3);

    command[0] = SSD1306_SET_PAGE;
    command[2] = 7;
    ssd1306SendCommand(command,3);

    ssd1306SendData(buff, 1024);

}