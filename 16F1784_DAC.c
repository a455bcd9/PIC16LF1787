//  Software License Agreement
//
// The software supplied herewith by Microchip Technology Incorporated (the "Company")
// for its PICmicro® Microcontroller is intended and supplied to you, the Company?s
// customer, for use solely and exclusively on Microchip PICmicro Microcontroller
// products.
//
// The software is owned by the Company and/or its supplier, and is protected under
// applicable copyright laws. All rights are reserved. Any use in violation of the
// foregoing restrictions may subject the user to criminal sanctions under applicable
// laws, as well as to civil liability for the breach of the terms and conditions of
// this license.
//
// THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, WHETHER EXPRESS,
// IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
// COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

//**********************************************************************************
// This very simple program shows how to set up and use the DAC on the
// PIC16F1784.  This example also shows how to set up the Fixed Voltage Reference (FVR)
// for use as the positive voltage reference for the DAC.
//*********************************************************************************** 
// Device: PIC16F1784
// Compiler: Microchip XC8 v1.12
// IDE: MPLAB X v1.7
// Created: May 2013
// 
// **********************************************************************************
#include <xc.h> // include standard header file

// set Config bits
#pragma config FOSC=INTOSC, PLLEN=OFF, MCLRE=ON, WDTE=OFF
#pragma config LVP=OFF, CLKOUTEN=OFF,


// Definitions
#define _XTAL_FREQ  16000000  // this is used by the __delay_ms(xx) and __delay_us(xx) functions

//*************************************************************************************

void main() {
    
    unsigned int i;

    // set up oscillator control register for internal clock running at 16Mhz
    OSCCONbits.SCS = 0x02; //set the SCS bits to select internal oscillator block
    OSCCONbits.IRCF = 0x0F; //set OSCCON IRCF bits to select OSC frequency=16Mhz
    
    OPTION_REGbits.nWPUEN = 0; // enable weak pullups (each pin must be enabled individually)

    // PORT A Assignments
    TRISAbits.TRISA0 = 0; // RA0 = nc
    TRISAbits.TRISA1 = 1; // RA1 = nc
    TRISAbits.TRISA2 = 0; // RA2 = DAC1 Output 1
    TRISAbits.TRISA3 = 0; // RA3 = nc
    TRISAbits.TRISA4 = 0; // RA4 = nc
    TRISAbits.TRISA5 = 0; // RA5 = nc
    TRISAbits.TRISA6 = 0; // RA6 = nc
    TRISAbits.TRISA7 = 0; // RA7 = nc

    ANSELA = 0x00; // all port A pins are digital I/O

    // if you want to enable weak pullups on some individual pins...here's how
    // WPUAbits.WPUA6 = 1;     // enable weak pullup on RA6
    //WPUAbits.WPUA7 = 1;     // enable weak pullup on RA7


    // PORT B Assignments
    TRISBbits.TRISB0 = 0; // RB0 = nc
    TRISBbits.TRISB1 = 0; // RB1 = nc
    TRISBbits.TRISB2 = 0; // RB2 = nc
    TRISBbits.TRISB3 = 0; // RB3 = nc
    TRISBbits.TRISB4 = 0; // RB4 = nc
    TRISBbits.TRISB5 = 0; // RB5 = nc
    TRISBbits.TRISB6 = 0; // RB6 = nc
    TRISBbits.TRISB7 = 0; // RB7 = DAC1 Output 2

    ANSELB = 0x00; // all port B pins are digital I/O

    // if you want to enable weak pullups on some individual pins...
    //WPUBbits.WPUB2 = 1;     // enable weak pullup on RB2
    //WPUBbits.WPUB3 = 1;     // enable weak pullup on RB3

    //	PORT C Assignments
    TRISCbits.TRISC0 = 0; // RC0 = nc
    TRISCbits.TRISC1 = 0; // RC1 = nc
    TRISCbits.TRISC2 = 0; // RC2 = nc
    TRISCbits.TRISC3 = 0; // RC3 = nc
    TRISCbits.TRISC4 = 0; // RC4 = nc
    TRISCbits.TRISC5 = 0; // RC5 = nc
    TRISCbits.TRISC6 = 0; // RC6 = nc
    TRISCbits.TRISC7 = 0; // RC7 = nc

 
    //	PORT D Assignments
    TRISDbits.TRISD0 = 0; // RD0 = nc
    TRISDbits.TRISD1 = 0; // RD1 = nc
    TRISDbits.TRISD2 = 0; // RD2 = nc
    TRISDbits.TRISD3 = 0; // RD3 = nc
    TRISDbits.TRISD4 = 0; // RD4 = nc
    TRISDbits.TRISD5 = 0; // RD5 = nc
    TRISDbits.TRISD6 = 0; // RD6 = nc
    TRISDbits.TRISD7 = 0; // RD7 = nc

    ANSELD=0x00;    // all port D pins are digital I/O

    
    //	PORT E Assignments
    TRISEbits.TRISE0 = 1; // RE0 = nc
    TRISEbits.TRISE1 = 1; // RE1 = nc
    TRISEbits.TRISE2 = 1; // RE2 = nc

    ANSELE=0x00;    // all port E pins are digital I/O

   //**********************************************************************************
    // Set up the FVR which will be used as Vref for the DAC
    //**********************************************************************************
    //FVRCONbits.CDAFVR=0x00;       // FVR output voltage for DAC = off
    //FVRCONbits.CDAFVR=0x01;       // FVR output voltage for DAC = 1.024V
    //FVRCONbits.CDAFVR=0x02;       // FVR output voltage for DAC = 2.048V
    FVRCONbits.CDAFVR=0x03;         // FVR output voltage for DAC = 4.096V
    FVRCONbits.FVREN=1;             // FVR enabled
    

    //**********************************************************************************
    // Set up the DAC
    //**********************************************************************************
    // Note that you can choose from different sources for the DAC positive voltage source.
    // Choosing Vdd can get you a wider output range, but choosing the
    // FVR (Fixed Voltage Reference) will typically get you a much
    // more stable reference voltage than Vdd. The FVR can also be used as the Vref for
    // the ADC.  Read the section in the datasheet about the FVR for more details.
    //**********************************************************************************
    DACCON0bits.DACOE1=1;        // enable the DAC1 output 1 pin (RA2).
    DACCON0bits.DACOE2=0;        // disable the DAC1 output 2 pin (RB7).


                                // Now set the DAC positive voltage source.
    //DACCON0bits.DACPSS=0x00;  // use this for DAC voltage source as VDD
    //DACCON0bits.DACPSS=0x01;  // use this for DAC voltage source as Vref pin (RA1)
    DACCON0bits.DACPSS=0x02;    // use this for DAC Voltage source as the FVR

    DACCON0bits.DACNSS = 0;     // DAC negative voltage source is Vss
    //DACCON0bits.DACNSS = 1;   // DAC negative voltage source is Vref- pin (RA2)

    DACCON0bits.DACEN=1;        // turn DAC on
                                 

    // Now we are ready to set the DAC output voltage using the DACR register.
    // Here are typical measured output values based on the selection of Vref
    // and the value in the DACR register. Measurement was taken using DVM
    // on the DAC1OUT pin (RA2).
    //
    //              Measured Output      Measured Output
    //      DACR    Vref=FVR(4.096V)     Voltage (Vdd=5V)
    //      ----    ----------------     ----------------
    //      0x01        0.020 V             0.024 V
    //      0x0F        0.246               0.295
    //      0x1F        0.506               0.608
    //      0x3F        1.021               1.226
    //      0x5F        1.536               1.844
    //      0x7F        2.052               2.462
    //      0x9F        2.568               3.082
    //      0xBF        3.087               3.706
    //      0xDF        3.607               4.329
    //      0xFF        4.129               4.955
    //
    //**********************************************************************************

    while (1) //endless loop - slowly ramp up DAC output voltage with half-second delay in between each step
    {
    
        for (i=15;i<=255;i=i+16)
        {
            DACCON1bits.DACR=i;  // set DAC output level
            __delay_ms(500);
        }


    } // end of main loop


}








