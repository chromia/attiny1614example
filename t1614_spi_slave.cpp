/*
 * t1614_spi_slave.cpp
 *
 * Created: 2018/11/10
 * Author : chromia <chromia@outlook.jp>
 */

#include <avr/io.h>

/* SPI as Slave
 * Pin Assign
 * 2. PA4 : SS
 * 11.PA1 : MOSI
 * 12.PA2 : MISO
 * 13.PA3 : SCK
 */
#define MOSI_bp 1
#define MOSI_bm (1 << MOSI_bp)
#define MISO_bp 2
#define MISO_bm (1 << MISO_bp)
#define SCK_bp 3
#define SCK_bm (1 << SCK_bp)
#define SS_bp 4
#define SS_bm (1 << SS_bp)

#define DUMMY 0xFF

int main(void)
{
    // Port I/O Configuration
    PORTA.DIRSET = MISO_bm; // Output:MISO Input:MOSI/SCK/SS

    // SPI Configuration
    SPI0.CTRLB =
        SPI_MODE_2_gc; // Mode2
    SPI0.CTRLA =
        !SPI_MASTER_bm | // as Slave
        SPI_ENABLE_bm; // Start

    /* Simple count-up responder */
    while (1)
    {
        if(SPI0.INTFLAGS & SPI_RXCIF_bm){
            uint8_t count = SPI0.DATA; //read received data
            if(count != DUMMY){
                count++; // count up
                SPI0.DATA = count; // respond to master
            }
        }
    }
}
