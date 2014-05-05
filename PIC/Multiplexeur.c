
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
