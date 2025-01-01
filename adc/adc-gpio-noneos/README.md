This example shows how to use the ADC
It could be good to use a potentiometer, the out pin will be connected to PA2,
the first pin to GND and the third pin to 3v

The ADC works by asking to read data from a channel, there are 16 input channels
and two special channels (temperature and Vref) you need to check datasheet to
check which channel is the input and configure the GPIO pin for Analog Input
