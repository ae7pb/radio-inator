#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "pfleury/i2cmaster.h"
#include "myFont.h"
#include "radio-inator.h"

#define SSD1063_ADDRESS 0x78

void error(void) {
    while(1) {
        PORTB |= (1 << PINB5);
        _delay_ms(500);
        PORTB &= ~(1 << PINB5);
        _delay_ms(500);
    }
}


int main(void) {
    unsigned char returnValue;
    DDRB |= (1 << PINB5);
    PORTB |= (1 << PINB5);
    DDRC &= ~((1 << PINC4) | (1 << PINC5));
    PORTC |= (1 << PINC4) | (1 << PINC5);
    i2c_init();
    returnValue = i2c_start(SSD1063_ADDRESS+I2C_WRITE);
    if( returnValue ) {
        i2c_stop();
        error();
        return 0;
    }
    
}
    
