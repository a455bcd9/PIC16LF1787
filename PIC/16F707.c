// Programme d Emile Martincic pour la PIC16F707


// PIC16F707 Configuration Bit Settings

// 'C' source line config statements
#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
#pragma config FOSC = INTOSC    // Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function  on RA7/OSC1/CLKIN)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR Pin Function Select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config BORV = 19        // Brown-out Reset Voltage Selection bit (Brown-out Reset Voltage (VBOR) set to 1.9 V nominal)
#pragma config PLLEN = ON       // INTOSC PLLEN Enable Bit (INTOSC Frequency is 16 MHz (32x))

// CONFIG2
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bits (All VCAP pin functions are disabled)

// #include <pic16f707.h>
#include <htc.h>

// Definitions des courants utilises par la source
// de courant pour la mesure de capacite
#define ledblanche PORTAbits.RA0
#define ON 				1
#define OFF 			0
#define NOISEDETECTION 	1
#define uA0d1 			2
#define uA1d2 			3
#define uA9 			4
#define uA18 			5
#define uA30 			6
#define uA100 			7

double NB_a_vide = 34708;
double C1 = 10e-12, NB_avec_C1 = 20209.0;
double C2 = 3.9e-12, NB_avec_C2 = 26389.5;
double Coef, Capa_par;

char MSG_INIT_1[]="\n\rCircuit de mesure v1.0\0";
char MSG_INIT_2[]="\n\rMesure sur 10 voies\n\r\0";
char MESSAGE_IT[]="\n\rIT \0";
char MESSAGE_IT1[]="Timer 1\0";
char MESSAGE_IT3[]="Timer 3\0";
char MESSAGE_ITA[]="Timer A\0";
char MESSAGE1[]="\n\rMesure = \0";
char MESSAGE2[]="\n\r\0";

unsigned int MeasuringPeriod=0, MeasuringPeriodSET=0;
unsigned int mTouch_counter=0, mTouchOF_counter=0, mTouchOverflow=0;
unsigned int SendingPeriod=0, SendingPeriodSET=0;
unsigned int DataReadyForSend=0;
unsigned int affiche;

int i,j;

void Init_CPSA(char Current)
{
//CPSxRM: Capacitive Sensing Reference Mode bit
//1 = Capacitive sensing module is in high range. 
//    DAC and FVR provide oscillator voltage references.
//0 = Capacitive sensing module is in low range. 
//    Internal oscillator voltage references are used.
//If CPSxRM = 0 (low range):
//Dans ce cas, les références de tension sont internes
//11 = Oscillator is in high range: Charge/discharge current is nominally 18 µA.
//10 = Oscillator is in medium range. Charge/discharge current is nominally 1.2 µA.
//01 = Oscillator is in low range. Charge/discharge current is nominally 0.1 µA.
//00 = Oscillator is off.
//If CPSxRM = 1 (high range):
//*** Dans ce cas, les références de tension sont externes ***
//*** Il faut alors utiliser les tensions de référence en  ***
//*** provenance du DAC et de la FVR                       ***
//11 = Oscillator is in high range: Charge/discharge current is nominally 100 µA.
//10 = Oscillator is in medium range. Charge/discharge current is nominally 30 µA.
//01 = Oscillator is in low range. Charge/discharge current is nominally 9 µA.
//00 = Oscillator is on; Noise Detection mode; 
//     No charge/discharge current is supplied.

switch (Current)
	{
	case OFF:
				CPSARM=0;
				CPSARNG0=0;
				CPSARNG1=0;
				break;
	case NOISEDETECTION:
				CPSARM=1;
				CPSARNG0=0;
				CPSARNG1=0;
				break;
	case uA0d1:
				CPSARM=0;
				CPSARNG0=1;
				CPSARNG1=0;
				break;
	case uA1d2:
				CPSARM=0;
				CPSARNG0=0;
				CPSARNG1=1;
				break;
	case uA9:
				CPSARM=1;
				CPSARNG0=1;
				CPSARNG1=0;
				break;
	case uA18:
				CPSARM=0;
				CPSARNG0=1;
				CPSARNG1=1;
				break;
	case uA30:
				CPSARM=1;
				CPSARNG0=0;
				CPSARNG1=1;
				break;
	case uA100:
				CPSARM=1;
				CPSARNG0=1;
				CPSARNG1=1;
				break;
	default:
				CPSARM=0;
				CPSARNG0=0;
				CPSARNG1=0;
	}

}



void init_timers(void)
{
// Le timer 1 sera le timer pour le module de détection
// capacitive (section 16.6.1 TIMERA/B de la doc) ...

// TMRCS<1:0>: Timerx Clock Source Select bits
// 11 =Timerx clock source is Capacitive Sensing Oscillator (CPSxOSC)
// 10 =Timerx clock source is pin or oscillator
// Dans la doc, c'est TMRxCS et cela ne concerne que le timer 1 ! 
// C'est un bug de nomenclature 
TMRCS1=1;
TMRCS0=1;

//If T1OSCEN = 0:
//External clock from TxCKI pin (on the rising edge)
//If T1OSCEN = 1:
//Crystal oscillator on T1OSI/T1OSO pins
//01 =Timerx clock source is system clock (FOSC)
//00 =Timerx clock source is instruction clock (FOSC/4)
//bit 3 T1OSCEN: LP Oscillator Enable Control bit(1)
//1 = Dedicated Timer1/3 oscillator circuit enabled
//0 = Dedicated Timer1/3 oscillator circuit disabled
// Ici, on s'en fout : c'est le capacitive sensing qui prend
// le pas sur T1OSCEN
T1OSCEN=0;

//TxCKPS<1:0>: Timerx Input Clock Prescale Select bits
//11 = 1:8 Prescale value
//10 = 1:4 Prescale value
//01 = 1:2 Prescale value
//00 = 1:1 Prescale value
T1CKPS0=0;
T1CKPS1=0;


//bit 2 TxSYNC: Timerx External Clock Input Synchronization Control bit
//If TMRxCS<1:0> = 1X
//1 = Do not synchronize external clock input
//0 = Synchronize external clock input with system clock (FOSC)
//If TMRxCS<1:0> = 0X
//This bit is ignored. Timerx uses the internal clock when TMR1CS<1:0> = 0X.
T1SYNC=1;

//TMRxON: Timerx on bit
//1 = Enables Timerx
//0 = Stops Timerx. Clears Timerx gate flip-flop
TMR1ON=1;

//TMRxGE: Timerx Gate Enable bit
///If TMRxON = 0:
//This bit is ignored.
//If TMRxON = 1:
//1 = Timerx counting is controlled by the Timerx gate function
//0 = Timerx counts regardless of Timerx gate function
TMR1GE=0;

// TMRxIE bit of the PIEx register enables TMR1 interrupt
TMR1IE=1;


// et le timer A fera la fenêtre temporelle de comptage
// TxXCS: TimerA/B External Clock Source Select bit
// If TMRxCS = 1:
// The TxXCS bit controls which clock external to the core/TimerA/B 
// module supplies TimerA/B:
// 1 = TimerA/B clock source is the capacitive sensing oscillator
// 0 = TimerA/B clock source is the TxCKI pin
// If TMRxCS = 0:
// TimerA/B clock source is controlled by the core/TimerA/B module 
// and is FOSC/4.
TMRACS = 0 ;
TAXCS = 0;	

// Prescaler TMRAPSA=0 pour l'activer
TMRAPSA = 0;
// Le code du prediviseur fait de 2 a 256 (2 puissance n+1) 
TMRAPS0 = 1;
TMRAPS1 = 1;
TMRAPS2 = 0;
// Donc l'overflow du timer A arrive a 4 000 000 / 16 = 250 kHz

TMRAON = 1;

// Et on autorise les interruptions en provenance du timer A
TMRAIE=1;


/*	*****************************************************
	Les timers 1 et 3 sont étroitement liés
	On ne peut pas choisir complètement séparéments leurs
	comportement
	***************************************************** */
//TMR3CS<1:0>: Timer3 Clock Source Select bits
// 00 = Timer3 clock source fosc/4
// 01 = Timer3 clock source fosc (il y a une erreur dans le
//      dessin de la doc
TMR3CS0=0;
TMR3CS1=0;

//TxCKPS<1:0>: Timerx Input Clock Prescale Select bits
//11 = 1:8 Prescale value
//10 = 1:4 Prescale value
//01 = 1:2 Prescale value
//00 = 1:1 Prescale value
T3CKPS0=0;
T3CKPS1=0;

//bit 2 TxSYNC: Timerx External Clock Input Synchronization Control bit
//If TMRxCS<1:0> = 1X
//1 = Do not synchronize external clock input
//0 = Synchronize external clock input with system clock (FOSC)
//If TMRxCS<1:0> = 0X
//This bit is ignored. Timerx uses the internal clock when TMR1CS<1:0> = 0X.
T3SYNC=1;

//TMRxON: Timerx on bit
//1 = Enables Timerx
//0 = Stops Timerx. Clears Timerx gate flip-flop
TMR3ON=0;

//TMRxGE: Timerx Gate Enable bit
///If TMRxON = 0:
//This bit is ignored.
//If TMRxON = 1:
//1 = Timerx counting is controlled by the Timerx gate function
//0 = Timerx counts regardless of Timerx gate function
TMR3GE=0;

// TMRxIE bit of the PIEx register enables TMR1 interrupt
TMR3IE=1;

}




void SetFVR(char State, char DACreference,char ADCreference)
{
FVREN=State;
//FVREN: Fixed Voltage Reference Enable bit
//0 = Fixed Voltage Reference is disabled
//1 = Fixed Voltage Reference is enabled
switch (DACreference)
	{
	//CDAFVR<1:0>: Cap Sense and D/A Converter Fixed Voltage 
	//Reference Selection bit(2)
	//A partir d'une tension de 1,024 V, on éteint ou on
	//multiplie par 1, 2 ou 4
	case 0:
		CDAFVR1=0;
		CDAFVR0=0;
		break;
	case 1:
		CDAFVR1=0;
		CDAFVR0=1;
		break;
	case 2:
		CDAFVR1=1;
		CDAFVR0=0;
		break;
	case 4:
		CDAFVR1=1;
		CDAFVR0=1;
		break;
	default:
		CDAFVR1=0;
		CDAFVR0=0;
	}

switch (ADCreference)
	{
	//ADFVR<1:0>: A/D Converter Fixed Voltage Reference Selection bit(2)
	//A partir d'une tension de 1,024 V, on éteint ou on
	//multiplie par 1, 2 ou 4
	case 0:
		ADFVR1=0;
		ADFVR0=0;
		break;
	case 1:
		ADFVR1=0;
		ADFVR0=1;
		break;
	case 2:
		ADFVR1=1;
		ADFVR0=0;
		break;
	case 4:
		ADFVR1=1;
		ADFVR0=1;
		break;
	default:
		ADFVR1=0;
		ADFVR0=0;
	}
while(!FVRRDY);
//FVRRDY: Fixed Voltage Reference Ready Flag bit
//0 = Fixed Voltage Reference output is not active or stable
//1 = Fixed Voltage Reference output is ready for use
//FVRRDY is always '1' on PIC16F707 devices
}


#define NEGATIVE_PIN 0
#define POSITIVE_PIN 1
#define ENABLE 1
#define DISABLE 0
#define VDD 0
#define VREF 1
#define FVR 2

void DAC_init(char State, char ReferencePin, char DacOutEnable, char RefSource, char DAC_val)
{
//DACEN: Digital-to-Analog Converter Enable bit (0 : éteint, 1 : allumé)
DACEN=State;
//DACLPS: DAC Low-Power Voltage State Select bit
//0 = VDAC = DAC negative reference source selected
//1 = VDAC = DAC positive reference source selected
DACLPS=ReferencePin;
//DACOE: DAC Voltage Output Enable bit
//0 = DAC voltage level is output on the DACOUT pin
//1 = DAC voltage level is disconnected from the DACOUT pin
DACOE=DacOutEnable;
switch (RefSource)
	{
	//DACPSS<1:0>: DAC Positive Source Select bits
	//00 = VDD
	//01 = VREF
	//10 = FVR Buffer 2 output
	//11 = Reserved, do not use
	case VDD:
		DACPSS1=0;
		DACPSS0=0;
		break;
	case VREF:
		DACPSS1=0;
		DACPSS0=1;
		break;
	case FVR:
		DACPSS1=1;
		DACPSS0=0;
		SetFVR(ON,4,OFF);
		break;
	default:
		DACPSS1=0;
		DACPSS0=0;
	}

if (DAC_val<32)
	DACCON1=DAC_val;
else
	DACCON1=31;

}

void interrupt isr(void)
{

/*	*********************************************
	Message lorsqu'on entre dans la fonction d'IT 
	********************************************* */
/*
for(i=0;MESSAGE_IT[i]!=0;i++)
	{
	for(;(TXSTA&0x02)!= 0x02;);
   	TXREG = MESSAGE_IT[i];
	}
*/

if (TMR1IF)
	{
	/*
	for(i=0;MESSAGE_IT1[i]!=0;i++)
		{
		for(;(TXSTA&0x02)!= 0x02;);
   		TXREG = MESSAGE_IT1[i];
		} */
	mTouchOverflow++;
	TMR1IF=0;
	}

if (TMRAIF)
	{
	/*
	for(i=0;MESSAGE_ITA[i]!=0;i++)
		{
		for(;(TXSTA&0x02)!= 0x02;);
   		TXREG = MESSAGE_ITA[i];
		} */ 

	// 250 kHz donc on met 6 pour etre interrompu a 1 kHz
	TMRA = 6;
	// Acquitement de l'IT : RAZ du flag
	TMRAIF=0;

	MeasuringPeriod++;
	
	if (MeasuringPeriod==MeasuringPeriodSET)
		{
		mTouch_counter=TMR1;
		mTouchOF_counter=mTouchOverflow;
		TMR1=0;
		mTouchOverflow=0;
		MeasuringPeriod=0;
		}

	SendingPeriod++;
	if (SendingPeriod==SendingPeriodSET)
		{
		SendingPeriod=0;
		DataReadyForSend=1;
		}
	}


if (TMR3IF)
	{
	/*
	for(i=0;MESSAGE_IT3[i]!=0;i++)
		{
		for(;(TXSTA&0x02)!= 0x02;);
   		TXREG = MESSAGE_IT3[i];
		} */
	TMR3IF=0;
	TMR3=25535;
	//clock is 16 000 000 Hz
	//and timer source is Fosc/4 = 4 000 000 Hz
	//if we want interrupt to ocure every 10ms timer should count up 
	//to 40 000 every time timer is initiated to make interrupt on 
	//overflow meaning it should start from 65535 (count on which
	//it will make interrupt) - 40000 = 25535
	MeasuringPeriod++;
	
	if (MeasuringPeriod==MeasuringPeriodSET)
		{
		mTouch_counter=TMR1;
		mTouchOF_counter=mTouchOverflow;
		mTouchOverflow=0;
		MeasuringPeriod=0;
		}
	SendingPeriod++;
	if (SendingPeriod==SendingPeriodSET)
		{
		SendingPeriod=0;
		DataReadyForSend=1;
		}
	}
}

void Affichage()
{
char dizmil, mil, cent, diz, unit;
double calcul, capa_pF;
char calculs = 1;


if (calculs == 0)
	{
	affiche = mTouch_counter ;
	for(;(TXSTA&0x02)!= 0x02;);
   	dizmil = ((char)(affiche/10000))+0x30;
	TXREG = dizmil;
	affiche = affiche % 10000 ;
	for(;(TXSTA&0x02)!= 0x02;);
   	mil = ((char)(affiche/1000))+0x30;
	TXREG = mil;
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
	for(i=0;MESSAGE2[i]!=0;i++)
		{
		for(;(TXSTA&0x02)!= 0x02;);
	   	TXREG = MESSAGE2[i];
		}
	}
else
// if (calculs == 1)
	{
	/* Transformation de la valeur lue en pF */
	/* On suppose une capa parasite et des tests
	   les calculs sont dans le main et en tête
	   du programme */

	calcul = (Coef / mTouch_counter) - Capa_par;
	capa_pF = calcul*1e12;
	affiche = (int) (capa_pF * 1000); // valeur lue pour nous

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
	for(i=0;MESSAGE2[i]!=0;i++)
		{
		for(;(TXSTA&0x02)!= 0x02;);
	   	TXREG = MESSAGE2[i];
		}
	}
}

void delay10us (unsigned usecs)
{
while (-- usecs > 0)
 {
 }
}

/* **********************************************************
   **********************************************************
   ATTENTION : PAR DEFAUT, LE PIC16F707 ATTEND UN OSCILLATEUR 
   EXTERNE - IL FAUT CHANGER CA DANS LES CONFIGURATION BITS
   **********************************************************
   ********************************************************** */

void main(void)
{

OSCCON = 0x30;	// On demande 16MHz ou 500 kHz
// Le resultat depend de PLLEN (bit 12 du registre CONFIG1 : 
// CONFIGURATION WORD REGISTER 1, p 77) 

for( ; (OSCCON & 0xC) != 0xC ; );   // On attend la stabilisation 
									// de l'oscillateur pour continuer
ANSELA = 2; // initialise les registres ANSELA et ANSELB  0  I/O numérique ; 
			// 1 : input ; 0 : Output  
ANSELB = 0;
// Port C : C6 et C7 sont TX et RX resp.  
// C2 est l'entrée du timer B et je ne vois pas bien
// pourquoi on s'en sert ici ...
ANSELC = ANSELC & 0x3D;

//configuration des périphériques 
TRISA = 0x02;	//configure le port A0 en sortie 1e 0s
TRISB = 0xFF;	//configure B en entrée (B0 en entrée)
TRISC = 0x80;	//RC6 : écriture et RC7 : lecture port serie

// Configuration timer port serie

SPBRG = 25; // 9600 bauds avec une horloge à 16 MHz (p 147 de la doc)
// page 360 de la datasheet
// FOSC : 20 MHz ?
// ADC clock FOSC/32 ? page 169

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


/* ********************************************************
   Test de la liaison série. On envoie le motif : 
              a - pause - b - pause longue 
for(;;)
	{
	for(;(TXSTA&0x02)!= 0x02;);
   	TXREG = 'a';
	for(i=0 ; i!= 500 ; i++);
	for(;(TXSTA&0x02)!= 0x02;);
   	TXREG = 'b';
	for(i=0 ; i!= 5000 ; i++);
	}
    ******************************************************** */

/* ********************************************************
   Message de bienvenue sur la liaison série
   ******************************************************** */ 

for(j=0;MSG_INIT_1[j]!=0;j++)
	{
	for(;(TXSTA&0x02)!=0x02;);
   	TXREG = MSG_INIT_1[j];
	}
for(j=0;MSG_INIT_2[j]!=0;j++)
	{
	for(;(TXSTA&0x02)!=0x02;);
   	TXREG = MSG_INIT_2[j];
	}


/* ********************************************************
   Initialisation du module de détection capacitive
   avec tous les détails qui vont bien
   ******************************************************** */ 
init_timers();

//Références de tension pour le module de détection capacitive
SetFVR(ON,4,OFF);

//DAC_init(ON/OFF, Reference pin, DacOutputEnable 1/0, RefSource, DAC_val)
DAC_init(ON,POSITIVE_PIN,1,FVR,20);

// CPSACON1 : choix de la voie (voie 0 CPSA0/RA1/AN1 !!)
// Numero de la voie sur le poids faible de CPSACON1, de 0 à 15
CPSACON1=0;

//CPSxON: Capacitive Sensing Module Enable bit
//1 = Capacitive sensing module is enabled
//0 = Capacitive sensing module is disabled
CPSAON=1;

// Choix de la source de courant 
// uA0d1, uA1d2, uA9, uA18, uA30, uA100, NOISEDETECTION
Init_CPSA(uA9);


// PEIE active le circuit d'autorisation des interruptions en 
// provenance des périphériques internes
PEIE=1;

// Une fois que tout est initialisé, on autorise les IT en général
GIE=1;


MeasuringPeriodSET = 200;
SendingPeriodSET = 300;


// Le calcul de la capa parasite ne peut pas être mis dans
// les constantes
Coef = ((C1-C2)*NB_avec_C1)*(NB_avec_C2/(NB_avec_C2-NB_avec_C1));
Capa_par = (Coef/NB_avec_C1)-C1;

while (1){
    if(PORTBbits.RB0==0){
        ledblanche=!ledblanche;
        delay10us(60000);
    }
}

while (1)
	{
	/* ********************************************************
	   La LED blanche s'allume si on appuie sur le bouton  
       ******************************************************** */
      //  ledblanche = !PORTBbits.RB0;

              // PORTAbits.RA0=0;
              // PORTAbits.RA0=1;
       // la LED blanche clignote
	/*  if (DataReadyForSend)
		{
		DataReadyForSend = 0;
		/* PORTAbits.RA0 = !PORTAbits.RA0;
		for(i=0;MESSAGE1[i]!=0;i++)
			{
			for(;(TXSTA&0x02)!= 0x02;);
		   	TXREG = MESSAGE1[i];
			} */

	/*	Affichage();
		} */
    }

}
