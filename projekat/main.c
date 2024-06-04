#include <msp430.h> 
#include <stdint.h>
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
#define NUM_SAMPLES    200     // 200 samples total
#define SAMPLE_RATE    20      // 20 samples per second
#define MUX_PERIOD     (164)  // ~5ms
#define SECOND 32768            //1s
#define DEBOUNCE 33             //1ms
volatile unsigned int samples[NUM_SAMPLES] = { 0 };
volatile unsigned int sample_index = 0;
volatile unsigned int sampling = 0;
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
    // initialize Timer A0
    TA0CCR0 = SECOND / SAMPLE_RATE;      // Set timer count for 20Hz
    TA0CCTL0 = CCIE;            // enable CCR0 interrupt
    TA0CTL = TASSEL__ACLK;
    // initialize Timer A1 - debounce
    TA1CCR0 = DEBOUNCE;      // Set timer count for 20Hz
    TA1CCTL0 = CCIE;            // enable CCR0 interrupt
    TA1CTL = TASSEL__ACLK;
    // initialize Timer A2 - mux
    TA2CCR0 = MUX_PERIOD;      // Set timer count for 20Hz
    TA2CCTL0 = CCIE;            // enable CCR0 interrupt
    TA2CTL = TASSEL__ACLK;
    // 7SEG
    // sevenseg 1
    P7DIR |= BIT0;              // set P7.0 as out (SEL1)
    P7OUT |= BIT0;              // disable display 1
    // sevenseg 2
    P6DIR |= BIT4;              // set P6.4 as out (SEL2)
    P6OUT |= BIT4;              // disable display 2
    // a,b,c,d,e,f,g
    P2DIR |= BIT6 | BIT3;       // configure P2.3 and P2.6 as out
    P3DIR |= BIT7;              // configure P3.7 as out
    P4DIR |= BIT3 | BIT0;       // configure P4.0 and P4.3 as out
    P8DIR |= BIT2 | BIT1;       // configure P8.1 and P8.2 as out
    //ADC
    P6SEL |= BIT1;              // enable left pot
    ADC12CTL0 &= ~ADC12ENC;     // disable ADC before configuring
    ADC12CTL0 = ADC12SHT0_4 | ADC12ON;   // Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP;       // Use sampling timer
    ADC12CTL2 &= ~ADC12RES_2;      // 8bit resolution
    ADC12MCTL0 = ADC12INCH_1;   // ADC input channel A1 (P6.1)
    ADC12IE = ADC12IE0;         // Enable interrupt
    __enable_interrupt();       //GIE
    while (1)
    {

    }
    return 0;
}
static volatile uint8_t cifre[2] = { 0 };
void display(const uint8_t broj)
{                               //0x0F je za skidanje vi�ih cifara
    cifre[1] = (broj >> 4) & 0x0F;
    cifre[0] = broj & 0x0F;
}
void start()
{
    sampling = 1;
    ADC12CTL0 |= ADC12ENC;      // Enable conversion
    P1OUT |= BIT0;              //set output as 1
}
void stop()
{
    sampling = 0;
    P1OUT &= ~BIT0;  // LED OFF
    ADC12CTL0 &= ~ADC12ENC; //disable ADC
}
void minimum()
{
    unsigned int i = 0;
    unsigned int minVal = 0xFFFF;
    for (i = 0; i < 200; i++)
    {
        if (samples[i] < minVal)
        {
            minVal = samples[i];
        }
    }
}
void average()
{
    unsigned int i = 0;
    unsigned long sum = 0;
    for (i = 0; i < 200; i++)
    {
        sum += samples[i];
    }
    unsigned int avgVal = sum / 200;
}
void maximum()
{
    unsigned int i = 0;
    unsigned int maxVal = 0;
    for (i = 0; i < 200; i++)
    {
        if (samples[i] > maxVal)
        {
            maxVal = samples[i];
        }
    }
}
