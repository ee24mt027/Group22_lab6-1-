#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

//A 100kHz PWM varying duty cycle waveform on Pin PF2; controlled by Module 1 PWM Generator 3 A.
//SW2 for duty cycle increase and SW1 for decrease
//Interrupt Handlers
void GPIOFHandler(void);
void SysTickHandler(void){}

void GPIO_PORTF_setup(void);
void PWMConfig(void);
void dual_switch(void);


volatile int duty;



void GPIO_PORTF_setup(void)
{
    SYSCTL_RCGCGPIO_R |= (1<<5);
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;
    GPIO_PORTF_PUR_R = 0x11;
    GPIO_PORTF_DEN_R = 0x1F;
    GPIO_PORTF_DIR_R = 0x0E;

    GPIO_PORTF_IM_R &= ~0x11;
    GPIO_PORTF_IS_R &= ~0x11;
    GPIO_PORTF_IBE_R &= ~0x11;
    GPIO_PORTF_IEV_R &= ~0x11;


    GPIO_PORTF_ICR_R |= 0x11;
    GPIO_PORTF_IM_R |= 0x11;


    NVIC_PRI7_R &= 0xFF3FFFFF;

    NVIC_EN0_R |= 1 << 30;

}

void PWMConfig(void)
{
    SYSCTL_RCGCPWM_R |= (1<<1);     // Enable PWM1 clock
    GPIO_PORTF_AFSEL_R |= (1<<2);   //Enable alternate function
    GPIO_PORTF_PCTL_R |= 0x500;     //Make PF2 as PWM output


    PWM1_3_CTL_R |= 0x00;
    PWM1_3_GENA_R = 0x8C;
    PWM1_3_LOAD_R = 160;
    PWM1_3_CMPA_R = (duty/100)*160 - 1;
    PWM1_3_CTL_R |= 0x01; //Enable generator 3 counter
    PWM1_ENABLE_R |= 0x040; // Enable PWM1 channel 6 Output
}

void main(void)

{
    GPIO_PORTF_setup();
    PWMConfig();
    duty=50;
    PWM1_3_CMPA_R = (160 * duty) / 100;
    while(1){

    }
}


