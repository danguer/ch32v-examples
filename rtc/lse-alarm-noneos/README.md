The following shows how to use the `LSE` Clock for RTC, I'm using the board:
[WeActStudio.BluePill-Plus-CH32](https://github.com/WeActStudio/WeActStudio.BluePill-Plus-CH32)

This board contains a 32.768 Khz crystal so is only need to enable `LSEON` and `RCC_RTCSEL_LSE` (`01`) in the `RCC_BDCTLR`

And wait for `LSERDY` to become active to start using it.
