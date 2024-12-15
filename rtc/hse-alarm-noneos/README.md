The following shows how to use the `HSE` Clock for RTC (reuse SysClk and no need of additional clock),
The documentation on using the `HSE` is that there is a divider depending on the MCU, in the case of
CH32V203 is a 128 divider, so to calculate the prescaler is just:
```
ClockInHertz*1000 / Divider
```
So for example
```
8000*1000 / 128 = 62500 (0xF424)
```
