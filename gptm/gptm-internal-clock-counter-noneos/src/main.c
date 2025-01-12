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

// set this to 1 to enable a single time use (OPM)
// set to 0 to enable restarting and infinite cycle
#define SINGLE_TIME 0

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void enable_interrupt()
{
    NVIC_InitTypeDef NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // allow interrupt flag
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        printf("Got tim2 interrupt\n");
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
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

    enable_interrupt();

    // init TIM1
    TIM_SetCounter(TIM2, 0);

    // as the bus speed is around 96Mhz (8Mhz * 12 (PLL)) = 96Mhz
    // so TIM_Period 10000 will be 1s
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_TimeBaseInitStructure.TIM_Period = 20000; // 2 seconds
	TIM_TimeBaseInitStructure.TIM_Prescaler = 9600;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_Cmd(TIM2, ENABLE);

    if (SINGLE_TIME) {
        printf("Enabling single time use\n");
        TIM2->CTLR1 |= TIM_OPM;

        // because we have enabled in the past, we need to reset counter
        TIM_SetCounter(TIM2, 0);

        // after TIM_OPM is set, is needed to enable again to make it work
        // also note that OPM for some reason only works after TIM is enabled
        // so if you remove the previous TIM_Cmd won't work
        TIM_Cmd(TIM2, ENABLE);
    }

    int total = 0;
    while(1)
    {
        Delay_Ms(100);
        total++;
        if (TIM2->CTLR1 & TIM_CEN) {
            printf(
                "Counter: %d -> %d\n",
                TIM_GetCounter(TIM2),
                total
            );
        }
    }
}