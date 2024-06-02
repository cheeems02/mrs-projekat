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
#define NUM_SAMPLES    200
#define SAMPLE_RATE    20      // 20 samples per second
volatile unsigned int samples[NUM_SAMPLES];
volatile unsigned int sample_index = 0;
volatile unsigned char sampling = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    //buttons
    // configure button S1 - start
    P1REN |= BIT4;              // enable pull up/down
    P1OUT |= BIT4;              // set pull up
    P1DIR &= ~BIT4;             // configure P1.4 as in
    P1IES |= BIT4;              // interrupt on falling edge
    P1IFG &= ~BIT4;             // clear flag
    P1IE |= BIT4;               // enable interrupt
    // configure button S2 - min
    P1REN |= BIT5;              // enable pull up/down
    P1OUT |= BIT5;              // set pull up
    P1DIR &= ~BIT5;             // configure P1.4 as in
    P1IES |= BIT5;              // interrupt on falling edge
    P1IFG &= ~BIT5;             // clear flag
    P1IE |= BIT5;               // enable interrupt
    // configure button S3 -  avg
    P1REN |= BIT1;              // enable pull up/down
    P1OUT |= BIT1;              // set pull up
    P1DIR &= ~BIT1;             // configure P1.4 as in
    P1IES |= BIT1;              // interrupt on falling edge
    P1IFG &= ~BIT1;             // clear flag
    P1IE |= BIT1;               // enable interrupt
    // configure button S4 - max
    P2REN |= BIT1;              // enable pull up/down
    P2OUT |= BIT1;              // set pull up
    P2DIR &= ~BIT1;             // configure P1.4 as in
    P2IES |= BIT1;              // interrupt on falling edge
    P2IFG &= ~BIT1;             // clear flag
    P2IE |= BIT1;               // enable interrupt
    // led
    P1DIR |= BIT0;              //set P1.0 as output
    P1OUT &= ~BIT0;             //set output as 0
    // initialize Timer A
    TA0CCR0 = 32768 / SAMPLE_RATE;      // Set timer count for 20Hz
    TA0CCTL0 = CCIE;            // enable CCR0 interrupt
    TA0CTL = TASSEL__ACLK;
    //7seg
    P7DIR |= BIT0;              // set P7.0 as out (SEL1)
    P7OUT &= ~BIT0;             // enable display 1
    // a,b,c,d,e,f,g
    P2DIR |= BIT6 | BIT3;       // configure P2.3 and P2.6 as out
    P3DIR |= BIT7;              // configure P3.7 as out
    P4DIR |= BIT3 | BIT0;       // configure P4.0 and P4.3 as out
    P8DIR |= BIT2 | BIT1;       // configure P8.1 and P8.2 as out
    //ADC
    P6SEL |= BIT1;              // enable left pot
    ADC12CTL0 &= ~ADC12ENC;     // disable ADC before configuring
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;   // Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP;       // Use sampling timer
    ADC12MCTL0 = ADC12INCH_1;   // ADC input channel A1 (P6.1)
    ADC12IE = ADC12IE0;         // Enable interrupt
    ADC12CTL0 |= ADC12ENC;      // Enable conversion
    return 0;
}
