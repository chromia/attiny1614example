/*
 * t1614_led.cpp
 *
 * Created: 2018/11/03
 * Author : chromia <chromia@outlook.jp>
 */

#include <avr/io.h>
#define F_CPU 3333333 /* 20MHz / 6(default prescale) */
#include <util/delay.h>

/* Pin Assign
 * 7.PB2 : LED output
 */
#define LED_bp 2 //PB2
#define LED_bm (1<<LED_bp)

int main(void)
{
    // Port Configuration
    PORTB.DIRSET = LED_bm; // equals PORTB.DIR |= LED_bm;
    PORTB.OUTCLR = LED_bm; // equals PORTB.OUT &= ~LED_bm;

    while (1)
    {
        // Blinking the LED
        _delay_ms(500);
        if(PORTB.OUT & LED_bm){
            PORTB.OUTCLR = LED_bm;
        }else{
            PORTB.OUTSET = LED_bm;
        }
    }
}
