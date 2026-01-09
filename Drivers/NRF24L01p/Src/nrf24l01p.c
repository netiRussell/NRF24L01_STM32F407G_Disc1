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
/*
static void centralized_errorHandler(UART_HandleTypeDef huart, uint8_t *error_msg, uint8_t msg_size) {
	// TODO: to be implemented
	if( HAL_UART_Transmit(&huart, error_msg, msg_size, HAL_MAX_DELAY) != HAL_OK){
		Error_Handler();
	}
} */

// TODO: Dummy handler, to be substituted with the one that takes in function to be invoked with the corresponding error code pass to it
static void centralized_errorHandler() {
	uint8_t dummy = 0;
	for(uint8_t i = 0; i < 3; i++){
		dummy++;
	}	
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
	if( HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &reg, 1, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	}

	// Transmit data over the SPI
	if( HAL_SPI_Transmit( &NRF24_SPI_HANDLER, data, size, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	}
	
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
	if( HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &reg, 1, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	}

	// Store the received data in the buffer
	if( HAL_SPI_Receive( &NRF24_SPI_HANDLER, buffer, size, 1000) != HAL_OK ){
		centralized_errorHandler();
	}
	
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
	if( HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &cmd, 1, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	}

	// Release NRF24
	NSS_Deselect();
}

/*
 * nrf24_get_status_with_nop - Reads the status register by sending NOP 
 * 
 * @return uint8_t: value of STATUS register 
*/
uint8_t nrf24_get_status_with_nop(){
	uint8_t status_buffer = 0b0;
	uint8_t cmd = NOP;

	// Enable listening on the NRF24's end by pulling NSS pin low (SPI logic)
	NSS_Select();

	// Send a NOP and save the data
	if( HAL_SPI_TransmitReceive( &NRF24_SPI_HANDLER, &cmd, &status_buffer, 1, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	}

	// Release NRF24
	NSS_Deselect();

	return status_buffer;
}

/*
 * nrf24_is_rx_data_available - Checks if RX data is available in the specified pipe 
 *
 * nrf24_config_t @nrf24_config_t: structure with the NRF24 configurations 
 * 
 * @return uint8_t: RX data availability in the specified pipe status, 1 = some data is inside, 0 = empty  
*/
uint8_t nrf24_is_rx_data_available(nrf24_config_t *nrf24_config){
	/* Get the value of the status register */	
	uint8_t status_buffer = nrf24_get_status_with_nop();

	/* Check on the RX data ready flag */
	if( (status_buffer >> NRF24_REG_STATUS_RX_DR_Pos) & 0b1 ){
		// Clear the RX_DR flag
		uint8_t holder = 0b1 << NRF24_REG_STATUS_RX_DR_Pos;
		nrf24_writeReg(NRF24_REG_STATUS, &holder, 1);

		/* Get pipe-specific flag */
		status_buffer = (status_buffer >> NRF24_REG_STATUS_RX_P_NO_Pos) & 0b111;

		if( status_buffer == nrf24_config->rx_pipe ){
			return 1;
		} 
	}

	return 0;
} 


/* --- Init APIs --- */
// TODO: apply asserts in init
// TODO: finish the centralized error handler
// TODO: ensure that the SPI CPOL, CPHA match NRF24l01+'s configs 
// TODO: esnure that all LSBfirst and MSB first registers are accessed correctly
// TODO: when implementing interrupts, handle the HAL_BUSY return caused by multiple SPI transmissions at the same time
/*
 * nrf24_Init - Initializes the NRF24l01+ module in the polling SPI manner
 *
 * nrf24_config_t @nrf24_config: structure with the NRF24 configurations 
 * 
 * @return: void
 */
void nrf24_Init( nrf24_config_t* nrf24_config ){
	/* Initialize the variable that will hold the values to be written to the registers */
	uint8_t holder = 0b0;

	/* Ensure that the NSS pin is initially HIGH for the first SPI communication with NRF24 */
	NSS_Deselect();
	HAL_Delay(2);

	/* Disable NRF24 before modifying its registers */
	CE_Disable();

	/* Asserts */
	// NSS is disabled(high)
	custom_assert( HAL_GPIO_ReadPin(NRF24_NSS_PORT, NRF24_NSS_PIN) == GPIO_PIN_SET );
	
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

	// Final write to the RF_SETUP register
	nrf24_writeReg(NRF24_REG_RF_SETUP, &holder, 1);

	/* RX specific (only when the mode is RX) */
	if( nrf24_config->mode ) {
		/* EN_AA - Specify ACKing for the chosen RX pipe */
		holder = (uint8_t)(nrf24_config->en_aa << nrf24_config->rx_pipe);
		nrf24_writeReg(NRF24_REG_EN_AA, &holder, 1);

		/* EN_RXADDR - Enable the pipe specified in the config */
		holder = (uint8_t)(NRF24_REG_EN_RXADDR_ERX_Px_Val_ENABLE << nrf24_config->rx_pipe);
		nrf24_writeReg(NRF24_REG_EN_RXADDR, &holder, 1);

		/* RX_ADDR_Px - RX adress */
		// Pipe#0 case
		if(nrf24_config->rx_pipe == NRF24_REG_EN_RXADDR_ERX_P0_Pos) {
			nrf24_writeReg(NRF24_REG_RX_ADDR_P0, nrf24_config->rx_addr, 5);
		} else{
			// Fill-in the pipe#1 since it stores the first 4 address bytes for any other pipe
			nrf24_writeReg(NRF24_REG_RX_ADDR_P1, nrf24_config->rx_addr, 5);

			// Get register address of the corresponding pipe
			// Pipe#1 address + (specified pipe - pipe#1); e.g.,pipe#3: 0x0B+(3-1)=0x0D
			uint8_t pipe_reg_addr = NRF24_REG_RX_ADDR_P1 + nrf24_config->rx_pipe-NRF24_REG_EN_RXADDR_ERX_P1_Pos;

			// Get LSB of the RX address
			holder = (uint8_t)(nrf24_config->rx_addr[0]);

			// Final write to RX_ADDR_Px. i.e., Copy the LSB to the corresponding pipe
			nrf24_writeReg(pipe_reg_addr, &holder, 1);			
		}

		/* RX_PW_Px - size of the data in bytes */
		// Get register address of the corresponding pipe
		// Pipe#0 address + (spcified pipe - pipe#0); e.g., pipe#5: 0x11+(5-0) = 0x16
		holder = NRF24_REG_RX_PW_P0  + nrf24_config->rx_pipe-NRF24_REG_EN_RXADDR_ERX_P0_Pos;

		// Final write to RX_PW_Px
		nrf24_writeReg(holder, &nrf24_config->data_width, 1);
	}

	/* TX specific (only when the mode is TX) */
	else {
		/* EN_AA - Specify ACKing for the pipe#0, PTX requirement */
		holder = (uint8_t)(nrf24_config->en_aa ? 0b1 : 0b0);
		nrf24_writeReg(NRF24_REG_EN_AA, &holder, 1);

		/* TX Re-transmission */
		holder = 0b0;

		// ARC
		holder |= nrf24_config->arc << NRF24_REG_SETUP_RETR_ARC_Pos;

		// ARD
		holder |= nrf24_config->ard << NRF24_REG_SETUP_RETR_ARD_Pos;
		
		// Final write to the SETUP_RETR register
		nrf24_writeReg(NRF24_REG_SETUP_RETR, &holder, 1);

		/* TX address */
		nrf24_writeReg(NRF24_REG_TX_ADDR, nrf24_config->tx_addr, 5);

		/* RX pipe 0 address for ACKing */
		nrf24_writeReg(NRF24_REG_RX_ADDR_P0, nrf24_config->tx_addr, 5 );
	}

	/* Config register */
	holder = 0b0;

	// Mode
	holder |= nrf24_config->mode << NRF24_REG_CONFIG_PRIM_RX_Pos;

	// Power up
	holder |= NRF24_REG_CONFIG_PWR_UP_Val_UP << NRF24_REG_CONFIG_PWR_UP_Pos;

	// Enable CRC
	holder |= nrf24_config->en_crc << NRF24_REG_CONFIG_EN_CRC_Pos;

	// Mask MAX_RT
	holder |= nrf24_config->max_rt_iqr << NRF24_REG_CONFIG_MASK_MAX_RT_Pos;

	// Mask TX_DS
	holder |= nrf24_config->tx_iqr << NRF24_REG_CONFIG_MASK_TX_DS_Pos;
	
	// Mask RX_DR
	holder |= nrf24_config->rx_iqr << NRF24_REG_CONFIG_MASK_RX_DR_Pos;		

	// Final write to the CONFIG register
	nrf24_writeReg(NRF24_REG_CONFIG, &holder, 1);

	/* Enable the NRF24 module */
	HAL_Delay(2);
	CE_Enable();
}

/*
 * nrf24_Transmit - Transmits data over the NRF24l01+ module in the polling SPI manner
 *
 * nrf24_config_t @nrf24_config: structure with the NRF24 configurations 
 * uint8_t @data: payload of size == nrf24_config->data_width
 *
 * @return uint8_t: Whether maximum # of retransmissions was reached or the data was successfully sent with ACKing(if it was enabled), 1=true, 0=false
 */
uint8_t nrf24_Transmit(nrf24_config_t* nrf24_config, uint8_t *data){
	/* Transmit data */
	// Select the NRF24 module
	NSS_Select();

	// Send the "write tx payload" command
	uint8_t cmd = W_TX_PAYLOAD;
	if( HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &cmd, 1, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	}

	// Send the data
	if( HAL_SPI_Transmit( &NRF24_SPI_HANDLER, data, nrf24_config->data_width, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	}

	// Deselect the NRF24 module
	NSS_Deselect();

	// Give the SPI transmission idle time to separate commands for the NRF24 module by keeping the NSS pin HIGH for 1ms
	HAL_Delay(1);

	/* Esnure the data was succesfully sent and ACK took place (if it is enabled) */
	uint8_t status_buffer = 0;
	uint8_t tx_ds = 0;
	uint8_t max_rt = 0;

	uint32_t start = HAL_GetTick();
	while(HAL_GetTick() - start <= 20){

		// Get the value of the STATUS register
		status_buffer = nrf24_get_status_with_nop();
		tx_ds = (status_buffer >> NRF24_REG_STATUS_TX_DS_Pos) & 0b1;
		max_rt = (status_buffer >> NRF24_REG_STATUS_MAX_RT_Pos) & 0b1;

		/* Transmission was successful */ 
		if( tx_ds == TRUE ){
			// Clear the flag and return success
			// No need to flush, the NRF24 module automatically takes care of that
			uint8_t holder = 0b1 << NRF24_REG_STATUS_TX_DS_Pos;
			nrf24_writeReg(NRF24_REG_STATUS, &holder, 1);

			return 1;
		/* Not successful, e.g., no ACK received */
		} else if(max_rt == TRUE){
				/* Maximum number of retransmissions has been reached */

				// Clear the flag
				// [WARNING] technically, it is not a success because ACK was never provided by any RX device
				uint8_t holder = 0b1 << NRF24_REG_STATUS_MAX_RT_Pos;
				nrf24_writeReg(NRF24_REG_STATUS, &holder, 1);

				// Flush TX FIFO
				cmd = FLUSH_TX;
				nrf24_sendStandaloneCmd(cmd);

				return 1;
		}
	}

	// Transmission wasn't succesful and the maximum # of retransmissions wasn't reached
 	return 0;
}

/*
 * nrf24_Receive - Reads data from the NRF24l01+ module in the polling SPI manner
 *
 * nrf24_config_t @nrf24_config: structure with the NRF24 configurations 
 * uint8_t @rx_buffer: RX buffer of size == nrf24_config->data_width
 *
 * @return uint8_t: success flag, 1 = success, 0 = failure
 */
 uint8_t nrf24_Receive(nrf24_config_t *nrf24_config, uint8_t *rx_buffer){
	/* Check if data is availabe in the specified pipe */
	if( nrf24_is_rx_data_available(nrf24_config) == FALSE ){
		return 0;
	}

	/* Receive data */
	// Select the NRF24 module
	NSS_Select();

	// Send the "read rx payload" command
	uint8_t cmd = R_RX_PAYLOAD;
	if( HAL_SPI_Transmit( &NRF24_SPI_HANDLER, &cmd, 1, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	} 

	// Retrieve the data from the buffer
	if( HAL_SPI_Receive( &NRF24_SPI_HANDLER, rx_buffer, nrf24_config->data_width, 1000 ) != HAL_OK ){
		centralized_errorHandler();
	}

	// Deselect the NRF24 module
	NSS_Deselect();

	// Give the SPI transmission idle time to separate commands for the NRF24 module by keeping the NSS pin HIGH for 1micro sec
	HAL_Delay(1);

	return 1;
 }
