#include <msp430.h>
#include <stdint.h>
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
#define NUM_SAMPLES    200     // 200 samples total
#define SAMPLE_RATE    20      // 20 samples per second
#define MUX_PERIOD     (164)  // ~5ms
#define SECOND (32768)            // 1s
#define DEBOUNCE (164)             // 5ms
volatile unsigned int samples[NUM_SAMPLES] = { 0 };
volatile unsigned int sample_index = 0;
volatile unsigned int sampling = 0;
volatile unsigned int output = 0;
static volatile unsigned int cifre[2] = { 0 };
void buttons_setup(void)
{
    //buttons
    // configure button S1 - start
    P2REN |= BIT1;              // enable pull up/down
    P2OUT |= BIT1;              // set pull up
    P2DIR &= ~BIT1;             // configure P2.1 as in
    P2IES |= BIT1;              // interrupt on falling edge
    P2IFG &= ~BIT1;             // clear flag
    P2IE |= BIT1;               // enable interrupt
    // configure button S2 -  avg
    P1REN |= BIT1;              // enable pull up/down
    P1OUT |= BIT1;              // set pull up
    P1DIR &= ~BIT1;             // configure P1.1 as in
    P1IES |= BIT1;              // interrupt on falling edge
    P1IFG &= ~BIT1;             // clear flag
    P1IE |= BIT1;               // enable interrupt
    // configure button S3 - max
    P1REN |= BIT4;              // enable pull up/down
    P1OUT |= BIT4;              // set pull up
    P1DIR &= ~BIT4;             // configure P1.4 as in
    P1IES |= BIT4;              // interrupt on falling edge
    P1IFG &= ~BIT4;             // clear flag
    P1IE |= BIT4;               // enable interrupt
    // configure button S4 - min
    P1REN |= BIT5;              // enable pull up/down
    P1OUT |= BIT5;              // set pull up
    P1DIR &= ~BIT5;             // configure P1.5 as in
    P1IES |= BIT5;              // interrupt on falling edge
    P1IFG &= ~BIT5;             // clear flag
    P1IE |= BIT5;               // enable interrupt
}
void output_setup(void)
{
    // led
    P1DIR |= BIT0;              //set P1.0 as output
    P1OUT &= ~BIT0;             //set output as 0
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
}
void timer_setup(void)
{
    TA0CCR0 = ((SECOND / SAMPLE_RATE) / 2 - 1);    //polovina frekvence ide ovde
    TA0CCTL1 |= OUTMOD_4;      // toggle mode
    //TA0CCR1 = 5;             // any number since CCR0 defines period
    TA0CTL |= TASSEL__ACLK;
    ADC12IE |= ADC12IE0;        // enable interrupt when MEM0 is written
    // initialize Timer A1 - debounce
    TA1CCR0 = DEBOUNCE;      // Set timer count for debounce
    TA1CCTL0 = CCIE;            // enable CCR0 interrupt
    TA1CTL = TASSEL__ACLK;
    // initialize Timer A2 - display mux
    TA2CCR0 = MUX_PERIOD;      // Set timer count for mux
    TA2CCTL0 = CCIE;            // enable CCR0 interrupt
    TA2CTL = TASSEL__ACLK;
}
void adc_setup(void)
{
    P6SEL |= BIT1;             // set pin P6.1 as alternate function - A0 analog
    ADC12CTL0 &= ~ADC12ENC;     // disable while setting up
    /* 32 cycles for sampling, ref voltage is set by REF module, ADC ON */
    ADC12CTL0 |= ADC12ON;
    ADC12CTL1 |= ADC12SHS_1 + ADC12SSEL_3 + ADC12CONSEQ_2;
    // start address ADC12MEM0, timer starts conv
    // single channel continuous conversion, SMCLK clock
    // SAMPCON sourced from the sampling timer
    ADC12CTL2 &= ~ADC12RES_2;      // 8 bit conversion result
    ADC12MCTL0 |= ADC12INCH_1;    //reference AVCC and AVSS, channel A0
    ADC12CTL0 |= ADC12ENC;        // enable ADC12
}
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    buttons_setup();
    output_setup();
    timer_setup();
    adc_setup();
    __enable_interrupt();       //GIE
    while (1)
    {

    }
}
void __attribute__ ((interrupt(PORT2_VECTOR))) P2ISR(void) //debounce, start
{
    if (P2IFG & BIT1)       // check if P2.1 flag is set
    {
        /* start timer */
        P2IE &= ~BIT1;              // disable P2.1 interrupt
        TA1CTL |= MC__UP;           // start debounce timer
        P2IFG &= ~BIT1;             // clear P2.1 flag
    }
}
void __attribute__ ((interrupt(PORT1_VECTOR))) P1ISR(void) //debounce, outputs
{
    if (P1IFG & BIT4)        // check if P1.4 flag is set
    {
        /* start timer */
        P1IE &= ~BIT4;              // disable P1.4 interrupt
        TA1CTL |= MC__UP;           // start debounce timer
        P1IFG &= ~BIT4;             // clear P1.4 flag
    }
    if (P1IFG & BIT1)        // check if P1.1 flag is set
    {
        /* start timer */
        P1IE &= ~BIT1;              // disable P1.1 interrupt
        TA1CTL |= MC__UP;           // start debounce timer
        P1IFG &= ~BIT1;             // clear P1.1 flag
    }
    if (P1IFG & BIT5)        // check if P1.5 flag is set
    {
        /* start timer */
        P1IE &= ~BIT5;              // disable P1.5 interrupt
        TA1CTL |= MC__UP;           // start debounce timer
        P1IFG &= ~BIT5;             // clear P1.5 flag
    }
}
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TA1CCR0ISR(void) //debounce
{
    if (((P2IN & BIT1) == 0) && ~sampling)
    {
        TA1CTL &= ~(MC__STOP | MC__UP);        // stop timer
        TA1CTL |= TACLR;        // clear timer
        P2IFG &= ~BIT1;        //clear flag
        sampling = 1;
        sample_index = 0;        // reset sample index
        P1OUT |= BIT0;           // set output as 1 (LED on)
        TA0CTL |= MC__UP;        // Start Timer A0
    }
    if (((P1IN & BIT1) == 0) && ~sampling)
    {
        TA1CTL &= ~(MC__STOP | MC__UP);        // stop timer
        TA1CTL |= TACLR;        // clear timer
        P1IFG &= ~BIT1;        //clear flag
        P1IE |= BIT1;        //enable interrupt
        TA2CTL |= MC__UP;   //display timer
        unsigned int i = 0;
        output = 0;
        for (i = 0; i < 200; i++)
        {
            output += samples[i];
        }
        output = output / 200;
    }
    if (((P1IN & BIT4) == 0) && ~sampling)
    {
        TA1CTL &= ~(MC__STOP | MC__UP);        // stop timer
        TA1CTL |= TACLR;        // clear timer
        P1IFG &= ~BIT4;        //clear flag
        P1IE |= BIT4;        //enable interrupt
        TA2CTL |= MC__UP;   //display timer
        unsigned int i = 0;
        output = 0;
        for (i = 0; i < 200; i++)
        {
            if (samples[i] > output)
            {
                output = samples[i];
            }
        }
    }
    if (((P1IN & BIT5)) == 0 && ~sampling)
    {
        TA1CTL &= ~(MC__STOP | MC__UP);        // stop timer
        TA1CTL |= TACLR;        // clear timer
        P1IFG &= ~BIT5;        //clear flag
        P1IE |= BIT5;        //enable interrupt
        TA2CTL |= MC__UP;   //display timer
        unsigned int i = 0;
        output = 0xFF;
        for (i = 0; i < 200; i++)
        {
            if (samples[i] < output)
            {
                output = samples[i];
            }
        }
    }
}
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR(void) //sampling
{
    switch (ADC12IV)
    {
    case ADC12IV_ADC12IFG0:
        if (sample_index < 200 && sampling)
        {
            samples[sample_index++] = ADC12MEM0;    // saèuvaj podatak u nizu
            ADC12IFG &= ~ADC12IFG0; //briši flag
        }
        else
        {
            sampling = 0;
            TA0CTL &= ~(MC__STOP | MC__UP);        // stop timer
            TA0CTL |= TACLR;        // clear timer
            P1OUT &= ~BIT0;  // LED OFF
            ADC12CTL0 &= ~ADC12ENC; //disable ADC
            ADC12IFG &= ~ADC12IFG0; //briši flag
            P2IE |= BIT1;        //enable interrupt
        }
        break;
    default:
        break;
    }
}
void __attribute__ ((interrupt(TIMER2_A0_VECTOR))) TA2CCR0ISR(void) //screen mux
{
    static uint8_t current_digit = 0;
    /* algorithm:
     * - turn off previous display (SEL signal)
     * - set a..g for current display
     * - activate current display
     */
    cifre[1] = (output >> 4) & 0x0F;// deljenje sa 16, F skida višak cifre
    cifre[0] = output & 0x0F;   // skidanje viših cifara
    if (current_digit)
    {
        P6OUT |= BIT4;          // turn off SEL2
        WriteLed(cifre[current_digit]);    // define seg a..g
        P7OUT &= ~BIT0;         // turn on SEL1
    }
    else
    {
        P7OUT |= BIT0;          // turn off SEL2
        WriteLed(cifre[current_digit]);     // define seg a..g
        P6OUT &= ~BIT4;         // turn on SEL1
    }
    current_digit = (current_digit + 1) & 0x01; // 0 i 1
    return;
}
