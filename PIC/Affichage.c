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
