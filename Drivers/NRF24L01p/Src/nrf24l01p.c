/*
 * The main C-file of the NRF24L01 library
 * Board: STM32F407G-Disc1
 * Made by Ruslan Abdulin
 * With love
 * On Monday, November 3rd, 2025
 */


/* Header file */
#include "../Inc/nrf24l01p.h"


/* --- Local functions --- */
static void custom_assert( int result );
static void centralized_errorHandler();
static void CE_Disable( void );
static void CE_Enable( void );
static void NSS_Select( void );
static void NSS_Deselect( void );

/*
* [WARNING] - this function might utilize serial output!
* NRF24_assert - NRF24, STM32F407G-Disc1 specific assert functions that checks if an expression is correct
* In case, the expression is False, NRF24_centralized_errorHandler is invoked.
*
* @return: void
*/
#ifdef NRF24_USE_ASSERTS 
static void custom_assert( int result ){
	if(result == FALSE){
		// TODO: send line number of the code or some other info to be printed out
		centralized_errorHandler();	
	}
}
#endif

/*
* [WARNING] - this function utilizes serial output!
* NRF24_centralized_errorHandler - Centralized error handler that is invoked by the NRF24_assert function
* Default beahvior: system outputs error-specific information and flashes the red LED.
*  
* @return: void 
*/
static void centralized_errorHandler(){
	// TODO: to be implemented
}


/*
* Chip enable, disable functions.
* 1 = Chip is enabled
* 0 = Chip is disabled
*/
static void CE_Enable( void ){
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_SET);
}

static void CE_Disable( void ){
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}

/*
* Slave select, deselect functions.
* 0 = Slave is selected
* 1 = Slave is deselected
*/
static void NSS_Select( void ){
	HAL_GPIO_WritePin(NRF24_NSS_PORT, NRF24_NSS_PIN, GPIO_PIN_RESET);
}

static void NSS_Deselect( void ){
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
	NSS_Select();

	// Transmit register address over the SPI
	HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &reg, 1, 1000 );

	// Transmit data over the SPI
	HAL_SPI_Transmit( &NRF24_SPI_HANDLER, data, size, 1000 );
	
	// Release NRF24
	NSS_Deselect();
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
	NSS_Select();

	// Request data from the register
	HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &reg, 1, 1000 );

	// Store the received data in the buffer
	HAL_SPI_Receive( &NRF24_SPI_HANDLER, buffer, size, 1000);
	
	// Release NRF24
	NSS_Deselect();
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
	NSS_Select();

	// Request data from the register
	HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &cmd, 1, 1000 );

	// Release NRF24
	NSS_Deselect();
}


/* --- Init APIs --- */
// TODO: implement asserts
// TODO: apply asserts in init
// TODO: ensure that the SPI CPOL, CPHA match NRF24l01+'s configs 
/*
 * nrf24_Init - Initializes the NRF24l01+ module in the polling SPI manner
 *
 * nrf24_config_t @nrf24_config: structure with the NRF24 configurations 
 * 
 * @return: void
 */
void nrf24_Init( nrf24_config_t* nrf24_config ){
	/* Initialize the variable that will hold the values to be written to the registers */
	uint8_t holder;

	/* Assert if NSS is disabled(high) */
	custom_assert( HAL_GPIO_ReadPin(NRF24_NSS_PORT, NRF24_NSS_PIN) == GPIO_PIN_SET );

	/* Disable NRF24 before modifying its registers */
	CE_Disable();

	/* Config register */
	holder = 0b0;
	
	// Power up
	holder |= NRF24_REG_CONFIG_PWR_UP_Val_UP << NRF24_REG_CONFIG_PWR_UP_Pos;

	// Mode
	holder |= nrf24_config->mode << NRF24_REG_CONFIG_PRIM_RX_Pos;

	// Enable CRC
	holder |= nrf24_config->en_crc << NRF24_REG_CONFIG_EN_CRC_Pos;

	// Mask MAX_RT
	holder |= nrf24_config->max_rt_iqr << NRF24_REG_CONFIG_MASK_MAX_RT_Pos;

	// Mask TX_DS
	holder |= nrf24_config->tx_iqr << NRF24_REG_CONFIG_MASK_TX_DS_Pos;
	
	// Mask RX_DR
	holder |= nrf24_config->rx_iqr << NRF24_REG_CONFIG_MASK_RX_DR_Pos;	
	
	// Final write to the config register
	nrf24_writeReg(NRF24_REG_CONFIG, &holder, 1);

	/* RX pipes (only when the mode is RX) */
	if( nrf24_config->mode ) {
		// Enable the pipe #0
		holder = (uint8_t)(NRF24_REG_EN_AA_ENAA_Px_Val_ENABLE << NRF24_REG_EN_AA_ENAA_P0_Pos);
		nrf24_writeReg(NRF24_REG_EN_AA, &holder, 1);

		// Enable ACKing for the pipe #0
		holder = (uint8_t)(NRF24_REG_EN_RXADDR_ERX_Px_Val_ENABLE << NRF24_REG_EN_RXADDR_ERX_P0_Pos);
		nrf24_writeReg(NRF24_REG_EN_RXADDR, &holder, 1);
	}

	/* TX Re-transmission (only when the mode is TX) */
	else {
		holder = 0b0;

		// ARC
		holder |= nrf24_config->arc << NRF24_REG_SETUP_RETR_ARC_Pos;

		// ARD
		holder |= nrf24_config->ard << NRF24_REG_SETUP_RETR_ARD_Pos;
		
		// Final write to the SETUP_RETR register
		nrf24_writeReg(NRF24_REG_SETUP_RETR, &holder, 1);
	}

	/* Address Width */
	holder = (uint8_t)(nrf24_config->address_width << NRF24_REG_SETUP_AW_Pos);
	nrf24_writeReg(NRF24_REG_SETUP_AW, &holder, 1);

	/* RF Channel */
	holder = (uint8_t)((nrf24_config->rf_chl) << NRF24_REG_RF_CH_RF_CH_Pos);
	nrf24_writeReg(NRF24_REG_RF_CH, &holder, 1);

	/* RF Setup */
	holder = 0b0;

	// RF power
	holder |= nrf24_config->rf_pwr << NRF24_REG_RF_SETUP_RF_PWR_Pos;
	
	// RF DR High
	holder |= nrf24_config->dr_high << NRF24_REG_RF_SETUP_RF_DR_HIGH_Pos;
	
	// PLL Lock
	holder |= nrf24_config->pll_lock << NRF24_REG_RF_SETUP_PLL_LOCK_Pos;
	
	// RF DR Low
	holder |= nrf24_config->dr_low << NRF24_REG_RF_SETUP_RF_DR_LOW_Pos;
	
	// Count Wave
	holder |= nrf24_config->count_wave << NRF24_REG_RF_SETUP_CONT_WAVE_Pos;

	// Final write to the RF_SETUP function
	nrf24_writeReg(NRF24_REG_RF_SETUP, &holder, 1);

	/* Enable the NRF24 */ 
	CE_Enable();
}
