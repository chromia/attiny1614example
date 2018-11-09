/*
 * t1614_adc.cpp
 *
 * Created: 2018/11/04
 * Author : chromia <chromia@outlook.jp>
 */

#include <avr/io.h>
#define F_CPU 3333333 /* 20MHz / 6(default prescale) */
#include <util/delay.h>
#include <avr/interrupt.h>

/* Pin Assign
 * 3.PA5 : LED output (TCB0 WO)
 * 4.PB6 : ADC input
 */
#define LED_bp 5 //PB5
#define LED_bm (1<<LED_bp)
#define ADC_CTRL PORTB.PIN1CTRL
#define AIN6 6

int main(void)
{
    // LED Luminance Control by PWM
    // Potentiometer(PB1) -> ADC -> TCB(PWM) -> LED(PB5)

    // Port Configuration
    PORTA.DIRSET = LED_bm;
    PORTA.OUTCLR = LED_bm;
    ADC_CTRL &= ~PORT_ISC_gm;
    ADC_CTRL |= PORT_ISC_INPUT_DISABLE_gc; // Disable Digital Input Buffer

    // VRef Configuration
    VREF.CTRLA = VREF_ADC0REFSEL_4V34_gc; // VRef == 4.34V

    // ADC Configuration
    ADC0.CTRLB = ADC_SAMPNUM_ACC1_gc; // No accumulation
    ADC0.CTRLC = ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV16_gc; // Internal VRef, CLK_ADC=CLK_PER/16
    ADC0.MUXPOS = AIN6; // select target pin
    ADC0.CTRLA = ADC_RESSEL_8BIT_gc | ADC_ENABLE_bm; // 8-bit mode

    // TCB Configuration
    TCB0.CTRLB = TCB_CNTMODE_PWM8_gc | TCB_CCMPEN_bm; // 8-bit PWM mode, Enable Output to I/O pin
    TCB0.CTRLA = !TCB_ENABLE_bm; // not start yet

    volatile uint8_t adcval = 0;
    while (1)
    {
        if(ADC0.INTFLAGS & ADC_RESRDY_bm){
            // Retrieve converted ADC value
            adcval = ADC0.RES;
            // Update PWM duty rate
            TCB0.CTRLA &= ~TCB_ENABLE_bm;
            TCB0.CCMP = (adcval << 8) | 0xFF;
            TCB0.CNT = 0;
            TCB0.CTRLA |= TCB_ENABLE_bm;
        }
        _delay_ms(100);
        // Start A/D Conversion
        ADC0.COMMAND |= ADC_STCONV_bm;
    }
}
