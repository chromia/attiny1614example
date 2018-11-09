/*
 * t1614_usart.cpp
 *
 * Created: 2018/10/26
 * Author : chromia <chromia@outlook.jp>
 */

#include <avr/io.h>
#define F_CPU 3333333
#include <util/delay.h>
#include <avr/interrupt.h>

/* Pin Assign
 * 6. PB3(RxD) : target TxD
 * 7. PB2(TxD) : target RxD
 */

#define PIN_TX_bp 2 // TX = PB2
#define PIN_TX_bm (1 << PIN_TX_bp)
#define PIN_RX_bp 3 // RX = PB3
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

int main(void)
{
    // USART Configuration
    PORTB.DIRSET = PIN_TX_bm; // TX as OUTPUT, RX as INPUT
    PORTB.OUTSET = PIN_TX_bm; // TX is high
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

    sei();

    while (1)
    {
        // Check if TX buffer is empty
        if(USART0.STATUS & USART_DREIF_bm){
            USART0.TXDATAL = code; // Transmit a byte
        }
        _delay_ms(1000);
    }
}

ISR(USART0_RXC_vect)
{
    //Receive a byte
    code = USART0.RXDATAL;
}
