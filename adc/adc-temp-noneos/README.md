This example is to use the internal Temperature Sensor

This sensor is on the Channel 16 (NTC Thermistor)

Datasheet says it needs to be enabled `TSVREFE` bit in the `ADC_CTLR2` register,
this is made in code with:
```
ADC_TempSensorVrefintCmd(ENABLE);
```

Datasheet says recommended time is `17.1us` for this sensor,
as the bus clock runs on `12Mhz` each cycle is `0.08us` so needs
at least `213 cycles` in the configuration, so the nearest option is
`239 cycles`

The datasheet have the following values:
- Range: `-40C` to `85C`
- `V25` (Voltage at `25C`) = `1.40v`
- `Avg_Slope` = `4.3mV/C`

The formula in the datasheet is:
```
((VSENSE-V25)/Avg_Slope)+25
```

which doesn't make any sense =)

As the temperature sensor can read from `-40C` to `85C`
The range is `125C = 85 - (-40)`

So we can just calculate the max value in `12bit` resolution:
```
4096 / 125C = 32.76
```

So if we do:
```
temp = value / 32.76
```
we should get the temperature, so we need to adjust in the range this value
and since this is a NTC thermistor, it means higher temperature will have
less resistance, so the value will go from hot to cold, meaning range goes
from `85C` to `-45C` so the calculation is
```
temp = 85 - (temp / 32.76)
```

Now the `Avg_Slope` means that for each `C` calculated there is `4.3mV` of error

So we need to calculate the total of `mV` in the range.

The max voltage captured should be `2.69v`.
Datasheet says that `25C (V25) = 1.40v` so in relative terms:
`25C = 65C` (`40C` from negative range + `25`) and `65C / 125C = 0.52`
so `1.40v / 0.52 = 2.69v`

If we make into `mV` it turns that each `C` is around:
`2690mV / 125C = 21.52mV/C` if we have an error of `4.3mV/C` it means the error is:
`4.3mV / 21.52mV = 0.199 ~ 0.2`

So there is around `20%` of error in the sample; so it means we need to divide
by `1.2` to account this error; so the final formula is:
```
temp = (85 - (temp / 32.76))/1.2
```

a pure integer division approximation can be:
```
temp = 71 - temp / 39
```

for faster operations

## Example

Output
```
Starting ADC test
Starting calibration
Calibration finished
Calibration value: -1
Starting ADC measures
Temperature: raw: 1735, float: 26.70C, int: 27C
Temperature: raw: 1733, float: 26.75C, int: 27C
Temperature: raw: 1733, float: 26.75C, int: 27C
Temperature: raw: 1733, float: 26.75C, int: 27C
Temperature: raw: 1732, float: 26.78C, int: 27C
Temperature: raw: 1732, float: 26.78C, int: 27C
```