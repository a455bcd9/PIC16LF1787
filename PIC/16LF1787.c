// http://www.egr.msu.edu/classes/ece480/capstone/spring12/group03/Documents/Ryan_ApplicationNote.pdf (Je l’ai pas trop utilisé)
// http://electrosome.com/adc-pic-microcontroller-hi-tech-c/

// 'C' source line config statements
#include <xc.h>
#include <htc.h>

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
#pragma config BORV = 19        // Brown-out Reset Voltage Selection bit (Brown-out Reset Voltage (VBOR) set to 1.9 V nominal)
#pragma config PLLEN = ON       // INTOSC PLLEN Enable Bit (INTOSC Frequency is 32 MHz
#pragma config LVP = OFF		// cf UART

// CONFIG2
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bits (All VCAP pin functions are disabled)

// Definitions
#define _XTAL_FREQ  16000000    // this is used by the __delay_ms(xx) and __delay_us(xx) functions
#define IncomingData_LED   LATDbits.LATD4	// Incoming Data LED shows that uart data has been received

unsigned short ADC_Read()
{
	unsigned short ADCResult ;
 
// while(1)
//   {
	__delay_us(1); //Waits for the acquisition to complete 1 microseconde
	ADCON0bits.GO = 1;   //Starts ADC conversion 
	while (ADCON0bits.nDONE) continue;   //wait till ADC conversion is over 
	ADCResult = (ADRESH<<8) + ADRESL ;   //Merging the MSB and LSB   
// ADRESH<<8 ou bien ADRESHbits.AD ?
   return ADCResult ;
//   }
}

void main()
{
unsigned int a;
	OSCCON = 0xF8;	// 11111000 On demande 32 Mhz le resultat depend de PLLEN cf p 56 + 74 + 82
	
//	OPTION_REGbits.nWPUEN = 0; // enable weak pullups (each pin must be enabled individually)
	
    // PORT A Assignments
	TRISA = 0xFF; //PORTA as input RA0

// Configuration ADC
	ANSELAbits.ANSA0 = 1; // Pin is assigned as analog input. Digital input buffer disabled. Analog Select between Analog or Digital Function on Pins RA0

// for( ; (OSCCON & 0xC) != 0xC ; );  On attend la stabilisation de l'oscillateur pour continuer
// ANSELA = 2; // initialise les registres ANSELA et ANSELB  0  I/O numérique ; 
// 1 : input ; 0 : Output  
// ANSELB = 0;
// Port C : C6 et C7 sont TX et RX resp.  
	ANSELC = ANSELC & 0x3D;
	
    // PORT B Assignments
	TRISB0 = 0; // RB0 as output
	TRISB1 = 0;
	TRISB2 = 0;
	TRISB3 = 0;
	TRISB4 = 0;
	TRISB5 = 0;
	TRISB6 = 1; // RB6 ICSPCLK
	TRISB7 = 1; // RB7 ICSPDAT
	
    // PORT C Assignments
	TRISC0 = 1; // multiplexeur en sortie
	TRISC1 = 1; // idem
	TRISC2 = 1; // idem
	TRISC3 = 1; // idem
	TRISC4 = 0;
	TRISC5 = 0;
	TRISC6 = 0 // RC6 : écriture
	TRISC7 = 1 // RC7 : lecture port serie entre 1
	
	// 
	TRISD = 0x00; // multiplexeur en sortie
	
	// 
	TRISE = 0xFF; // RE3 MCLR as input
	
	ADCON1bits.ADCS = 010;   //Selecting the clk division factor = FOSC/32
	ADCON1bits.ADFM = 1;    // Result right justified (1 = 2’s complement format 0 = Sign-magnitude result format )   
	ADCON0bits.ADON = 1;    //Turns on ADC module 
	ADCON0bits.CHS = 00000;     //Selects channel 0 ( AN0) 
	ADCON0bits.ADRMD = 0 ;  // ADC Result Mode bit 12 bits
	ADCON0bits.ADNREF = 1 ; // VSS 
	ADCON0bits.ADPREF = 00 ; // VDD Reference voltages

// Configuration timer port serie
	BRG16= 0; // BRG16 = 0; page 363
	SPBRG = 51; // 9600 bauds avec une horloge à 32 MHz (p 360 de la doc)
// FOSC : 20 MHz ? NON car ici INTOSC = 32 Mhz

// TXSTA page 355
// Transmission 8 bits, pas de parite, 9600 bauds
	TXSTA = TXSTA & 0xDF; 	// TX9 = 1 on transmet 9 bits (donc la non)
	TXSTA = TXSTA | 1; 		// TX9D = 9eme bit a transmettre à 1 (=stop)
	TXSTA = TXSTA & 0xFB; 	// BRGH = 0 low speed

	RCSTA = RCSTA | 0x20; // SREN = 1 réception serie activee
	RCSTA = RCSTA & 0xDF; // RX9 = 0 on n'attend que 8 bits
	RCSTA = RCSTA | 0x04; // FERR=1
	RCSTA = RCSTA | 0x02; // OERR=1

// Monsieur Microchip dit que ces 3 la doivent etre dans
// cet etat pour avoir une liaison serie asynchrone
	TXSTA = TXSTA & 0xEF; // SYNC = 0 mode asynchrone
	TXSTA = TXSTA | 0x20; // TXEN = 1
	RCSTA = RCSTA | 0x80; // SPEN = 1 liaison serie activee

do
{
// Code de Guillaume
// Faire les 36 mesures

	a = ADC_Read(); //Reading Analog Channel (On lit déjà AN0)
// b = éliminer les bits de poids faible de a et envoyer un octet
	__delay_ms(50);        // give time for voltage levels on board to settle
}while(1); //Infinite Loop
}

-----------------------------------------------
// Code pour l’export par Martincic
char MSG_INIT_1[]="\n\rCircuit de mesure v1.0\0";

Dans le main : Affichage((int)((ADCREAD()>>4)&0xFF));

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
	
	TXREG = "/n"; // Saut de ligne
}

------------------------------------------------

// Code de Guillaume à insérer dans celui de Li

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
