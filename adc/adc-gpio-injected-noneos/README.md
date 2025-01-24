This is similar to the GPIO ADC example.

Main difference are changing some methods names and flags:

Trigger manual capture using following flag:
```
// from
// adc_request.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
// to
adc_request.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
```

As the injected channel group can have 4 different channels to read the
data it needs to set how many channels will be captured and read from
a specific channel.

To specify how many channels:
```
ADC_InjectedSequencerLengthConfig(ADC1, 1);
```

To get the value for channel 1:
```
ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
// instead of
// ADC_GetConversionValue(ADC1);
```

Other changes are:
- Channel config change `ADC_RegularChannelConfig` to `ADC_InjectedChannelConfig`
- Flag to start software capture change `ADC_SoftwareStartConvCmd` to `ADC_SoftwareStartInjectedConvCmd`
- Flag for checking finish of ADC change `ADC_FLAG_EOC` to `ADC_FLAG_JEOC`
