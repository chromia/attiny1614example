/*
 * t1614_rtc.cpp
 *
 * Created: 2018/11/03
 * Author : chromia <chromia@outlook.jp>
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

/* PIN Assign
 * 1.VCC   : 5V
 * 6.TOSC1 : External Xtal 32.768KHz
 * 7.TOSC2 : External Xtal 32.768KHz
 * 11.PA1  : USART TX
 * 12.PA2  : USART RX
 * 14.GND  :
 */

#define PIN_TX_bp 1 // TX = PA1(Alt)
#define PIN_TX_bm (1 << PIN_TX_bp)
#define PIN_RX_bp 2 // RX = PA2(Alt)
#define PIN_RX_bm (1 << PIN_RX_bp)

/* Baud Rate Calculation
 * Attiny1614's default clock is 20MHz(RC)
 * CLK_PER is 3.333MHz ( default prescaler rate is 6, then 20/6 == 3.3 )
 *
 * Requested Baud Rate: f_baud = 9600[bps]
 * register parameter: BAUD = floor((64 * CLK_PER ) / (N * f_baud))
 *  N = 16 (asynchronous mode)
 *  -> BAUD = 1389
 */
#define PARAM_BAUD 1389

volatile uint8_t code = 'x';

#define USE_PIT 0  /* 1:Use PIT Function, 0:Use RTC Function */

int main(void)
{	
	// USART Configuration
	PORTMUX.CTRLB |= PORTMUX_USART0_ALTERNATE_gc; // Alternative Pins
	PORTA.DIRSET = PIN_TX_bm; // TX as OUTPUT, RX as INPUT
	PORTA.OUTSET = PIN_TX_bm; // TX is high
	USART0.BAUD = PARAM_BAUD; // Set Baud Rate
	USART0.CTRLC = 
		USART_CMODE_ASYNCHRONOUS_gc | // Mode: Asynchronous[default]
		USART_PMODE_DISABLED_gc | // Parity: None[default]
		USART_SBMODE_1BIT_gc | // StopBit: 1bit[default]
		USART_CHSIZE_8BIT_gc; // CharacterSize: 8bit[default]
	USART0.CTRLA = 
		USART_RXCIE_bm | // Enable RX interrupt
		!USART_TXCIE_bm; // Disable TX interrupt
	USART0.CTRLB = 
		USART_RXEN_bm | // Start Receiver  
		USART_TXEN_bm | // Start Transmitter
		USART_RXMODE_NORMAL_gc; // Receiver mode is Normal USART & 1x-speed
	
	// CLKCTRL Configuration
	_PROTECTED_WRITE(CLKCTRL_XOSC32KCTRLA, CLKCTRL_ENABLE_bm); // Enable External Oscillator(32.768kHz Xtal)
	
	// RTC Configuration
	RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc; // Use 32.768kHz clock as CLK_RTC
#if USE_PIT
	// 1[sec]-interval interrupt(PIT version) - simple
	RTC.PITINTCTRL = RTC_PI_bm; // Enable PIT Interrupts
	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm; // Run (interval 1[sec])
#else
	// 1[sec]-interval interrupt(RTC version) - more flexible
	RTC.INTCTRL = RTC_OVF_bm;
	RTC.PER = 0x8000; // Loop every 32768 clocks
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm; // Not Prescaled | Run
#endif
			
	sei();
	
    while (1) 
    {
    }
}

ISR(USART0_RXC_vect)
{
	//Receive a byte
	code = USART0.RXDATAL;
}

void on_tick()
{
	// Check if TX buffer is empty
	if(USART0.STATUS & USART_DREIF_bm){
		USART0.TXDATAL = code; // Transmit a byte
	}
}

ISR(RTC_CNT_vect)
{
	on_tick();
	RTC.INTFLAGS |= RTC_OVF_bm;
}

ISR(RTC_PIT_vect)
{
	on_tick();
	RTC.PITINTFLAGS |= RTC_PI_bm;
}
