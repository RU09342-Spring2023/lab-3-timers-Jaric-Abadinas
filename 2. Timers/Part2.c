/*
 * Part2.c
 *
 *  Created on: Feb 11, 2023
 *      Author: Jaric Abadinas
 *
 *      This code will need to change the speed of an LED between 3 different speeds by pressing a button.
 */

#include <msp430.h>

void gpioInit();
void timerInit();

void main(){

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();
    timerInit();

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(LPM3_bits | GIE);

}


void gpioInit(){
    // @TODO Initialize the Red or Green LED --> doing Red LED
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

    // @TODO Initialize Button 2.3

    // P2DIR &= ~BIT3;                       // Configure Pin 2.3 to an input
    P2REN |= BIT3;                        // Enable the pull up/down resistor for Pin 2.3
    P2OUT |= BIT3;                        // Pull-up resistor

    // Configure Button on P2.3 as input with pullup resistor
    P2IES |= BIT3;                          // P2.3 High --> Low edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled
    //P2IFG &= ~BIT3;                         // Clear interrupt flag
}

void timerInit(){
    // @TODO Initialize Timer B1 in Continuous Mode using ACLK as the source CLK with Interrupts turned on
    TB1CTL = TBSSEL_1 | MC_2;                 // ACLK, continuous mode
    TB1CCTL0 |= CCIE;                         // TBCCR0 interrupt enabled
    TB1CCR0 = 32768;
}


/*
 * INTERRUPT ROUTINES
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    // @TODO Remember that when you service the GPIO Interrupt, you need to set the interrupt flag to 0.
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG

    // @TODO When the button is pressed, you can change what the CCR0 Register is for the Timer. You will need to track what speed you should be flashing at.
    if (P2IES & BIT3) {
        if (TB1CCR0 == 32768) {
            TB1CCR0 = 16384;
        }
        else if (TB1CCR0 == 16384) {
            TB1CCR0 = 8192;
        }
        else {
            TB1CCR0 = 32768;
        }
        P2IES &= ~BIT3;
    }
    else if (!(P2IES & BIT3)) {
        P2IES |= BIT3;
    }
}


// Timer B1 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    // @TODO You can toggle the LED Pin in this routine and if adjust your count in CCR0.
    P1OUT ^= BIT0;
}


