#ifndef NRF24L01P_INC_NRF24L01P_H_
#define NRF24L01P_INC_NRF24L01P_H_

// Libraries to be used
#include "stm32f4xx_hal.h"  // HAL
#include "stm32f407xx.h"    //CMSIS
#include <stdint.h>



/* ----------------------------------------------------------- */
/* ----------------------- Structures ------------------------ */
/* ----------------------------------------------------------- */
typedef struct {
  uint8_t rx_iqr;         // @NRF24_REG_CONFIG_MASK_xx_Val
  uint8_t tx_iqr;         // @NRF24_REG_CONFIG_MASK_xx_Val
  uint8_t max_rt_iqr;     // @NRF24_REG_CONFIG_MASK_xx_Val
  uint8_t en_crc;         // @NRF24_REG_CONFIG_EN_CRC_Val
  uint8_t mode;           // @NRF24_REG_CONFIG_PRIM_RX_Val

  uint8_t address_width;  // @NRF24_REG_SETUP_AW_Val

  uint8_t ard;            // @NRF24_REG_SETUP_RETR_ARD_Func_Val_step250_max4000us() [TX-specific]
  uint8_t arc;            // Value between 0 and 15                                 [TX-specific]

  uint8_t rf_chl;         // 6 bits(0-63) frequency channel

  /* RF_SETUP is suggested to have default for everything
  beside rf_pwr and dr_high which can be set to maximum */
  uint8_t rf_pwr;         // @NRF24_REG_RF_SETUP_RF_PWR_Val
  uint8_t dr_high;     // @NRF24_REG_RF_SETUP_RF_DR_HIGH_Val
  uint8_t pll_lock;       // @NRF24_REG_RF_SETUP_PLL_LOCK_Val                       [FOR TESTING-ONLY]
  uint8_t dr_low;         // @NRF24_REG_RF_SETUP_RF_DR_LOW_Val
  uint8_t count_wave;     // @NRF24_REG_RF_SETUP_CONT_WAVE_Val
} nrf24_config_t;




/* ----------------------------------------------------------- */
/* ---------------- Functions declarations ------------------- */
/* ----------------------------------------------------------- */
void nrf24_writeReg( uint8_t reg, uint8_t* data, uint8_t size );
void nrf24_readReg( uint8_t reg, uint8_t* buffer, uint8_t size );
void nrf24_sendStandaloneCmd( uint8_t cmd );
void nrf24_Init( nrf24_config_t* nrf24_config );



/* ----------------------------------------------------------- */
/* ------------------------ General -------------------------- */
/* ----------------------------------------------------------- */
#define NRF24_USE_ASSERTS

// Redundant copy of HAL macros
#define GPIO_PIN_RESET  0b0u
#define GPIO_PIN_SET    0b1u

#define FALSE           0b0u
#define TRUE            0b1u


/* ----------------------------------------------------------- */
/* ------------------ STM32F407G1-specific ------------------- */
/* ----------------------------------------------------------- */

/* GPIO setup */ 
#define NRF24_SPI1_PORT 	GPIOA
#define NRF24_SPI1_SCLK_PIN	GPIO_PIN_5
#define NRF24_SPI1_MISO_PIN	GPIO_PIN_6
#define NRF24_SPI1_MOSI_PIN	GPIO_PIN_7

#define NRF24_CE_PORT 	GPIOC
#define NRF24_CE_PIN    GPIO_PIN_4

#define NRF24_NSS_PORT 	GPIOC
#define NRF24_NSS_PIN 	GPIO_PIN_5

#define NRF24_IRQ_PORT 	GPIOB
#define NRF24_IRQ_PIN 	GPIO_PIN_0


/* SPI1 Handler */ 
extern SPI_HandleTypeDef hspi1;
#define NRF24_SPI_HANDLER hspi1



/* ----------------------------------------------------------- */
/* ------------------ Macros for Registers ------------------- */
/* - Addresses 0x18–0x1B are reserved for test; do not touch - */
/* ----------------------------------------------------------- */

/* ---------------- Instruction Mnemonics ---------------- */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

/* ---------------- Register Addresses ---------------- */
#define NRF24_REG_CONFIG              0x00
#define NRF24_REG_EN_AA               0x01
#define NRF24_REG_EN_RXADDR           0x02
#define NRF24_REG_SETUP_AW            0x03
#define NRF24_REG_SETUP_RETR          0x04
#define NRF24_REG_RF_CH               0x05
#define NRF24_REG_RF_SETUP            0x06
#define NRF24_REG_STATUS              0x07
#define NRF24_REG_OBSERVE_TX          0x08
#define NRF24_REG_RPD                 0x09
#define NRF24_REG_RX_ADDR_P0          0x0A  /* 5-byte max, LSByte first */
#define NRF24_REG_RX_ADDR_P1          0x0B  /* 5-byte max, LSByte first */
#define NRF24_REG_RX_ADDR_P2          0x0C  /* only LSB; MSBs = P1[39:8] */
#define NRF24_REG_RX_ADDR_P3          0x0D  /* only LSB; MSBs = P1[39:8] */
#define NRF24_REG_RX_ADDR_P4          0x0E  /* only LSB; MSBs = P1[39:8] */
#define NRF24_REG_RX_ADDR_P5          0x0F  /* only LSB; MSBs = P1[39:8] */
#define NRF24_REG_TX_ADDR             0x10  /* 5-byte max, LSByte first */
#define NRF24_REG_RX_PW_P0            0x11
#define NRF24_REG_RX_PW_P1            0x12
#define NRF24_REG_RX_PW_P2            0x13
#define NRF24_REG_RX_PW_P3            0x14
#define NRF24_REG_RX_PW_P4            0x15
#define NRF24_REG_RX_PW_P5            0x16
#define NRF24_REG_FIFO_STATUS         0x17
/* 0x18–0x1B reserved */
#define NRF24_REG_DYNPD               0x1C
#define NRF24_REG_FEATURE             0x1D


/* ---------------- CONFIG (0x00) ---------------- */
// Positions
#define NRF24_REG_CONFIG_PRIM_RX_Pos      0
#define NRF24_REG_CONFIG_PWR_UP_Pos       1
#define NRF24_REG_CONFIG_CRCO_Pos         2
#define NRF24_REG_CONFIG_EN_CRC_Pos       3
#define NRF24_REG_CONFIG_MASK_MAX_RT_Pos  4
#define NRF24_REG_CONFIG_MASK_TX_DS_Pos   5
#define NRF24_REG_CONFIG_MASK_RX_DR_Pos   6
/* bit7 reserved */

// Values  
#define NRF24_REG_CONFIG_PRIM_RX_Val_PRX      0b1u
#define NRF24_REG_CONFIG_PRIM_RX_Val_PTX      0b0u

#define NRF24_REG_CONFIG_PWR_UP_Val_UP        0b1u
#define NRF24_REG_CONFIG_PWR_UP_Val_DOWN      0b0u

#define NRF24_REG_CONFIG_CRCO_Val_BYTE        0b0u
#define NRF24_REG_CONFIG_CRCO_Val_2BYTES      0b1u

#define NRF24_REG_CONFIG_EN_CRC_Val_DISABLE   0b0u
#define NRF24_REG_CONFIG_EN_CRC_Val_ENABLE    0b1u

#define NRF24_REG_CONFIG_MASK_xx_Val_IQR_ENABLE   0b0u
#define NRF24_REG_CONFIG_MASK_xx_Val_IQR_DISABLE  0b1u

/* ---------------- EN_AA (0x01) ---------------- */
// Positions
#define NRF24_REG_EN_AA_ENAA_P0_Pos       0
#define NRF24_REG_EN_AA_ENAA_P1_Pos       1
#define NRF24_REG_EN_AA_ENAA_P2_Pos       2
#define NRF24_REG_EN_AA_ENAA_P3_Pos       3
#define NRF24_REG_EN_AA_ENAA_P4_Pos       4
#define NRF24_REG_EN_AA_ENAA_P5_Pos       5
/* bits7:6 reserved */

// Values  
#define NRF24_REG_EN_AA_ENAA_Px_Val_DISABLE   0u
#define NRF24_REG_EN_AA_ENAA_Px_Val_ENABLE    1u

/* ---------------- EN_RXADDR (0x02) ---------------- */
// Positions
#define NRF24_REG_EN_RXADDR_ERX_P0_Pos    0
#define NRF24_REG_EN_RXADDR_ERX_P1_Pos    1
#define NRF24_REG_EN_RXADDR_ERX_P2_Pos    2
#define NRF24_REG_EN_RXADDR_ERX_P3_Pos    3
#define NRF24_REG_EN_RXADDR_ERX_P4_Pos    4
#define NRF24_REG_EN_RXADDR_ERX_P5_Pos    5
/* bits7:6 reserved */

// Values 
#define NRF24_REG_EN_RXADDR_ERX_Px_Val_DISABLE  0u
#define NRF24_REG_EN_RXADDR_ERX_Px_Val_ENABLE   1u


/* ---------------- SETUP_AW (0x03) ---------------- */
// Positions
#define NRF24_REG_SETUP_AW_Pos            0  /* width field at [1:0] */
/* 6 MSBs reserved */

// Values 
#define NRF24_REG_SETUP_AW_Val_ILLEGAL  0b00u
#define NRF24_REG_SETUP_AW_Val_3BYTES   0b01u
#define NRF24_REG_SETUP_AW_Val_4BYTES   0b10u
#define NRF24_REG_SETUP_AW_Val_5BYTES   0b11u


/* ---------------- SETUP_RETR (0x04) ---------------- */
// Positions
#define NRF24_REG_SETUP_RETR_ARC_Pos      0
#define NRF24_REG_SETUP_RETR_ARD_Pos      4

// Values
/* Number of re-transmits on fail can be set directly. 
e.g., 3 = 3 re-transmits on fail. */
#define NRF24_REG_SETUP_RETR_ARC_Val_DISABLE                        0b0000u

#define NRF24_REG_SETUP_RETR_ARD_Func_Val_step250_max4000us(value)  (int(value/250)-1)


/* ---------------- RF_CH (0x05) ---------------- */
// Positions
#define NRF24_REG_RF_CH_RF_CH_Pos         0 /* [6:0] channel number */
/* bit7 reserved */


/* ---------------- RF_SETUP (0x06) ---------------- */
// Positions
#define NRF24_REG_RF_SETUP_RF_PWR_Pos     1  /* [2:1] */
#define NRF24_REG_RF_SETUP_RF_DR_HIGH_Pos 3
#define NRF24_REG_RF_SETUP_PLL_LOCK_Pos   4
#define NRF24_REG_RF_SETUP_RF_DR_LOW_Pos  5
#define NRF24_REG_RF_SETUP_CONT_WAVE_Pos  7

// Values 
#define NRF24_REG_RF_SETUP_RF_PWR_Val_NEG18dBm                  0b00u
#define NRF24_REG_RF_SETUP_RF_PWR_Val_NEG12dBm                  0b01u
#define NRF24_REG_RF_SETUP_RF_PWR_Val_NEG6dBm                   0b10u
#define NRF24_REG_RF_SETUP_RF_PWR_Val_0dBm                      0b11u

#define NRF24_REG_RF_SETUP_RF_DR_HIGH_Val_1MBPS                 0b00u
#define NRF24_REG_RF_SETUP_RF_DR_HIGH_Val_2MBPS                 0b01u
#define NRF24_REG_RF_SETUP_RF_DR_HIGH_Val_250KBPS               0b10u

#define NRF24_REG_RF_SETUP_PLL_LOCK_Val_SET                     0b0u
#define NRF24_REG_RF_SETUP_PLL_LOCK_Val_RESET                   0b1u

#define NRF24_REG_RF_SETUP_RF_DR_LOW_Val_RESET                  0b0u
#define NRF24_REG_RF_SETUP_RF_DR_LOW_Val_SET                    0b1u

#define NRF24_REG_RF_SETUP_CONT_WAVE_Val_DISABLE                0b0u
#define NRF24_REG_RF_SETUP_CONT_WAVE_Val_ENABLE                 0b1u


/* ---------------- STATUS (0x07) ---------------- */
// Positions
#define NRF24_REG_STATUS_TX_FULL_Pos                  0
#define NRF24_REG_STATUS_RX_P_NO_Pos                  1 
#define NRF24_REG_STATUS_MAX_RT_Pos                   4
#define NRF24_REG_STATUS_TX_DS_Pos                    5
#define NRF24_REG_STATUS_RX_DR_Pos                    6
/* bit7 reserved */

// Values 
#define NRF24_REG_STATUS_TX_FULL_Val_AVAILABLE        0b1u
#define NRF24_REG_STATUS_TX_FULL_Val_FULL             0b1u

#define NRF24_REG_STATUS_RX_P_NO_Val_PIPE0_AVAILABLE  0b000u 
#define NRF24_REG_STATUS_RX_P_NO_Val_PIPE1_AVAILABLE  0b001u 
#define NRF24_REG_STATUS_RX_P_NO_Val_PIPE2_AVAILABLE  0b010u 
#define NRF24_REG_STATUS_RX_P_NO_Val_PIPE3_AVAILABLE  0b011u 
#define NRF24_REG_STATUS_RX_P_NO_Val_PIPE4_AVAILABLE  0b100u 
#define NRF24_REG_STATUS_RX_P_NO_Val_PIPE5_AVAILABLE  0b101u 
#define NRF24_REG_STATUS_RX_P_NO_Val_NOT_USED         0b110u 
#define NRF24_REG_STATUS_RX_P_NO_Val_RX_EMPTY         0b111u 

#define NRF24_REG_STATUS_MAX_RT_Val_CLEAR             0b1u
#define NRF24_REG_STATUS_TX_DS_Val_CLEAR              0b1u
#define NRF24_REG_STATUS_RX_DR_Val_CLEAR              0b1u


/* ---------------- OBSERVE_TX (0x08) ---------------- */
// Positions
#define NRF24_REG_OBSERVE_TX_ARC_CNT_Pos  0 /* [3:0] */
#define NRF24_REG_OBSERVE_TX_PLOS_CNT_Pos 4 /* [7:4] */


/* ---------------- RPD (0x09) ---------------- */
// Positions
#define NRF24_REG_RPD_RPD_Pos             0
/* bits7:1 reserved */

/* ---------------- RX_ADDR_Px / TX_ADDR (0x0A–0x10) ---------------- */
/* Multi-byte address registers are written LSByte-first. P2–P5 accept only the LSB,
   MSBytes mirror RX_ADDR_P1[39:8]. */


/* ---------------- RX_PW_Px (0x11–0x16) ---------------- */
// Positions
#define NRF24_REG_RX_PW_PX_LEN_Pos        0  /* [5:0] length 0..32 (0=pipe not used) */
/* bits7:6 reserved */


/* ---------------- FIFO_STATUS (0x17) ---------------- */
// Positions
#define NRF24_REG_FIFO_STATUS_RX_EMPTY_Pos 0
#define NRF24_REG_FIFO_STATUS_RX_FULL_Pos  1
/* bits3:2 reserved */
#define NRF24_REG_FIFO_STATUS_TX_EMPTY_Pos 4
#define NRF24_REG_FIFO_STATUS_TX_FULL_Pos  5
#define NRF24_REG_FIFO_STATUS_TX_REUSE_Pos 6
/* bit7 reserved */


/* ---------------- DYNPD (0x1C) ---------------- */
// Positions
#define NRF24_REG_DYNPD_DPL_P0_Pos        0
#define NRF24_REG_DYNPD_DPL_P1_Pos        1
#define NRF24_REG_DYNPD_DPL_P2_Pos        2
#define NRF24_REG_DYNPD_DPL_P3_Pos        3
#define NRF24_REG_DYNPD_DPL_P4_Pos        4
#define NRF24_REG_DYNPD_DPL_P5_Pos        5
/* bits7:6 reserved */


/* ---------------- FEATURE (0x1D) ---------------- */
#define NRF24_REG_FEATURE_EN_DYN_ACK_Pos  0
#define NRF24_REG_FEATURE_EN_ACK_PAY_Pos  1
#define NRF24_REG_FEATURE_EN_DPL_Pos      2
/* bits7:3 reserved */

#endif // NRF24L01P_INC_NRF24L01P_H_
