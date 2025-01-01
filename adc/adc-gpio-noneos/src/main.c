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

uint16_t read_adc_value(uint8_t channel) {
    ADC_RegularChannelConfig(
        ADC1,
        channel,
        1,
        ADC_SampleTime_239Cycles5
    );

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // wait until ADC is completed
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {}

    // get value
    return ADC_GetConversionValue(ADC1);
}

void do_adc_calibration() {
    // start calibration
    printf("Starting calibration\n");
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    printf("Calibration finished\n");
    printf("Calibration value: %d\n", Get_CalibrationValue(ADC1));
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // enable clock for ADC1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // BUS runs at 96 Mhz and ADC only can run up to 14Mhz
    // so divide the bus frequency
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    // allow GPIO A2 to be analog input
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // reset ADC
    ADC_DeInit(ADC1);

    printf("Starting ADC GPIO\n");

    // set params for request
    ADC_InitTypeDef adc_request;
    ADC_StructInit(&adc_request);
    // ask to ADC run manually
    adc_request.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_Init(ADC1, &adc_request);

    // enable ADC
    ADC_Cmd(ADC1, ENABLE);

    // calibrate, this is suggested only after a reset
    do_adc_calibration();

    printf("Starting ADC measures\n");
    while(1) {
        // datasheet says PA2 is ADC_IN2 so is the Channel 2
        uint16_t value = read_adc_value(ADC_Channel_2);
        // ADC is 12 bit so max value is 4096
        float percent = value / 4096.0;
        printf("Value read: %u, %.2f\n", value, percent);
        Delay_Ms(500);
    }
}