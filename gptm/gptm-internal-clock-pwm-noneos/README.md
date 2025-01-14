# Timer PWM

This example is very similar to the compare example.
The main difference is the mode is `TIM_OCMode_PWM1` which is `110` and means
once the `Counter > Capture/Compare` it will set the level to low, otherwise
will be high.

So under this, the `TIM_Prescaler` (`TIMx_PSC` register) is the frequency
and the `TIM_Pulse` (`TIMx_CHyCVR` register) is the duty cycle.

The `TIM_Period` (`TIMx_ATRLR`) should be set to the maximum value `0xFFFF` but
that can be changed with proper calculations.

## Example

The example similar to the compare will use a led as output, it will have
65 transitions going from very dim to full light.