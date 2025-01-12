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

    printf("Capture example\n");

    // allow GPIO PA0 to be floating input to work
    // this is the default mapping
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // init TIM2
    TIM_SetCounter(TIM2, 0);

    // as the bus speed is around 96Mhz (8Mhz * 12 (PLL)) = 96Mhz
    // so TIM_Period 10000 will be 1s
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = 50000; // 5 seconds
	TIM_TimeBaseInitStructure.TIM_Prescaler = 9600;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    // we need to create input capture on each second
    TIM_ICInitTypeDef TIM_IC1 = {0};
    TIM_ICStructInit(&TIM_IC1);

    // use channel1
    TIM_IC1.TIM_Channel = TIM_Channel_1;

    // following two actions are to try to prevent bounce
    // so you need to click the button a bit more
    // this is just an example to test the settings
    // so bounce should be handled in a different way

    // this select that there should be at least 8 samples
    // before triggering the signal
    TIM_IC1.TIM_ICFilter = 0x3;

    // it needs another 8 times to trigger the signal
    TIM_IC1.TIM_ICPrescaler = TIM_ICPSC_DIV8;

    TIM_ICInit(TIM2, &TIM_IC1);

    TIM_Cmd(TIM2, ENABLE);

    int total = 0;
    uint16_t capture_register = TIM2->CH1CVR;
    uint16_t capture_check = 0;
    while(1)
    {
        capture_check = TIM2->CH1CVR;
        if (capture_register != capture_check) {
            printf(
                "Capture register new value: %u, previous: %u\n",
                capture_check,
                capture_register
            );
            capture_register = capture_check;
        }

        // each 2 seconds force a software capture
        if (total % 200 == 0) {
            if (TIM2->CTLR1 & TIM_CEN) {
                printf(
                    "Forcing capture, counter: %u\n",
                    TIM_GetCounter(TIM2)
                );

                // we need to enable CC1G to generate a capture event
                // this will be reset by hardware
                TIM2->SWEVGR |= 0x2;
            }
        }

        total++;
        Delay_Ms(10);
    }
}