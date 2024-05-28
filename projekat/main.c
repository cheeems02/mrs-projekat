#include <msp430.h> 
/*
 * Napisati program kojim se vrši akvizicija signala sa jednog naponskog kanala. Na komandu
pritiska tastera startuje se akvizicija sa uèestanošæu od 20 sample/s, i sekvenca dužina 200 odbiraka
se smešta u ram memoriju kontrolera. U toku akvizicije ukljuèena je LE dioda. Po završetku
akvizicije na komandu pritiska preostalih tastera se na LED displej ispisuje minimalna, srednja i
maksimalna vrednost uzorkovanog signala, u zavisnosti od toga koji je od preostalih tastera
pritisnut.
 */

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	return 0;
}
