This example shows how to use Memory to Memory DMA
Once selected a DMA and Channel the following registers can be used (refer to section 11.3 in the Reference Manual):
- `CFGR` to set the params
- `CNTR` the count of bytes to transfer
- `PADDR` the peripheral address
- `MADDR` the memory address

The `CFGR` should have the following settings:
- `MEM2MEM=1`
- `MINC=1` so it will increase the memory address after each byte copied
- `PINC=1` so it will increase the peripheral address after each byte copied
- `DIR=0` this makes the peripheral address the source and the memory address the destination
- `HTIE=1` (optional) this enables to generate the interrupt when Half transfer happens
- `TCIE=1` (optional) this enables to generate the interrupt when Full transfer happens
- `EN=1` to enable the actual DMA


The output will be:
```
Starting DMA test
Buffer input : ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
Buffer output:


Starting DMA copy
DMA Half transfer interrupt
DMA Completed interrupt
DMA finished by checking counter register
Buffer input : ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
Buffer output: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```