/*
 * Part3.c
 *
 *  Created on: Feb 12, 2023
 *      Author: FILL THIS IN
 *
 *      YOU NEED TO FILL IN THIS AUTHOR BLOCK
 */

#include <msp430.h>

void gpioInit();
void timerInit();

void main() {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();
    timerInit();

    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(LPM3_bits | GIE);
}


void gpioInit() {

    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

    P2REN |= BIT3;                          // Enable the pull up/down resistor for Pin 2.3
    P2OUT |= BIT3;                          // Pull-up resistor
    P2IES |= BIT3;                          // P2.3 High --> Low edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled

    P4REN |= BIT1;                          // Enable the pull up/down resistor for Pin 4.1
    P4OUT |= BIT1;                          // Pull-up resistor
    P4IES |= BIT1;                          // P4.1 High --> Low edge
    P4IE |= BIT1;                           // P4.1 interrupt enabled
}


void timerInit() {
    TB1CTL |= TBCLR;
    TB1CTL |= TBSSEL_1;
    TB1CTL |= MC_3;                 // ACLK, continuous mode
    TB1CCR0 = 8192;

    // Set up Timer Compare IRQ
    TB1CCTL0 |= CCIE;
    TB1CCTL0 &= ~CCIFG;
}


/*
 * INTERRUPT ROUTINES
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    int buttonPressDuration = 0;

    // @TODO Remember that when you service the GPIO Interrupt, you need to set the interrupt flag to 0.
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG

    // @TODO When the button is pressed, you can change what the CCR0 Register is for the Timer. You will need to track what speed you should be flashing at.
    if (P2IES & BIT3) {
        buttonPressDuration = 0;
        TB1CTL &= ~MC_3;
        P2IES &= ~BIT3;
    }
    else if (!(P2IES & BIT3)) {
        TB1CCR0 = buttonPressDuration * 32.768;
        TB1CTL |= TBCLR;
        TB1CTL |= MC_3;
    }
}


// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    // @TODO Remember that when you service the GPIO Interrupt, you need to set the interrupt flag to 0.
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG

    // @TODO When the button is pressed, you can change what the CCR0 Register is for the Timer. You will need to track what speed you should be flashing at.
    if (P4IES & BIT1) {
        TB1CCR0 = 8192;
        P4IES &= ~BIT1;
    }
}


// Timer B1 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    // @TODO You can toggle the LED Pin in this routine and if adjust your count in CCR0.
    P1OUT ^= BIT0;
    TB1CCTL0 &= ~CCIFG;
}




