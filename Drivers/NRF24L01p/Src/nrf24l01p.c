/*
 * The main C-file of the NRF24L01 library
 * Made by Ruslan Abdulin
 * With love
 * On Monday, November 3rd, 2025
 */

#include "nrf24l01p.h"

/* --- Local functions --- */

/*
* Chip enable, disable functions.
* 1 = Chip is enabled
* 0 = Chip is disabled
*/
static void CE_select( void ){
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_SET);
}

static void CE_deselect( void ){
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}

/*
* Slave select, deselect functions.
* 0 = Slave is selected
* 1 = Slave is deselected
*/
static void NSS_select( void ){
	HAL_GPIO_WritePin(NRF24_NSS_PORT, NRF24_NSS_PIN, GPIO_PIN_RESET);
}

static void NSS_deselect( void ){
	HAL_GPIO_WritePin(NRF24_NSS_PORT, NRF24_NSS_PIN, GPIO_PIN_SET);
}



/* --- General APIs --- */

/*
 * nrf24_writeReg - Writes @size # of data bytes to the @reg NRF24 register
 *
 * uint8_t @reg:		The 5bit register address: 000AAAAA
 * *uint8_t @data:	Data to be written to the register
 * uint8_t @size:		# of data bytes to be transmitted (size of the TX buffer)
 * 
 * @return: void
 */
void nrf24_writeReg( uint8_t reg, uint8_t* data, uint8_t size ){
	// Register. Write operation requires "001A AAAA" pattern
	// where "A"s are the 5 bit register address
	reg = reg | (0b1 << 5);

	// Enable listening on the NRF24's end by pulling NSS pin low (SPI logic)
	NSS_select();

	// Transmit register address over the SPI
	HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &reg, 1, 1000 );

	// Transmit data over the SPI
	HAL_SPI_Transmit( &NRF24_SPI_HANDLER, data, size, 1000 );
	
	// Release NRF24
	NSS_deselect();
}

/*
 * nrf24_readReg - Reads @size # of data bytes from the @reg NRF24 register
 *
 * uint8_t @reg:		The 5bit register address: 000AAAAA
 * *uint8_t @data:	Data to be written to the register
 * uint8_t @size:		# of data bytes to be received (size of the RX buffer)
 * 
 * @return: void
 */
void nrf24_readReg( uint8_t reg, uint8_t* buffer, uint8_t size ){
	// Enable listening on the NRF24's end by pulling NSS pin low (SPI logic)
	NSS_select();

	// Request data from the register
	HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &reg, 1, 1000 );

	// Store the received data in the buffer
	HAL_SPI_Receive( &NRF24_SPI_HANDLER, buffer, size, 1000);
	
	// Release NRF24
	NSS_deselect();
}

/*
 * nrf24_sendStandaloneCmd - Sends the @cmd command to the NRF24 module
 *
 * uint8_t @cmd: The standalone command(no data bytes) to be sent
 * 
 * @return: void
 */
void nrf24_sendStandaloneCmd( uint8_t cmd ){
	// Enable listening on the NRF24's end by pulling NSS pin low (SPI logic)
	NSS_select();

	// Request data from the register
	HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &cmd, 1, 1000 );

	// Release NRF24
	NSS_deselect();
}


/* --- Init APIs --- */