#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"


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

void Portf_interrupt_handler(void)
{
    GPIO_PORTF_IM_R &= ~0x11;

           if(GPIO_PORTF_RIS_R & 0x10)   //switch_2
           {
               if (duty < 90)
                      {
                          duty = duty + 5;
                      }
               if (duty >= 90){
                   duty = 90;
               }
           }
           if (GPIO_PORTF_RIS_R & 0x01)    //switch_1
           {
               if (duty > 5)
                      {
                          duty = duty - 5;
                      }
               if (duty <= 5){
                   duty = 5;
               }
           }
           if (GPIO_PORTF_RIS_R & 0x11)    //Both switches pressed at a time
           {
               duty = duty;
           }
           PWM1_3_CMPA_R = (160 * duty) / 100;

    int wait;
    for(wait = 0; wait <1600*1000/4; wait++){}           //Debounce Delay of 0.25seconds

    GPIO_PORTF_ICR_R = 0x11;
    GPIO_PORTF_IM_R |= 0x11;
}

