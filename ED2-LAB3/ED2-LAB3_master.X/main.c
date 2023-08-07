// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
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

// LIBRERIAS
#include <xc.h>
#include <stdint.h>
#include "LCD8.h"
#include "map.h"
#include "SPI.h"

// VARIABLES GLOBALES
int slave_val;
int cont;
int adc_val;
int voltaje_int;
int adc_val2;
int voltaje_int2;
int unidades;
int decenas;
int centenas;
//const char data = 64;
//int cont = 0;

/*
 * Constantes
 */
#define _XTAL_FREQ 8000000
#define RS RD0
#define EN RD1
#define D0 RC0
#define D1 RC1
#define D2 RC2
#define D3 RC3
#define D4 RC4
#define D5 RC5
#define D6 RC6
#define D7 RC7

/*
 * Prototipos de funcion
 */
void setup(void);
void conversor(int num);
int int2char(int num);

void main(void) {
    setup();
    Lcd_Clear();
    while (1){
        PORTCbits.RC2 = 0;       //Slave Select 1 micro
       __delay_ms(1);
       
       spiWrite(0);
       __delay_ms(10);
       adc_val = spiRead();
       __delay_ms(10);
       
       spiWrite(1);
       __delay_ms(10);
       slave_val = spiRead();
       
       __delay_ms(1);
       PORTCbits.RC2 = 1;       //Slave Deselect 1 micro
       __delay_ms(10);
        
        //**********************************************************************
        PORTCbits.RC1 = 0;       //Slave Select 2 micro
       __delay_ms(1);
       
       spiWrite(PORTB);
       adc_val2 = spiRead();
       
       __delay_ms(1);
       PORTCbits.RC1 = 1;       //Slave Deselect 2 micro
       __delay_ms(10);
       
        //**********************************************************************
       
        voltaje_int = (int) mapRange(adc_val, 0, 255, 0.0, 500.0);
        conversor(voltaje_int);
        int centenas_char = int2char(centenas);
        int decenas_char = int2char(decenas);
        int unidades_char = int2char(unidades);
        char voltaje_texto[6] = {centenas_char, '.', decenas_char, unidades_char, 'V','\0'};
       
        voltaje_int2 = (int) mapRange(adc_val2, 0, 255, 0.0, 500.0);
        conversor(voltaje_int2);
        int centenas_char2 = int2char(centenas);
        int decenas_char2 = int2char(decenas);
        int unidades_char2 = int2char(unidades);
        char voltaje_texto2[6] = {centenas_char2, '.', decenas_char2, unidades_char2, 'V','\0'};
        
        cont = (int) mapRange(slave_val, 0, 255, 0.0, 255.0);
        conversor(cont);
        int centenas_char3 = int2char(centenas);
        int decenas_char3 = int2char(decenas);
        int unidades_char3 = int2char(unidades);
        char cont_texto[3] = {centenas_char3, decenas_char3, unidades_char3};
        
        Lcd_Set_Cursor(1,7);
        Lcd_Write_String("POT2:");
        Lcd_Set_Cursor(2,7);
        Lcd_Write_String(voltaje_texto2);
       
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("POT1:");
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(voltaje_texto);
        
//        char cont_char = (char) int2char(cont);
        
        Lcd_Set_Cursor(1,13);
        Lcd_Write_String("CON:");
//        Lcd_Set_Cursor(2,14);
//        Lcd_Write_Char('#');
        Lcd_Set_Cursor(2,13);
        Lcd_Write_String(cont_texto);
    }
    return;
}

//******************************************************************************
// FUNCIONES
//******************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0;
    PORTB = 0;
    
    TRISE = 0;
    PORTE = 0;
    
    TRISD = 0x00;
    PORTD = 0x00;
    
    // Configuracion oscilador interno
    OSCCONbits.IRCF = 0b111; // 8MHz
    OSCCONbits.SCS = 1;
   
    TRISC2 = 0;
    TRISC1 = 0;
    PORTCbits.RC2 = 1;
    PORTCbits.RC1 = 1;
    
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
    Lcd_Init();
}

void conversor (int num) {
    centenas = num / 100;
    num %= 100;
    decenas = num / 10;
    unidades = num % 10;
    return;
}

int int2char(int num){
    int val;
    switch (num) {
            case 0:
                val = 48; //0
                break;
            case 1:
                val = 49; //1
                break;
            case 2:
                val = 50; //2
                break;
            case 3:
                val = 51; //3
                break;
            case 4:
                val = 52; //4
                break;
            case 5:
                val = 53; //5
                break;
            case 6:
                val = 54; //6
                break;
            case 7:
                val = 55; //7
                break;
            case 8:
                val = 56; //8
                break;
            case 9:
                val = 57; //9
                break;
    }
    return val;
}