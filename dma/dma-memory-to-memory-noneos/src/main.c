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

// 248 plus 1 for the terminator character
#define BUFFER_LENGTH 249
char buffer_input[BUFFER_LENGTH];
char buffer_output[BUFFER_LENGTH];

void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void enable_interrupt()
{
    NVIC_InitTypeDef NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // allow interrupt flag on DMA1 and Channel1 for Half and Full Transfer interrupts
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC | DMA_IT_HT, ENABLE);
}

void DMA1_Channel1_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC1) == SET) {
        printf("DMA Completed interrupt\n");
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }

    if (DMA_GetITStatus(DMA1_IT_HT1) == SET) {
        printf("DMA Half transfer interrupt\n");
        DMA_ClearITPendingBit(DMA1_IT_HT1);
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // enable clock for DMA1
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    enable_interrupt();
    printf("Starting DMA test\n");

    // create string for source buffer
    for (int i=0; i<BUFFER_LENGTH; ) {
        // uppercase
        int j;
        for (j=65; j<=90; j++) {
            buffer_input[i++] = j;
        }

        //lowercase
        for (j=97; j<=122; j++) {
            buffer_input[i++] = j;
        }

        for (j=48; j<=57; j++) {
            buffer_input[i++] = j;
        }
    }

    // terminator string
    buffer_input[BUFFER_LENGTH-1] = 0;

    // init destination buffer to zero
    for (int i=0; i<BUFFER_LENGTH; i++) {
        buffer_output[i] = 0;
    }

    printf("Buffer input : %s\n", buffer_input);
    printf("Buffer output: %s\n\n\n", buffer_output);

    // just in case, stop any DMA1 Channel 1 process
    DMA_Cmd(DMA1_Channel1, DISABLE);

    // set params for request
    DMA_InitTypeDef dma_request;
    DMA_StructInit(&dma_request);

    // if direction is not set, by default goes from Peripheral to Memory
    // so Peripheralbase is the Source Address
    // and MemoryBase is the Destination Address

    dma_request.DMA_PeripheralBaseAddr = (uint32_t)&buffer_input;
    dma_request.DMA_MemoryBaseAddr = (uint32_t)&buffer_output;
    // size of the string to copy
    dma_request.DMA_BufferSize = BUFFER_LENGTH;
    // allow memory to memory copy
    dma_request.DMA_M2M = DMA_M2M_Enable;

    // allow to increase memory address, if you comment the
    // DMA_PeripheralInc you will see it just copies first character
    dma_request.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    dma_request.DMA_MemoryInc = DMA_MemoryInc_Enable;

    // apply request to dma channel
    DMA_Init(DMA1_Channel1, &dma_request);

    printf("Starting DMA copy\n");
    // run dma operation
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // there are two ways to check if have finished
    // one is checking the complete flag:
    // while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET) {};
    // the other is checking the counter when reaches 0
    while (DMA1_Channel1->CNTR > 0) {};

    printf("DMA finished by checking counter register\n");
    printf("Buffer input : %s\n", buffer_input);
    printf("Buffer output: %s\n", buffer_output);
}