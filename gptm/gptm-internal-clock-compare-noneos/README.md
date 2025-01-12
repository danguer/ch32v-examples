# Timer compare

The compare part of the timer works by setting the `CCyS` mode to output: `00`
(default value) and setting the Capture/Compare Register (`CHyCVR` register).

If enabled (`CCyG` bit in `TIMx_SWEVGR` register) will generate an interrupt when
compare is matched to counter value.

## Output signal

To enable the output, there are two parts, one is the Output Mode `OCyM` bits in the
`TIMx_CTLR1` register (`TIM_OCMode` field in the SDK) this becomes the `OCyREF` signal,
there is also a Polarity bit, `CCyP` bit in the `TIMx_CCER` register
(`TIM_OCPolarity` in the SDK) which selects to use the high (default) or low level;
finally there is a Enable bit `CCyE` in the `TIMx_CCER` (`TIM_OutputState` in the
SDK) to allow the signal to go into `TIMx_CHy` (by default is disabled).

The Output Mode can be (excluding PWM options):
- `000` *Frozen*, this disable any output signal (no change on `OCyREF`).
- `001` *Active*, When counter matches Capture/Compare register `OCyREF` becomes high.
- `010` *Inactive*, When counter matches Capture/Compare register `OCyREF` becomes low.
- `011` *Toggle*, When counter matches Capture/Compare register `OCyREF` toggles between high and low.
- `100` *Always Active*, `OCyREF` is always high.
- `101` *Always Inactive*, `OCyREF` is always low.

It's important to note that on *Active* and *Inactive* once the Counter matches Capture/Compare it will remain in the same state (low or high) to show there was a match.

The toggle (used in the example) will change from low to high (and viceversa) when the
counter matches.

## Interrupt

The match in the comparator between Counter and Capture/Compare will generate an interrupt
(if enabled) no matter of the Output Signal configuration.

# GPIO
For each channel there is a GPIO associated, so in the case of the `TIM2_TI1` (output pin of
channel 1, timer 2) the documentation says is the `PA0`, for output this needs to be in
Alternate Function and Push-Pull, so in the SDK this option is: `GPIO_Mode_AF_PP` and set
a speed, in this case `GPIO_Speed_10MHz`.

It needs to enable the GPIO clock (`APB2`) and configure this pin.

## Example

The example sets the timer to run for one second, the compare will happen at 500ms and will
output a toggle signal in the pin, so the first 500ms the signal will be low, then will
toggle to high and after a second will toggle to low (500ms of the remaining clock going
from 500ms to 1s and after restart of the clock going from 0ms to 500ms).

If you connect a led in the `PA0` pin you will see a blink example each 1s.

There is also an interrupt enabled so each time the counter matches 500ms it will print
a message.

Output:
```
Compare example
Compare worked 5000
Compare worked 5000
```

