# NRF24L01 
Configurable driver with Interrupt handling, dynamic payload size, error handling, optional asserts for the STM32F407G-Disc1 board. The driver is easy to modify to make it compatible with other STM32 boards. Only one pipe RX is employed.

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

## Notes
### ShockBurst
- ShockBurst feature provides reliable frameworkd for ACKing and dynamic payload size (DPL).
- In accordance with the NRF24L01+ documentation, to enable ShockBurst, set nrf24_config->EN_AA to NRF24_REG_EN_AA_ENAA_Px_Val_ENABLE and nrf24_config->ARC to anything but NRF24_REG_SETUP_RETR_ARC_Val_DISABLE during the peripheral initialization.
### ACKing
- Auto-Acknowledgement can be enabled/disabled in the config struct
### RX
- Pipe can be specified in the config struct
- rx_addr and rf_chl must have the same value as the device that will transmit data to the NRF24 module
### TX
- ACKing address for the pipe#0 is provided
- Retransmission count and delay when no ACK received can be specified in the config struct
- tx_addr and rf_chl must have the same value as the device that will receive data from the NRF24 module