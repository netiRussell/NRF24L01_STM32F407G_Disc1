# NRF24L01 
Driver with Interrupt handling for the STM32F407G-Disc1 board

## Initial setup
### Clock
- Input freq = 8Mhz, HSE clock, HCLK=168Mhz
- APB1 prescaler = /4
- APB2 prescaler = /2
### SPI
- HAL library for the SPI protocol
- SPI Baud Rate must be <= 8MBits/s
- Clock Prescaler chosen = /16
- Resulting Baud Rate = 5.25MBits/s 
### Pins
- PA5: SPI1 SCLK
- PA6: SPI1 MISO
- PA7: SPI1 MOSI
- PC4: CE
- PC5: NSS
- PB0: IRQ
(These can be modified in the header file: nrf24l01p.h)
### Power
- 3.3V DC
- Common ground
