#include <avr/io.h>
// for netbeans parsing
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
    //ssd1306AddStringToBufferQuadSize(0, 2, "106.62", buffer, 0);
    //ssd1306AddStringToBufferDoubleSize(0, 6, "Farnsworth", buffer, 0);
    ssd1306DrawBuffer(0, 0, buffer);
    softuart_turn_rx_on();
    char gpsBuffer[64];
    unsigned char c;
    int longDeg = 0;
    int longMin = 0;
    long longSec = 0;
    int longSecHigh = 0;
    int longSecDecimal = 0;
    char longNS[2];
    int latDeg = 0;
    int latMin = 0;
    long latSec = 0;
    int latSecHigh = 0;
    int latSecDecimal = 0;
    int day = 0;
    int month = 0;
    int year = 0;
    char latEW[2];
    unsigned long time = 0; // seconds in the day
    char numSatellites[3];
    char tinyString[] = "0";
    int thisField = 0;
    long thisNumber = 0;
    char tempString[20];
    char tempString2[25];
    char tempSmallString[6];
    char tempSmallString2[6];
    char afterDecimal = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    longNS[0] = 0;
    latEW[0] = 0;
    tempString2[0] = 0;
    while (1) {
        x = 0;
        gpsBuffer[0] = 0;
        while (1) {
            c = softuart_getchar();
            if (softUartNoDataFlag == 1)
                continue;
            if (c == 13 || c == 10) { // cr
                gpsBuffer[x] = 0;
                break;
            }
            gpsBuffer[x] = c;
            x++;
            if (x == 63) {
                gpsBuffer[x] = 0;
                break;
            }
        }
        // $GPRMC has different fields so we need to have a way to deal with both of them
        int gprmcAdder = 0;
        if (strstr(gpsBuffer, "$GPGGA") || strstr(gpsBuffer, "$GPRMC")) {
            if (strstr(gpsBuffer, "$GPRMC")) {
                gprmcAdder = 1;
            }
            //if(gprmcAdder == 0)  ssd1306AddStringToBuffer(0, 0, gpsBuffer, buffer, 0);
            tempString[0] = 0;
            thisField = 0;
            // $GPGGA,043755.00,39xx.xxxxx,N,111xx.xxxxx,W,1,07,1.20,1582.4,M,-19.3,M,,*5A
            // field 0 is id, field 1 is time HHMMSS, field 2 is latitude, field 3 N/S
            // field 4 longitude, field 5 E/W., field 5 # satellites Don't care about rest
            for (int x = 0; x < strlen(gpsBuffer); x++) {
                if (gpsBuffer[x] == ',') {
                    // Decide which field this is
                    switch (thisField) {
                        case 1:
                            if(gprmcAdder == 1)
                                break;
                            tempSmallString[0] = tempString[0];
                            tempSmallString[1] = tempString[1];
                            tempSmallString[2] = 0;
                            thisNumber = atol(tempSmallString);
                            time = thisNumber * 60 * 60;
                            tempSmallString[0] = tempString[2];
                            tempSmallString[1] = tempString[3];
                            thisNumber = atol(tempSmallString);
                            time = time + (thisNumber * 60);
                            tempSmallString[0] = tempString[4];
                            tempSmallString[1] = tempString[5];
                            thisNumber = atol(tempSmallString);
                            time = time + thisNumber;
                            break;
                        case 2:
                            if(gprmcAdder == 1)
                                break;
                            afterDecimal = 0;
                            tempSmallString[0] = 0;
                            tempSmallString2[0] = 0;
                            for (int y = 0; y < strlen(tempString); y++) {
                                if (tempString[y] != '.') {
                                    tinyString[0] = tempString[y];
                                    if (afterDecimal == 0)
                                        strncat(tempSmallString, tinyString, 6);
                                    else
                                        strncat(tempSmallString2, tinyString, 6);
                                } else {
                                    afterDecimal = 1;
                                }
                            }
                            thisNumber = atol(tempSmallString);
                            if(thisNumber > 0) {
                            latDeg = thisNumber / 100;
                            latMin = thisNumber % 100;
                            tempSmallString[0] = tempSmallString2[0];
                            tempSmallString[1] = tempSmallString2[1];
                            tempSmallString[2] = tempSmallString2[2];
                            tempSmallString[3] = tempSmallString2[3];
                            tempSmallString[4] = tempSmallString2[4];
                            tempSmallString[5] = 0;
                            latSec = atof(tempSmallString);
                            // right now seconds are really decimal minutes * 1000
                            latSec = latSec * 60 / 100;                            
                            latSecHigh = latSec / 1000;
                            latSecDecimal = latSec % 1000;
                            }
                            break;
                        case 3:
                            if(gprmcAdder == 1)
                                break;
                            latEW[0] = tempString[0];
                            latEW[1] = 0;
                            break;
                        case 4:
                            if(gprmcAdder == 1)
                                break;
                            afterDecimal = 0;
                            tempSmallString[0] = 0;
                            tempSmallString2[0] = 0;
                            for (int y = 0; y < strlen(tempString); y++) {
                                if (tempString[y] != '.') {
                                    tinyString[0] = tempString[y];
                                    if (afterDecimal == 0)
                                        strncat(tempSmallString, tinyString, 6);
                                    else
                                        strncat(tempSmallString2, tinyString, 6);
                                } else {
                                    afterDecimal = 1;
                                }
                            }
                            thisNumber = atol(tempSmallString);
                            if(thisNumber > 0) {
                            longDeg = thisNumber / 100;
                            longMin = thisNumber % 100;
                            tempSmallString[0] = tempSmallString2[0];
                            tempSmallString[1] = tempSmallString2[1];
                            tempSmallString[2] = tempSmallString2[2];
                            tempSmallString[3] = tempSmallString2[3];
                            tempSmallString[4] = tempSmallString2[4];
                            tempSmallString[5] = 0;
                            longSec = atof(tempSmallString);
                            // right now seconds are really decimal minutes * 1000
                            longSec = longSec * 60 / 100;
                            
                            longSecHigh = longSec / 1000;
                            longSecDecimal = longSec % 1000;
                            }
                            break;
                        case 5:
                            if(gprmcAdder == 1)
                                break;
                            longNS[0] = tempString[0];
                            longNS[1] = 0;
                            break;
                        case 7:
                            if(gprmcAdder == 1)
                                break;
                            strcpy(numSatellites, tempString);
                            break;
                        case 9:
                            if(gprmcAdder == 0)
                                break;
                            tempSmallString[0] = tempString[0];
                            tempSmallString[1] = tempString[1];
                            tempSmallString[2] = 0;
                            day = atoi(tempSmallString);
                            tempSmallString[0] = tempString[2];
                            tempSmallString[1] = tempString[3];
                            month = atol(tempSmallString);
                            tempSmallString[0] = tempString[4];
                            tempSmallString[1] = tempString[5];
                            year = atol(tempSmallString);
                            break;
                        default:
                            break;
                    }
                    thisField++;
                    tempString[0] = 0;
                } else {
                    tinyString[0] = gpsBuffer[x];
                    strcat(tempString, tinyString);
                }

            }
            hour = (time / 60 / 60);
            minute = ((time / 60) - (hour * 60) );
            second = (time % 60);
            snprintf(tempString2, 21, "20%02i-%02i-%02i %02i:%02i:%02i",year,month,day,hour,minute,second);
            ssd1306AddStringToBuffer(0, 3, tempString2, buffer, 0);
            snprintf(tempString2,24,"Lat:%i%c%i\'%i.%u\"%s",latDeg,0x1f,latMin,latSecHigh,latSecDecimal,latEW);
            ssd1306AddStringToBuffer(0, 4, tempString2, buffer, 0);
            snprintf(tempString2,24,"Long:%i%c%i\'%i.%i\"%s",longDeg,0x1f,longMin,longSecHigh,longSecDecimal,longNS);
            ssd1306AddStringToBuffer(0, 5, tempString2, buffer, 0);
            ssd1306AddStringToBuffer(0, 6, "#Sat:", buffer, 0);
            ssd1306AddStringToBuffer(5 * 6, 6, numSatellites, buffer, 0);
            ssd1306DrawBuffer(0, 0, buffer);
        }
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

