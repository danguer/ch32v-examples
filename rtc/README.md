There are two ways to handle the alarm, one is through the RTC IRQ which is probably easier,
in that case it needs to remove the proper flags to ACK the interrupt

The other is using a EXTI interrupt line, which needs the RTC IRQ and also the EXTI setup,
this needs to clear the EXTI line to ACK the interrupt.

The RTC IRQ can contain three events:
- Overflow (when counter goes from 0xFFFF FFFF to 0)
- Alarm (when alarm register is equal to counter)
- Second Flag (anytime there is an overflow on the internal divider, by default this is each second)

The output of the script will be like this
```
Initializing RTC clock
RTC Initialized
Entering rtc config
waiting for CNF to go off
Exit rtc config
Starting main body
Counter: fffffff0
Counter: fffffff0
Counter: fffffff1
Counter: fffffff1
Counter: fffffff2
Counter: fffffff2
Counter: fffffff3
Counter: fffffff3
Counter: fffffff4
Counter: fffffff4
Counter: fffffff5
Counter: fffffff5
Got alarm through RTC_IRQHandler
Alarm called
Counter: fffffff6
Counter: fffffff6
Counter: fffffff7
Counter: fffffff7
Counter: fffffff8
Counter: fffffff8
Counter: fffffff9
Counter: fffffff9
Counter: fffffffa
Counter: fffffffa
Counter: fffffffb
Counter: fffffffb
Counter: fffffffc
Counter: fffffffc
Counter: fffffffd
Counter: fffffffd
Counter: fffffffe
Counter: fffffffe
Counter: ffffffff
Counter: ffffffff
Got counter overflow
Counter: 0
Counter: 0
```