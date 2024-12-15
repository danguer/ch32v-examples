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

void RTCAlarm_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void RTC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void enable_overflow_interrupt()
{
    NVIC_InitTypeDef NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    // note that alarm also will be enabled through enable_alarm_interrupt
    // left here to make it clear which one is
    // is also possible to enable the Second Flag interrupt with:
    // RTC_ITConfig(RTC_IT_OW | RTC_IT_SEC, ENABLE);

	RTC_ITConfig(RTC_IT_OW, ENABLE);
}

void enable_alarm_interrupt()
{
    NVIC_InitTypeDef NVIC_InitStruct;
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // enable interrupts
    NVIC_InitStruct.NVIC_IRQChannel = RTCAlarm_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
}

void RTCAlarm_IRQHandler(void) {
    // this is important to avoid keeping calling this method
    // so it ACK the interrupt has been processed
    EXTI_ClearFlag(EXTI_Line17);

	printf("Alarm called\n");
}

void RTC_IRQHandler(void) {
    if (RTC_GetFlagStatus(RTC_FLAG_OW) == SET) {
	    printf("Got counter overflow\n");
    } else if (RTC_GetFlagStatus(RTC_FLAG_ALR) == SET) {
        printf("Got alarm through RTC_IRQHandler\n");
    } else {
        printf("Second interrupt\n");
    }

    // need to clear the mask for either alarm or overflow or will keep calling to this function
    RTC_ClearITPendingBit(RTC_IT_OW | RTC_IT_ALR);

    // note that this also works the same:
    // RTC_ClearFlag(RTC_FLAG_OW | RTC_FLAG_ALR);

    // if you enable the Second Flag interrupt, then also do:
    // RTC_ClearITPendingBit(RTC_IT_OW | RTC_FLAG_ALR | RTC_IT_SEC);
}

void rtc_clock_init() {
    printf("Initializing RTC clock\n");

    // Enable clock for backup interface
    RCC->APB1PCENR |= RCC_APB1Periph_BKP | RCC_APB1Periph_PWR;

    // Enable to write to backup domain
    PWR->CTLR |= PWR_CTLR_DBP;

    // Enable external low-speed oscillator and select RTC clock source
    RCC->BDCTLR = RCC_LSEON | RCC_RTCSEL_LSE;

    // Enable RTC (must be RTCSEL != 0)
    RCC->BDCTLR |= RCC_RTCEN;

    // Wait for LSE clock gets ready
    while ((RCC->BDCTLR & RCC_LSERDY) == 0) {};
	printf("RTC Initialized\n");
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    rtc_clock_init();

    printf("Entering rtc config\n");

    RTC_EnterConfigMode();

    // this is not totally needed, but have found is better to keep it
    printf("Waiting for CNF to go off\n");
    while((RTC->CTLRL & RTC_CTLRL_CNF) == (uint16_t)RESET) {}

    RTC_SetCounter(0xFFFFFFF0);
    RTC_SetAlarm(0xFFFFFFF5);
    RTC_ExitConfigMode();

    // wait until changes are propagated
    RTC_WaitForLastTask();
    printf("Exit rtc config\n");

    enable_alarm_interrupt();
    enable_overflow_interrupt();
    printf("Starting main body\n");

    while(1)
    {
    	printf("Counter: %lx\n", RTC_GetCounter());
    	Delay_Ms(500);
    }
}