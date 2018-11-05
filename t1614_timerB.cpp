/*
 * t1614_timerB.cpp
 *
 * Created: 2018/10/25
 * Author : chromia <chromia@outlook.jp>
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

/* PIN Assign
 * 5.PA7  : LED
 */

#define PIN_LED_bp 7 // PA7
#define PIN_LED_bm (1 << PIN_LED_bp)

/* 1sec-interval LED blinking by TCB(Periodic Interrupt Mode)
 * Attiny1614's default clock is 20MHz(RC)
 * CLK_PER is 3.333MHz ( default prescaler rate is 6, then 20/6 == 3.3 )
 * 
 * interrupt interval = 1ms ( 3333 / 3.333MHz = 0.001sec )
 * 500ms ON -> 500ms OFF
 *
 * If you want to write more smart code, 
 * please use TCA which has an internal prescaler.
 */
#define COMPARE 3333
#define LOOP 500

int main(void)
{
	// LED
	PORTA.DIRSET = PIN_LED_bm;
	
	// TCB0
	TCB0.CCMP = COMPARE;
	TCB0.INTCTRL = TCB_CAPT_bm;
	TCB0.CTRLA = TCB_ENABLE_bm;
	
	sei();
	
    while (1) 
    {
	}
}

volatile uint16_t count = 0;
ISR(TCB0_INT_vect)
{
	count++;
	if(count == LOOP){
		count = 0;
		if(PORTA.OUT & PIN_LED_bm){
			PORTA.OUTCLR = PIN_LED_bm;
		}else{
			PORTA.OUTSET = PIN_LED_bm;
		}
	}
	TCB0.INTFLAGS |= TCB_CAPT_bm; //clear the interrupt flag(to reset TCB0.CNT)
}
