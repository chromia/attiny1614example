/*
 * t1614_twi.cpp
 *  I2C Communication Example
 *    with ACM0802C-NLW-BBW-IIC(8x2 character LCD)
 *
 * Created: 2018/10/30
 * Author : chromia <chromia@outlook.jp>
 */ 

#include <avr/io.h>
#define F_CPU 20000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

/* Pin Assign
 * ATTiny1614
 * 8. PB1 -> SDA
 * 9. PB0 -> SCL
 *
 * ACM0802C
 * 1. Vss : GND
 * 2. Vdd : +5V
 * 3. Vo  : Potentiometer(10Kohm)
 * 4. SA1 : GND
 * 5. SA0 : GND
 * 6. SDA : 1614_PB1 (&PullUp)
 * 7. SCL : 1614_PB0 (&PullUp)
 * 8. NC
 * 9. BL+ : +5V
 * 10.BL- : GND
 */


/* TWI Baud Rate Calculation
 * CLK_PER is 20MHz
 * Target is ACM0802C I2C(8x2 Character LCD Module)
 *
 * Requested Baud Rate: f_SCL = 400[KHz]
 * register parameter: BAUD = (CLK_PER - f_SCL*(10+CLK_PER*t_RISE)) / (2*f_SCL)
 *  t_RISE = 300[ns] = 3.0e-7[s]
 *   -> BAUD = 17
 */
#define TWI_PARAM_BAUD 17

#define TWI_BUFFER_SIZE 3
uint8_t twi_buffer[TWI_BUFFER_SIZE];
volatile uint8_t *twi_ptr;
volatile uint8_t twi_rest;
volatile bool twi_sending = false;
volatile bool twi_error = false;

#define LCD_ADDR 0x78    // 01111[0][0][0]  <- SA1 SA0 W
#define LCD_ADDR_R 0x79  // 01111[0][0][1]  <- SA1 SA0 R
#define LCD_COMMAND 0x00 // [0][0]000000    <- LastByte RS
#define LCD_DATA 0x40    // [0][1]000000


bool TWI_init()
{
	// TWI Configuration
	TWI0.MBAUD = TWI_PARAM_BAUD;
	TWI0.MCTRLA = TWI_WIEN_bm | TWI_ENABLE_bm;
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);	
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
	return true;
}

bool TWI_sendMessage(uint8_t address, const uint8_t *message, uint8_t length)
{
	// Copy Message Data
	if(length > TWI_BUFFER_SIZE) return false;
	memcpy(twi_buffer, message, length);
	twi_ptr = twi_buffer;
	twi_rest = length;
	
	twi_sending = true;
	
	// Send Slave Address(Start Sending)
	TWI0.MADDR = address;
	
	// Wait until finished
	while(twi_sending);
	
	return !twi_error;
}

ISR(TWI0_TWIM_vect)
{
	if(TWI0.MSTATUS & TWI_RXACK_bm){
		twi_error = true;
		twi_sending = false;
	}
	if(twi_rest){
		// Send Next Character
		--twi_rest;
		TWI0.MSTATUS |= (TWI_WIF_bm || TWI_RIF_bm);
		TWI0.MDATA = *twi_ptr++;
	}else{
		// Generate STOP Condition
		TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
		TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
		twi_sending = false;
	}
}

bool LCD_setMessage(uint8_t address, uint8_t cont, uint8_t data)
{
	uint8_t msg[] = { cont, data };
	return TWI_sendMessage(address, msg, 2);
}

int main(void)
{	
	// System Clock
	_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0); // Disable prescaler ( CLK_PER=20MHz )
	
	_delay_ms(40); // Wait until the LCD is ready
	
	TWI_init();
	
	sei();
	
	// Send Commands
	LCD_setMessage(LCD_ADDR, LCD_COMMAND, 0x01); // Clear Display
	_delay_ms(2);
	LCD_setMessage(LCD_ADDR, LCD_COMMAND, 0x38); // Function Set : 8bitI/F, 2Line, 5x8Font 
	_delay_us(50);
	LCD_setMessage(LCD_ADDR, LCD_COMMAND, 0x0C); // Display ON/OFF : display=ON, cursor=OFF, blink=OFF
	_delay_us(50);
	LCD_setMessage(LCD_ADDR, LCD_COMMAND, 0x06); // Entry Mode Set : Step to Right
	_delay_us(50);
	// Write Data
	LCD_setMessage(LCD_ADDR, LCD_DATA, 0x48);    // Write Data : 'H'
	_delay_ms(2);
	LCD_setMessage(LCD_ADDR, LCD_DATA, 0x45);    // Write Data : 'E'
	_delay_ms(2);
	LCD_setMessage(LCD_ADDR, LCD_DATA, 0x4C);    // Write Data : 'L'
	_delay_ms(2);
	LCD_setMessage(LCD_ADDR, LCD_DATA, 0x4C);    // Write Data : 'L'
	_delay_ms(2);
	LCD_setMessage(LCD_ADDR, LCD_DATA, 0x4F);    // Write Data : 'O'
	_delay_ms(2);
	
	while(1)
	{
		
	}
}

