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

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void enable_interrupt()
{
    NVIC_InitTypeDef NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // allow interrupt flag for channel1
    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
}

void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET) {
        printf("Compare match %u\n", TIM_GetCounter(TIM2));
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    }
}

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

    enable_interrupt();

    printf("Compare example\n");

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

    // as the bus speed is around 96Mhz (8Mhz * 12 (PLL)) = 96Mhz
    // so TIM_Period 10000 will be 1s
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = 10000; // 1 seconds
	TIM_TimeBaseInitStructure.TIM_Prescaler = 9600;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    // we need to create input capture on each second
    TIM_OCInitTypeDef TIM_OC1 = {0};
    TIM_OCStructInit(&TIM_OC1);

    // enabled with CNT = Capture/Compare Register
    TIM_OC1.TIM_OCMode = TIM_OCMode_Toggle;

    // enable output
    TIM_OC1.TIM_OutputState = TIM_OutputState_Enable;

    // set the time when want to output, it will be each 500ms
    TIM_OC1.TIM_Pulse = 5000;

    TIM_OC1Init(TIM2, &TIM_OC1);

    TIM_Cmd(TIM2, ENABLE);

    while(1)
    {
        // NOP
    }
}