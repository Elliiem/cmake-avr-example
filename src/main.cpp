#include <avr/io.h>
#include <util/delay.h>

int main() {
    uint8_t mask = 0x80;

    /* on atmega2560, PWM pin 13 (LED pin) = port B bit 7 */
    DDRB |= mask; /* set port B pin 7 to output */

    while(1) {
        PORTB |= mask; /* set port B pin 7 to high */
        _delay_ms(100);
        PORTB &= ~mask; /* set port B pin 7 to low */
        _delay_ms(100);
    }

    return 0;
}
