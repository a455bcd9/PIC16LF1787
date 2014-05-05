// Configuration de la PIClF1787
// Selection d une capacite a l aide des multiplexeurs ligne / colonne
// Fonction ADC : lecture de la tension de sortie
// Envoi des valeurs au port serie ordinateur

// Definitions
#define _XTAL_FREQ  32000000    // this is used by the __delay_ms(xx) and __delay_us(xx) functions
// #define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))

// 'C' source line config statements
#include <xc.h>
// #include <pic16lf1787.h> Inutile deja dans pic.h dans htc.h dans xc.h

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIGURATION BITS p 53 datasheet
#pragma config FOSC = INTOSC    // Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function  on RA7/OSC1/CLKIN)
				// frequence interne de 32Mhz
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR Pin Function Select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config BORV = 1         // Brown-out Reset Voltage Selection bit (Brown-out Reset Voltage (VBOR) set to 1.9 V nominal) page 411
#pragma config PLLEN = ON       // INTOSC PLLEN Enable Bit (INTOSC Frequency is 32 MHz
#pragma config LVP = OFF	// cf UART

unsigned short ADC_Read()
{
	unsigned short ADCResult ;

	__delay_us(1); //Waits for the acquisition to complete 1 microseconde
	ADCON0bits.GO = 1;   //Starts ADC conversion
	while(ADCON0bits.DONE);   //wait till ADC conversion is over
	ADCResult = (ADRESH<<8) + ADRESL ;   //Merging the MSB and LSB
	// ADRESH<<8 ou bien ADRESHbits.AD ?
   return ADCResult ;
}

void main()
{
	// set up oscillator control register for internal clock running at 32hz
	OSCCON = 0xF8;	// 11111000 On demande 32 Mhz le resultat depend de PLLEN cf pp 56 + 74 + 82

	// OPTION_REGbits.nWPUEN = 0; // enable weak pullups (each pin must be enabled individually)

	// PORT A Assignments
	TRISA = 0xFF; //PORTA as input RA0

	ANSELAbits.ANSA0 = 1; // Pin is assigned as analog input. Digital input buffer disabled. Analog Select between Analog or Digital Function on Pins RA0

	// for( ; (OSCCON & 0xC) != 0xC ; );  On attend la stabilisation de l'oscillateur pour continuer
	// ANSELA = 2; // initialise les registres ANSELA et ANSELB  0  I/O numérique ;
	// 1 : input ; 0 : Output

	// PORT B Assignments
	TRISB0 = 0; // RB0 as output VDD
	TRISB1 = 0; // nc
	TRISB2 = 0; // nc
	TRISB3 = 0; // nc
	TRISB4 = 0; // nc
	TRISB5 = 0; // nc
	TRISB6 = 1; // RB6 ICSPCLK
	TRISB7 = 1; // RB7 ICSPDAT

	ANSELB = 0x00; // all port B pins are digital I/O

	// PORT C Assignments
	TRISC0 = 1; // multiplexeur en sortie
	TRISC1 = 1; // idem
	TRISC2 = 1; // idem
	TRISC3 = 1; // idem
	TRISC4 = 0; // nc
	TRISC5 = 0; // nc
	TRISC6 = 0; // RC6 : écriture
	TRISC7 = 1; // RC7 : lecture port serie entre 1

        APFCON1bits.TXSEL = 0; // page 131 TX is on pin RC6
        APFCON1bits.RXSEL = 0; // page 131 RX is on pin RC7


	// ANSELC = ANSELC & 0x3D; // RC6 = TXREG et RC7 = RXREG pas sur PIC16LF1787

	// PORT D Assignments
	TRISD = 0x00; 	// multiplexeur en sortie

	ANSELD = 0x00;    // all port D pins are digital I/O

	// PORT E Assignments
	TRISE = 0xFF; 	// RE3 MCLR as input

	ANSELE = 0x00;    // all port E pins are digital I/O

	//**********************************************************************************
	// Set up the ADC
	//**********************************************************************************
	// Note that you can choose from different sources for the ADC positive voltage source.
	// Choosing Vdd can get you a wider output range, but choosing the
	// FVR (Fixed Voltage Reference) will typically get you a much
	// more stable reference voltage than Vdd. The FVR can also be used as the Vref for
	// the DAC.  Read the section in the datasheet about the FVR for more details.
	//**********************************************************************************

	ADCON1bits.ADCS = 0b010;	// Selecting the clk division factor = FOSC/32 cf page 169 ADC Clock Period TAD = 1.0 µs
	ADCON1bits.ADFM = 1;		// Result right justified (1 = 2 s complement format 0 = Sign-magnitude result format )
	ADCON0bits.ADON = 1;		// Turns on ADC module
	ADCON0bits.CHS = 0b00000;	// Selects channel 0 ( AN0)
	ADCON0bits.ADRMD = 0 ;		// ADC Result Mode bit 12 bits
	ADCON1bits.ADNREF = 0 ;		// VSS Reference voltage
	ADCON1bits.ADPREF = 0b00 ;	// VDD Reference voltage


	//*************************************************************************************
    	// Set up the UART
    	// Note: See the datasheet for tables with the values of SPBRGH:SPBRGL already
    	// calculated based on different values of Fosc and BRGH and desired
    	// baud rates.
    	//*************************************************************************************

	// page 359 8 bit asynchronous SYNC = 0 BRG16 = 0 BRGH = 0
	// BRG16 dans BAUDCON page 357
	// SYNC et BRGH dans TXSTA page 355

	// Configuration timer port serie
	BAUDCONbits.BRG16 = 0; 					// BRG16 = 0 page 363 et 357

	// TXSTA page 355
	// Transmission 8 bits, pas de parite, 9600 bauds
/* 	TXSTA = TXSTA & 0xDF; 		// TX9 = 1 on transmet 9 bits (donc la non)
	TXSTA = TXSTA | 1; 			// TX9D = 9eme bit a transmettre à 1 (=stop)
	TXSTA = TXSTA & 0xFB; 		// BRGH = 0 low speed */
    	TXSTAbits.BRGH = 0;		// select low speed Baud Rate (see baud rate calcs below)
    	TXSTAbits.TX9  = 0;		// select 8 data bits
    	TXSTAbits.TXEN = 1;     // enable transmit
    	TXSTAbits.SYNC = 0; 	// asynchronous

/* 	RCSTA = RCSTA | 0x20; 		// SREN = 1 réception serie activee
	RCSTA = RCSTA & 0xDF; 		// RX9 = 0 on n'attend que 8 bits
	RCSTA = RCSTA | 0x04; 		// FERR=1
	RCSTA = RCSTA | 0x02; 		// OERR=1 */

    	RCSTAbits.SPEN = 1;       // serial port is enabled
    	RCSTAbits.RX9 = 0;        // select 8 data bits
    	RCSTAbits.FERR = 1;       // Framing Error
    	RCSTAbits.OERR = 1;       // Overrun Error
    	RCSTAbits.CREN = 1;       // receive enabled

/* 	// Monsieur Microchip dit que ces 3 la doivent etre dans
	// cet etat pour avoir une liaison serie asynchrone
	TXSTA = TXSTA & 0xEF;		// SYNC = 0 mode asynchrone
	TXSTA = TXSTA | 0x20; 		// TXEN = 1
	RCSTA = RCSTA | 0x80; 		// SPEN = 1 liaison serie activee */

	// calculate values of SPBRGL and SPBRGH based on the desired baud rate
	// cf explications page 358
	SPBRG = 51; 				// 9600 bauds avec une horloge INTOSC = 32 MHz BRGH = 0 et BRG16 = 0 pour 8 bit asynchronous (p 360 de la doc)

	PIR1bits.RCIF=0;        // make sure receive interrupt flag is clear
    	PIE1bits.RCIE=1;        // enable UART Receive interrupt
    	INTCONbits.PEIE = 1;    // Enable peripheral interrupt
	INTCONbits.GIE = 1;     // enable global interrupt

do
{
// Inserer code de Guillaume de selection des lignes / colonnes
// Faire les 36 mesures

/* 	a = ADC_Read(); //Reading Analog Channel (On lit déjà AN0)
// b = éliminer les bits de poids faible de a et envoyer un octet
	__delay_ms(50);        // give time for voltage levels on board to settle */

	TXREG = 'H'; // Test de EUSART
}while(1); //Infinite Loop
}

/* -----------------------------------------------
// Code pour export repris de PIC16F707.c
char MSG_INIT_1[]="\n\rCircuit de mesure v1.0\0";

Dans le main : Affichage((int)((ADCREAD()>>4)&0xFF)); */

void Affichage(char affiche)
{
char dizmil, mil, cent, diz, unit;
double calcul;
// double capa_pF;
// char calculs = 1;

	/* Transformation de la valeur lue en pF */
	/* On suppose une capa parasite et des tests
	   les calculs sont dans le main et en tête
	   du programme */

//	calcul = (Coef / mTouch_counter) - Capa_par;
//	capa_pF = calcul*1e12;
//	affiche = (int) (capa_pF * 1000); // valeur lue pour nous

	for(;(TXSTA&0x02)!= 0x02;);
   	dizmil = ((char)(affiche/10000))+0x30;
	TXREG = dizmil;
	affiche = affiche % 10000 ;

	for(;(TXSTA&0x02)!= 0x02;);
   	mil = ((char)(affiche/1000))+0x30;
	TXREG = mil;

	for(;(TXSTA&0x02)!= 0x02;);
   	TXREG = '.';

	affiche = affiche % 1000 ;

	for(;(TXSTA&0x02)!= 0x02;);
   	cent = ((char)(affiche/100))+0x30;
	TXREG = cent;
	affiche = affiche % 100 ;

	for(;(TXSTA&0x02)!= 0x02;);
	diz = ((char)(affiche/10))+0x30;
	TXREG = diz;
	affiche = affiche % 10 ;

	for(;(TXSTA&0x02)!= 0x02;);
   	unit = ((char)(affiche))+0x30;
	TXREG = unit;

	TXREG=0x0D; // Carriage return
        // TXREG=0x0A; ou bien ça ?
}

/* ------------------------------------------------

// Code de Guillaume à insérer dans celui de Li
// Erreur sur entree 1 et entree 2 a corriger
// Inscrire les equivalences entree / ligne et sortie / colonne
// Debrancher l entree apres la mesure puis attendre 5 tau la decharge du condo puis faire le nouveau branchement et le changement de voie

	PORTDbits.RD3 = 1; //active multiplexeur droit (entree = ligne)
	PORTCbits.RC3 = 1; //active multiplexeur gauche (sortie = colonne)

//boucle infinie sur l'ensemble des capteurs
for (;;) {
	PORTDbits.RD0 = 0; //entree 1
	PORTDbits.RD1 = 0;
	PORTDbits.RD2 = 0;

	PORTCbits.RC0 = 0; //sortie 1
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 11

	PORTCbits.RC0 = 1; //sortie 2
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 12

	PORTCbits.RC0 = 0; //sortie 3
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 13

	PORTCbits.RC0 = 1; //sortie 4
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 14

	PORTCbits.RC0 = 0; //sortie 5
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 15

	PORTCbits.RC0 = 1; //sortie 6
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 16


	PORTDbits.RD0 = 1; //entree 2
	PORTDbits.RD1 = 0;
	PORTDbits.RD2 = 0;

	PORTCbits.RC0 = 0; //sortie 1
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 21

	PORTCbits.RC0 = 1; //sortie 2
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 22

	PORTCbits.RC0 = 0; //sortie 3
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 23

	PORTCbits.RC0 = 1; //sortie 4
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 24

	PORTCbits.RC0 = 0; //sortie 5
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 25

	PORTCbits.RC0 = 1; //sortie 6
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 26


	PORTDbits.RD0 = 0; //entree 3
	PORTDbits.RD1 = 1;
	PORTDbits.RD2 = 0;

	PORTCbits.RC0 = 0; //sortie 1
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 31

	PORTCbits.RC0 = 1; //sortie 2
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 32

	PORTCbits.RC0 = 0; //sortie 3
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 33

	PORTCbits.RC0 = 1; //sortie 4
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 34

	PORTCbits.RC0 = 0; //sortie 5
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 35

	PORTCbits.RC0 = 1; //sortie 6
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 36


	PORTDbits.RD0 = 1; //entree 4
	PORTDbits.RD1 = 1;
	PORTDbits.RD2 = 0;

	PORTCbits.RC0 = 0; //sortie 1
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 41

	PORTCbits.RC0 = 1; //sortie 2
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 42

	PORTCbits.RC0 = 0; //sortie 3
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 43

	PORTCbits.RC0 = 1; //sortie 4
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 44

	PORTCbits.RC0 = 0; //sortie 5
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 45

	PORTCbits.RC0 = 1; //sortie 6
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 46


	PORTDbits.RD0 = 0; //entree 5
	PORTDbits.RD1 = 0;
	PORTDbits.RD2 = 2;

	PORTCbits.RC0 = 0; //sortie 1
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 51

	PORTCbits.RC0 = 1; //sortie 2
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 52

	PORTCbits.RC0 = 0; //sortie 3
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 53

	PORTCbits.RC0 = 1; //sortie 4
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 54

	PORTCbits.RC0 = 0; //sortie 5
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 55

	PORTCbits.RC0 = 1; //sortie 6
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 56


	PORTDbits.RD0 = 1; //entree 6
	PORTDbits.RD1 = 0;
	PORTDbits.RD2 = 1;

	PORTCbits.RC0 = 0; //sortie 1
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 61

	PORTCbits.RC0 = 1; //sortie 2
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
//mesure 62

	PORTCbits.RC0 = 0; //sortie 3
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 63

	PORTCbits.RC0 = 1; //sortie 4
	PORTCbits.RC1 = 1;
	PORTCbits.RC2 = 0;
//mesure 64

	PORTCbits.RC0 = 0; //sortie 5
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 65

	PORTCbits.RC0 = 1; //sortie 6
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 1;
//mesure 66
}
 */
