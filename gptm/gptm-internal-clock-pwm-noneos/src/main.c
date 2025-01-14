#if defined(CH32V00X)
#include <ch32v00x.h>
#elif defined(CH32V10X)
#include <ch32v10x.h>
#elif defined(CH32V20X)
#include <ch32v20x.h>
#elif defined(CH32V30X) || defined(CH32V31X)
#include <ch32v30x.h>
#elif defined (CH32X035)
#include <ch32x035.h>
#elif defined(CH32L10X)
#include <ch32l103.h>
#endif
#include <debug.h>

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // init clock part
    // tim2 is on the APB1 bus
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // enable GPIOA clock to use button for TIM2_CH1 input
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    printf("PWM example\n");

    // allow GPIO PA0 to be floating input to work
    // this is the default mapping
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    // documentation says that in output should be selected
    // Push-pull alternate output
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // init TIM2
    TIM_SetCounter(TIM2, 0);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF; // use all the cycle
	TIM_TimeBaseInitStructure.TIM_Prescaler = 10; // ~ 10Mhz cycle
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    // we need to create input capture on each second
    TIM_OCInitTypeDef TIM_OC1 = {0};
    TIM_OCStructInit(&TIM_OC1);

    // when CNT > Capture/Compare Register will set level to low
    TIM_OC1.TIM_OCMode = TIM_OCMode_PWM1;

    // this can also be enabled, in this case
    // when CNT > Capture/Compare Register will set level to high
    // but in the case of led will show as fading, but will show
    // more flickering on larger values (low light situation)
    // TIM_OC1.TIM_OCMode = TIM_OCMode_PWM2;

    // enable output
    TIM_OC1.TIM_OutputState = TIM_OutputState_Enable;

    // set a 50% duty cycle
    TIM_OC1.TIM_Pulse = 0x7FFF;

    while(1)
    {
        // increase to have around 65 levels to show
        for (uint16_t duty_cycle=3024; duty_cycle<=65535; duty_cycle+=1008) {
            printf("Setting cycle to: %u\n", duty_cycle);
            TIM_Cmd(TIM2, DISABLE);
            TIM_OC1.TIM_Pulse = duty_cycle;
            TIM_OC1Init(TIM2, &TIM_OC1);
            TIM_Cmd(TIM2, ENABLE);
            Delay_Ms(50); // wait some time before changing cycle
        }
    }
}