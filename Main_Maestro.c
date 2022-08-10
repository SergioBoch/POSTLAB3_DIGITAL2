/*
 * File:   Main_Maestro.c
 * Author: Sergio Alejandro Boch Ixén
 *
 * Created on 8 de agosto de 2022, 12:07 PM
 */
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
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

#include <xc.h>
#include <stdint.h> 
#include <stdio.h>
#include "SPI.h" 
#include "Oscilador.h"
#include "LCD.h"
#include "TMR0.h"
#define _XTAL_FREQ 1000000
#define RS RC6
#define EN RC7
#define IN_MIN 0
#define IN_MAX 1023 
#define OUT_MIN 0
#define OUT_MAX 500

uint16_t ADRESH1= 0;
uint16_t ADRESH2= 0;
uint16_t ADRESL1 = 0;
uint16_t ADRESL2 = 0;
uint8_t i = 0;
uint8_t VALOR_DESC = 0;
uint16_t ADREST1 = 0;
uint16_t ADREST2 = 0;
uint16_t MAP1;
uint16_t MAP2;
int UNI;
int DECI;
int UNI2;
int DECI2;
char VALORES1[10];
char VALORES2[10];
uint8_t CONT_TMR0 = 0;
uint8_t MOSTRAR = 0;

void __interrupt() isr (void){
    if(INTCONbits.T0IF){
        CONT_TMR0++;                       
        if (CONT_TMR0 == 10){      
            MOSTRAR = 1;                  
            CONT_TMR0 = 0;                  
        }
        tmr0_reload();
    }
    return;
}


void main(void) {
    int_osc_MHz(1);
    setup();
    tmr0_init(255);
    tmr0_reload();
    unsigned int a;
    Lcd8_Init();
    while(1){
        if (MOSTRAR){
            PORTEbits.RE0 = 0;          
            __delay_ms(1); 
            spiWrite(72);               
            if (i == 0){                
                VALOR_DESC = spiRead(); 
                i = 1;                  
                __delay_ms(1); 
                PORTEbits.RE0 = 0;   
            } else {
                ADRESL2 = spiRead();    
                __delay_ms(1); 
                PORTEbits.RE0 = 0;      
            }
            __delay_ms(100);

            ADREST1 = ((ADRESH1 << 2) + ADRESL1);
            ADREST2 = ((ADRESH2 << 2) + ADRESL2);
            MAP1 = (uint16_t)(map(ADREST1, IN_MIN, IN_MAX, OUT_MIN, OUT_MAX));
            UNI = (MAP1/100);
            DECI = (MAP1 - UNI*100);
            sprintf(VALORES1, "%d.%d V", UNI, DECI);

            MAP2 = (uint16_t)(map(ADREST2, IN_MIN, IN_MAX, OUT_MIN, OUT_MAX));
            UNI2 = (MAP2/100);
            DECI2 = (MAP2 - UNI2*100);
            sprintf(VALORES2, "%d.%d V", UNI2, DECI2);
            MOSTRAR = 0;
        }
        Lcd8_Set_Cursor(1,1);
        Lcd8_Write_String("S1:      S2:");
        __delay_ms(100);
        Lcd8_Set_Cursor(2,1);
        Lcd8_Write_String(VALORES1);
        Lcd8_Set_Cursor(2,10);
        Lcd8_Write_String(VALORES2);
        __delay_ms(100);
    }
    return;
}

void setup(void){       
 
    ANSEL = 0b00000000;         // I/O digitales
    ANSELH = 0b00000000;        // I/O digitales
    TRISD = 0b00000000;         // PORTD como salida
    TRISEbits.TRISE0 = 0;       // RE0 como salida para SS
    PORTD = 0b00000000;         // Limpiar PORTD
    PORTE = 0b0001;             // Limpiar PORTE
    TRISCbits.TRISC6 = 0; //RC6 salida
    TRISCbits.TRISC7 = 0; //RC7 salida
    PORTCbits.RC6 = 0;
    PORTCbits.RC7 = 0;
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitar interrupciones de perifericos
    return;
}

