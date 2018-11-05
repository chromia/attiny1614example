/*
 * t1614_timerA.cpp
 *
 * Created: 2018/10/27
 * Author : chromia <chromia@outlook.jp>
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

/* PIN Assign
 * 9.PB0 : LED
 */

#define PIN_LED_bp 0 // PB0
#define PIN_LED_bm (1 << PIN_LED_bp)

/* 1sec-interval LED blinking by TCA(Normal Mode)
 * Attiny1614's default clock is 20MHz(RC)
 * CLK_PER is 3.333MHz ( default prescaler rate is 6, then 20/6 == 3.3 )
 * 
 * TCA's freq = CLK_PER / (2*N*COMPARE)
 *  N is prescaler rate (1,2,4,8,16,64,256,1024)
 * if N=64, COMPARE = 26042  (3333333 / 2*64*26042 = 0.999987 [sec]) -> 1Hz
 */
#define COMPARE 26042

int main(void)
{	
	// LED
	PORTB.DIRSET = PIN_LED_bm;
	
	// TCA
	TCA0.SINGLE.CTRLB = 
		TCA_SINGLE_CMP0EN_bm | // Enables Compare Channel 0 (output port WO0 = PB0)
		TCA_SINGLE_WGMODE_FRQ_gc; // FRQ Waveform Generation
	TCA0.SINGLE.CMP0 = COMPARE;
	TCA0.SINGLE.CTRLA = 
		TCA_SINGLE_CLKSEL_DIV64_gc | // N=64
		TCA_SINGLE_ENABLE_bm; // Start
	
    while (1) 
    {
    }
}

