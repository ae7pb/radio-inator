#include <avr/io.h>
// for netbeans parsing
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "pfleury/i2cmaster.h"
#include "pfleury/uart.h"
#include "radio-inator.h"
#include "myLibraries/ssd1306.h"
#include "avr-softuart-master/softuart.h"



int main(void) {

    DDRC &= ~((1 << PINC4) | (1 << PINC5));
    PORTC |= (1 << PINC4) | (1 << PINC5);
    i2c_init();
    ssd1306Setup();
    uart_init(UART_BAUD_SELECT(9600, F_CPU));
    softuart_init();

    // yea interrupts
    sei();

    int x;

    unsigned char buffer[1024];
    for (x = 0; x < 1024; x++) {
        buffer[x] = 0xff;
    }


    unsigned char commands[10];
    commands[0] = SSD1306_SET_CONTRAST;
    commands[1] = 0xa0;
    ssd1306SendCommand(commands, 2);
    ssd1306AddStringToBufferQuadSize(0, 2, "106.62", buffer, 0);
    ssd1306AddStringToBufferDoubleSize(0, 6, "Farnsworth", buffer, 0);
    ssd1306DrawBuffer(0, 0, buffer);
    softuart_turn_rx_on();
    char gpsBuffer[401];
    unsigned char c;
    while (1) {
        x = 0;
        while (1) {
            c = softuart_getchar();
            if(softUartNoDataFlag == 1)
                continue;
            if(c == 13 || c == 10) { // cr
                gpsBuffer[x]=0;
                break;
            }
            gpsBuffer[x] = c;
            x++;            

        }
        gpsBuffer[400] = 0;
        ssd1306AddStringToBuffer(0,0,gpsBuffer,buffer,0);
        ssd1306DrawBuffer(0,0,buffer);
    }
    /*
    // Override Default
    i2c_write(SSD1306_SET_ADDRESSING);
    i2c_write(SSD1306_PAGE_ADDR);
    
    // Page address start at left
    i2c_write(SSD1306_PAGE_COL_LOW_NIBBLE | 0x00);
    i2c_write(SSD1306_PAGE_COL_HIGH_NIBBLE | 0x00);
    i2c_write(SSD1306_PAGE | 0x00);
    //i2c_write(0x40);
    i2c_start(SSD1063_ADDRESS+I2C_WRITE);
    for(int x=0; x<8; x++) {
        i2c_write(SSD1306_PAGE | x);
        for (int y = 0; y < 128; y ++) {
            i2c_write(15);
        }
    }
     */
}

