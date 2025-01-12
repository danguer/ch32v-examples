# Timer simple example

This is a simple example of using a Timer, in this case `TIM2`

The code should be the same for any timer as only setup the timer, reads manually
the counter and configure an interrupt.

By default the timer's input signal will be the internal clock, in the board
is a 8Mhz crystal, but internally is using `PLL x12` so the bus will have
a 96Mhz clock.

There are two parts: the `ATRLR` register which is labeled as Autoload Register,
but in practice this is check for the counter, in SDK this is the `TIM_Period` field.
And the `PSC` register which is a prescaler (`TIM_Prescaler` in SDK).

Both registers are 16 bits.

If you want to match the timer with seconds, the `PSC` should match the bus speed
in Hertz, so this should be `96000` which is bigger than the 16 bits register,
so instead it can be used `9600` and the `ATRLR` could count up to 6.5 seconds,
and in this scenario each `10000` will be one second.
There could be other combinations, the maximum in this setup for `ATRLR` and `PSC`
being `0xFFFF` it will be around 42 seconds.

The timer have two options: continuous or single time.
In the first when counter is the same as `ATRLR` it will just set to zero and
start again.
In the single time, the timer will be disabled after this happens.

In any case, when `ATRLR = CNT` (counter register) it will trigger an interrupt
in the timer IRQ, the example shows how to process the interrupt.

## Example

The example will print each 100ms the counter (when timer is enabled, so if enabled
single time use, counter won't be reset). Each 2 seconds it will print the
interrupt message (single time will print once).

```
Counter: 999 -> 1
Counter: 2016 -> 2
Counter: 3033 -> 3
Counter: 4050 -> 4
Counter: 5068 -> 5
Counter: 6085 -> 6
Counter: 7102 -> 7
Counter: 8119 -> 8
Counter: 9137 -> 9
Counter: 10154 -> 10
Counter: 11173 -> 11
Counter: 12192 -> 12
Counter: 13212 -> 13
Counter: 14231 -> 14
Counter: 15250 -> 15
Counter: 16269 -> 16
Counter: 17288 -> 17
Counter: 18308 -> 18
Counter: 19327 -> 19
Got tim2 interrupt
Counter: 345 -> 20
```

