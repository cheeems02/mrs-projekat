#include <msp430.h> 
/*
 * Napisati program kojim se vr�i akvizicija signala sa jednog naponskog kanala. Na komandu
pritiska tastera startuje se akvizicija sa u�estano��u od 20 sample/s, i sekvenca du�ina 200 odbiraka
se sme�ta u ram memoriju kontrolera. U toku akvizicije uklju�ena je LE dioda. Po zavr�etku
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
