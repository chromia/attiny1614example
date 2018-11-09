/*
 * t1614_spi_master.cpp
 *
 * Created: 2018/11/10
 * Author : chromia <chromia@outlook.jp>
 */

#include <avr/io.h>
#define F_CPU 3333333 /* 20MHz / 6(default prescale) */
#include <util/delay.h>

/* SPI as Master
 * Pin Assign
 * 2. PA4 : SS
 * 11.PA1 : MOSI
 * 12.PA2 : MISO
 * 13.PA3 : SCK
 * 7. PB2 : LED
 */
#define MOSI_bp 1
#define MOSI_bm (1 << MOSI_bp)
#define MISO_bp 2
#define MISO_bm (1 << MISO_bp)
#define SCK_bp 3
#define SCK_bm (1 << SCK_bp)
#define SS_bp 4
#define SS_bm (1 << SS_bp)
#define LED_bp 2
#define LED_bm (1 << LED_bp)

#define HIGH 1
#define LOW 0
#define DUMMY 0xFF

extern void setSS(uint8_t val);
extern uint8_t transfer(uint8_t sendData);

int main(void)
{
    // Port I/O Configuration
    PORTA.DIRSET = MOSI_bm | SCK_bm | SS_bm; // Output: MOSI/SCK/SS Input:MISO
    PORTB.DIRSET = LED_bm; // Output: LED

    // SPI Configuration
    SPI0.CTRLB =
        SPI_MODE_2_gc; // Mode2
    SPI0.CTRLA =
        SPI_MASTER_bm | // as a Master
        SPI_PRESC_DIV64_gc | // prescaler = CLK_PER/64
        SPI_ENABLE_bm; // Start

    /* 1-sec Interval LED Blinking */
    uint8_t count = 0;
    while (1)
    {
        count = transfer(count); // count up in Slave
        if(count >= 100) count = 0;
        if(count >= 50){
            PORTB.OUTSET = LED_bm;
        }else{
            PORTB.OUTCLR = LED_bm;
        }
        _delay_ms(10);
    }
}

void setSS(uint8_t val)
{
    if(val) PORTA.OUTSET = SS_bm;
    else PORTA.OUTCLR = SS_bm;
}

uint8_t transfer(uint8_t sendData)
{
    uint8_t recvData;

    setSS(LOW);
    // Send a byte
    SPI0.DATA = sendData;
    while((SPI0.INTFLAGS & SPI_RXCIE_bm) == 0);
    // Receive a byte
    SPI0.DATA = DUMMY;
    while((SPI0.INTFLAGS & SPI_RXCIE_bm) == 0);
    recvData = SPI0.DATA;
    setSS(HIGH);

    return recvData;
}
