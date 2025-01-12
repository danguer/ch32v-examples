The `ADTM` and `GPTM` timers have 4 channels each, and each channel has a capture or compare operation.

The capture is when you configure the channel as input, in this case when there is a signal
it will capture the value of the timer counter into to the Capture/Compare
Register (`CHxCVR` where `x` is the channel number).

The Figure 15-3 of the datasheet provides a diagram of how it works
In order to describe the parts used I will use the following letters
- `x` to refer the timer number, for example `TIMx` can be `TIM2`
- `y` to refer to the channel in the timer, for example `ICyF` can be `IC1F`
which are the bits configuration for channel 1, or `TIMx_CHy` can be `TIM2_CH1` which is the
channel 1 for `TIM2`

## GPIO
For each channel there is a GPIO associated, so in the case of the `TIM2_TI1` (input pin of
channel 1, timer 2) the documentation says is the `PA0`, this needs to be in floating mode
input and this pin will be used for input (capture) or output (compare).

So it needs to enable the GPIO clock (`APB2`) and configure this pin.

## Input part
- *Filter downcounter* `ICyF` located in registers `TIMx_CHCTLR1` (Channel 1,2) or `TIMx_CHCTLR2` (Channel 3,4). Only send the signal to next stage after repeated
a number of times; this signal is called `TIyFP1` in documentation
- *Input selection* `CCyS`, `00` means output (compare) and others are inputs,
`01` uses the signal from `TI1FP1` (for channel 1 and 2) or `TI3FP1` (for channel 3 and 4),
`10` uses the signal from `TI2FP1` (for channel 1 and 2) or `TI4FP1` (for channel 3 and 4),
`11` uses `TRC` input (from slave mode controller in the main timer configuration)
- *Divider* `ICyPSC` can divide the signal: `None`, 2, 4 or 8 times

The output of input signal is called `ICyPS`

## Compare part
Once the signal comes from the input part, there is an `OR` to enable the capture, it can be
from `ICyPS` or from the `CCyG` bit (`TIMx_SWEVGR` register), this bit will force a
software capture action when there is no input signal.

In the example each 2 seconds this bit is enabled to force a capture if there is no button
pressed, the code is `TIM2->SWEVGR |= 0x2` and this set the `CC1G` bit.

## Hardware

This needs to be connect a pulldown button to `PA0`, so when clicked will be `3v`

## Example

The code will setup the GPIO and TIM2, this is a `GPTM` timer but will work the same
for an `ADTM` timer.

If there is no signal, around each 2 seconds it will force a software capture enabling
the `CC1G` bit.

So by default you will see something like this

```
Capture example
Forcing capture, counter: 0
Capture register new value: 26, previous: 0
Forcing capture, counter: 20067
Capture register new value: 20097, previous: 26
Forcing capture, counter: 40141
Capture register new value: 40171, previous: 20097
Forcing capture, counter: 10218
Capture register new value: 10247, previous: 40171
```

As you can see each 2 seconds will force a software capture, since the `Forcing capture`
message is printing the counter and the actual capture happens later you will see there
is a difference in the actual counter, in this case is around 30 timer cycles.

If you click the button (since is using filtering and divider you should keep the
button pressed a bit longer) you will see multiple `Capture register` printed:

```
Forcing capture, counter: 12099
Capture register new value: 12129, previous: 42052
Capture register new value: 17357, previous: 12129
Forcing capture, counter: 32224
Capture register new value: 32254, previous: 17357
Capture register new value: 36780, previous: 32254
Capture register new value: 40595, previous: 36780
Forcing capture, counter: 2397
Capture register new value: 2426, previous: 40595
Forcing capture, counter: 22472
```
