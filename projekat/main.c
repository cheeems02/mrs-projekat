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
#define SECOND (32768)            // 1s
#define DEBOUNCE (164)             // 5ms
volatile unsigned int samples[NUM_SAMPLES] = { 0 };
volatile unsigned int sample_index = 0;
volatile unsigned int sampling = 0;
volatile uint8_t data = 0;
static volatile uint8_t cifre[2] = { 0 };
void buttons_setup(void)
{
    //buttons
    // configure button S1 - start
    P2REN |= BIT1;              // enable pull up/down
    P2OUT |= BIT1;              // set pull up
    P2DIR &= ~BIT1;             // configure P1.4 as in
    P2IES |= BIT1;              // interrupt on falling edge
    P2IFG &= ~BIT1;             // clear flag
    P2IE |= BIT1;               // enable interrupt
    // configure button S2 -  avg
    P1REN |= BIT1;              // enable pull up/down
    P1OUT |= BIT1;              // set pull up
    P1DIR &= ~BIT1;             // configure P1.4 as in
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
    P1DIR &= ~BIT5;             // configure P1.4 as in
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
    // initialize Timer A0 - sampling
    TB0CCR0 = SECOND / SAMPLE_RATE;      // Set timer count for 20Hz
    TB0CCR1 = 5;            //any number for test
    TB0CCTL0 = CCIE;            // enable CCR0 interrupt
    TB0CCTL1 |= OUTMOD_4;
    TB0CTL = TBSSEL__ACLK;
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
    //ADC
    P6SEL |= BIT1;             // set pin P6.1 as alternate function - A0 analog
    ADC12CTL0 &= ~ADC12ENC;     // disable while setting up
    ADC12CTL0 |= ADC12ON | ADC12MSC | ADC12SHT0_3;       // activate ADC, multiple samples and sample cycles
    ADC12MCTL0 |= ADC12INCH_1;  // input channel A0 = P6.1
    ADC12CTL1 |= ADC12CONSEQ_2;  // single channel repeat
    ADC12CTL1 |= ADC12SHS_2;    //trigger preko tajmera B0
    ADC12CTL1 |= ADC12SSEL_1;   // clock setup
    ADC12CTL0 |= ADC12ENC;      // enable conversion
    ADC12IE |= ADC12IE0;        // enable interrupt when MEM0 is written
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
    return 0;
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
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TA1CCR0ISR(void) //debounce
{
    if (((P2IN & BIT1) == 0) && ~(sampling))
    {
        TA1CTL &= ~(MC__STOP | MC__UP);        // stop timer
        TA1CTL |= TACLR;        // clear timer
        P2IFG &= ~BIT1;        //clear flag
        P2IE |= BIT1;        //enable interrupt
        sampling = 1;
        sample_index = 0;        // reset sample index
        P1OUT |= BIT0;           // set output as 1 (LED on)
        TB0CTL |= MC__UP;        // Start Timer A0
    }
    if (((P1IN & BIT1) == 0) && ~(sampling))
    {
        TA1CTL &= ~(MC__STOP | MC__UP);        // stop timer
        TA1CTL |= TACLR;        // clear timer
        P1IFG &= ~BIT1;        //clear flag
        P1IE |= BIT1;        //enable interrupt
        unsigned int i = 0;
        unsigned long output = 0;
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
        unsigned int i = 0;
        unsigned int output = 0;
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
        unsigned int i = 0;
        unsigned int output = 0xFFFF;
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
    switch(ADC12IV)
    {
    case ADC12IV_ADC12IFG0:
        if (sample_index < 200 && sampling)
        {
            samples[sample_index++] = ADC12MEM0;    // sa�uvaj podatak u nizu
        }
        else
        {
            sampling = 0;
            P1OUT &= ~BIT0;  // LED OFF
            ADC12CTL0 &= ~ADC12ENC; //disable ADC
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
    if (current_digit)
    {
        P6OUT |= BIT4;          // turn off SEL2
        WriteLed(cifre[current_digit]);    // define seg a..g
        P7OUT &= ~BIT0;         // turn on SEL1
    }
    else
    {
        P7OUT |= BIT0;
        WriteLed(cifre[current_digit]);
        P6OUT &= ~BIT4;
    }
    current_digit = (current_digit + 1) & 0x01;
    return;
}
void display(const uint8_t broj)
{
    //0x0F je za skidanje vi�ih cifara
    cifre[1] = (broj >> 4) & 0x0F;
    cifre[0] = broj & 0x0F;
}
