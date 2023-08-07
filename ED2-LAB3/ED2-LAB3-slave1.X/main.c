//*****************************************************************************
// Palabra de configuraci�n
//*****************************************************************************
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definici�n e importaci�n de librer�as
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include "SPI.h"
#include "adc.h"
#include "Interrupciones.h"
//*****************************************************************************
// Definici�n de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000

uint8_t temporal = 0;
uint8_t adc_val;
uint8_t cont;
uint8_t selector;   


void setup(void);
//*****************************************************************************
// C�digo de Interrupci�n 
//*****************************************************************************
void __interrupt() isr(void){
    if(INTCONbits.RBIF)
    {
        if (PORTBbits.RB0 == 0){
            PORTD++;
            cont++;
        }
        
        if (PORTBbits.RB1 == 0){
            PORTD--;
            cont--;
        }
        
        INTCONbits.RBIF = 0;
    }
    
    if (PIR1bits.ADIF){// Si se activa la bandera de interupci�n del adc
        adc_val = (uint8_t) adc_read();
//        PORTD = adc_read();
        __delay_ms(10);
        PIR1bits.ADIF = 0; //Limpiar la bandera de la interrupcion del ADC
    }
    
    if(SSPIF == 1){
        selector = spiRead();
        spiWrite(selector == 1 ? adc_val : cont);
        SSPIF = 0;
    }
}
//*****************************************************************************
// C�digo Principal
//*****************************************************************************
void main(void) {
    setup();
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){
//       PORTB--;
//       __delay_ms(250);
        ADCON0bits.GO = 1;
    }
    return;
}
//*****************************************************************************
// Funci�n de Inicializaci�n
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0;
    TRISD = 0;
    
    PORTB = 0;
    PORTD = 0;
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupci�n MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupci�n MSSP
    TRISAbits.TRISA5 = 1;       // Slave Select
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
    ioc_init(0);
    ioc_init(1); 
    
    adc_init(0);
}