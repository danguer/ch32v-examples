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

uint16_t read_adc_value() {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // wait until ADC is completed
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {}

    // get value
    return ADC_GetConversionValue(ADC1);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // enable clock for ADC1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    // Bus runs at 96Mhz, so make it run at 12Mhz as the max ADC speed is 14Mhz
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    // reset ADC
    ADC_DeInit(ADC1);

    // just in case, stop any ADC operation
    printf("Starting ADC test\n");

    // set params for request
    ADC_InitTypeDef adc_request;
    ADC_StructInit(&adc_request);
    // ask to run manually
    adc_request.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_Init(ADC1, &adc_request);

    // enable ADC
    ADC_Cmd(ADC1, ENABLE);

    // start calibration
    printf("Starting calibration\n");
    ADC_BufferCmd(ADC1, DISABLE); //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    printf("Calibration finished\n");
    printf("Calibration value: %d\n", Get_CalibrationValue(ADC1));
    ADC_BufferCmd(ADC1, ENABLE); //disable buffer

    // enable temperature sensor and Vref
    ADC_TempSensorVrefintCmd(ENABLE);

    // select temperature input to feed into first channel


    printf("Starting ADC measures\n");
    // configure to read from temperature sensor
    // recommended time is 17.1us
    // as the bus clock runs on 12Mhz each cycle is 0.08us so needs at least 213 cycles
    ADC_RegularChannelConfig(
        ADC1,
        ADC_Channel_16, // channel 16 is the temperature sensor
        1,
        ADC_SampleTime_239Cycles5
    );

    for (int i=0; i<5; i++) {
        // datasheet says there is some setup time
        // doing this just to remove some values, but maybe is not needed in actual code
        read_adc_value();
    }


    // note that recommendation is to use average samples
    // I'm not doing this just to show simple operations
    // but is easier change to read samples first and later just
    // calculate temperature from the average value
    while(1) {

        uint16_t temp = read_adc_value();

        // to understand the values check the README.md
        float temp_real = (85 - (temp / 32.76))/1.2;
        int temp_int = 71 - temp / 39;
        printf("Temperature: raw: %u, float: %.2fC, int: %dC\n", temp, temp_real, temp_int);
        Delay_Ms(500);
    }
}