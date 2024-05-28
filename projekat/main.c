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
    // configure button S1
    P1REN |= BIT4;              // enable pull up/down
    P1OUT |= BIT4;              // set pull up
    P1DIR &= ~BIT4;             // configure P1.4 as in
    P1IES |= BIT4;              // interrupt on falling edge
    P1IFG &= ~BIT4;             // clear flag
    P1IE  |= BIT4;              // enable interrupt
    /* initialize Timer A */
    TA0CCR0 = TIMER_PERIOD;     // debounce period
    TA0CCTL0 = CCIE;            // enable CCR0 interrupt
    TA0CTL = TASSEL__ACLK;

	return 0;
}
