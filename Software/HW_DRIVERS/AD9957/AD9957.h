/*
 * AD9957.h
 *
 *  Created on: Aug 23, 2020
 *      Author: voyag
 */

#ifndef INC_AD9957_H_
#define INC_AD9957_H_

#include "main.h"
#include "math.h"

// PIN - Definitions
#define AD9957_IOUP_Pin GPIO_PIN_8
#define AD9957_IOUP_GPIO_Port GPIOE
#define AD9957_CS_Pin GPIO_PIN_9
#define AD9957_CS_GPIO_Port GPIOE
#define AD9957_RESET_Pin GPIO_PIN_10
#define AD9957_RESET_GPIO_Port GPIOE
#define AD9957_IORESET_Pin GPIO_PIN_11
#define AD9957_IORESET_GPIO_Port GPIOE
#define AD9957_PLL_LOCK_Pin GPIO_PIN_12
#define AD9957_PLL_LOCK_GPIO_Port GPIOE

// Register-Addresses - Registers are 32bit long unless otherwise noted
#define CFR1 0x00
#define CFR2 0x01
#define CFR3 0x02
#define AUXDAC 0x03
#define IOUPRATE 0x04
#define RAMSEG0 0x05 // 48bit Register!
#define RAMSEG1 0x06 // 48bit Register!
#define AMP 0x09
#define CHIPSYNC 0x0A	// Configuration for Chip Synchronisation. Not needed, only one Chip used
// Profile-Registers (64 bit Registers!)
#define PROFILE_0 0x0E	// PROFILE 7
#define PROFILE_1 0x0F	// PROFILE 6
#define PROFILE_2 0x10	// PROFILE 5
#define PROFILE_3 0x11	// PROFILE 4
#define PROFILE_4 0x12	// PROFILE 3
#define PROFILE_5 0x13	// PROFILE 2
#define PROFILE_6 0x14	// PROFILE 1
#define PROFILE_7 0x15	// PROFILE 0
// Register-Sizes:
// RAM REGISTER - 32bit RAM WORD
#define AD9957_RAM 0x16
// Modes
#define AD_DIRECT_CTRL 0xD0
#define AD_FPGA_CTRL 0xF0
// Chip Operating MOdes
#define AD_QDUC 0x00
#define AD_ST 0x01
#define AD_IDAC 0x02
#define AD_OFF 0xFF
// RAM Destinations / Operating-Modes
#define AD_RAM_QDUC 0x00
#define AD_RAM_ST 0x01
#define AD_RAM_IDAC 0x02
#define AD_RAM_OFF 0xFF
// Constants
#define AD_MIN_REF_CLK 20000000	// Minimum Reference-Clock - Is being used as the default RefCLK

// Datastructure that holds the Configuration-Data for the AD9957
typedef struct AD9957_InternalConfig {
	double REFCLK;
	double SYSCLK;
	// FPGA-Settings:
	uint8_t fpga_mode;

	// AD9957-Settings:
	// CFR 1 - Config-Settings
	unsigned char CFR1_RAM_EN;				// ENABLE THE RAM (0 = RAM DISABLE)
	unsigned char CFR1_RAM_PB_DEST;			// RAM Playback Destination: 0 = Baseband Scaling Multipliers
	unsigned char CFR1_OP_MODE;				// Operating-Mode of the Chip - This Variable is also used to configure the State of the AD9957-Driver
	unsigned char CFR1_MAN_OSK_CTRL;			// Manual OSK Enable-Control (OSK-Pin Enable or Disable)
	unsigned char CFR1_INV_SINC_FILT;		// Enable the Inverse SINC-Filter (0 = Filter Bypassed)
	unsigned char CFR1_CLEAR_CCI;			// 1 = Initiate Asynchronous RESET of the CCI-Filter Accumulators
	unsigned char CFR1_SEL_DDS_OUTPUT;		// Select SINE (1) or COSINE (0) Output of DDS - Only Available if OP_MODE = 0x01 = Single Tone-Mode
	unsigned char CFR1_AUTOCLR_PHASE_ACCU;	// 1 = Initiate Asynchronous RESET of Phase Accumulator if IOUpdate is Asserted or Profile-Change Occurs
	unsigned char CFR1_CLR_PHASE_ACCU;		// 1 = Reset Phase Accumulator
	unsigned char CFR1_LOAD_ARR_IOUP;		// 1 = Amplitude Ramp Timer is reloaded on IOUpdate Assert
	unsigned char CFR1_OSK_ENABLE;
	unsigned char CFR1_OSK_AUTO;
	unsigned char CFR1_DIGI_PWRDN;			// 1 = Clock to Digital Core disabled
	unsigned char CFR1_DAC_PWRDN; 			// 1 = DAC  disabled
	unsigned char CFR1_REFCLK_IN_PWRDN;		// 1 = REFClk-Input disabled
	unsigned char CFR1_AUXDAC_PWRDN;			// 1 = Auxiliary DAC Disabled
	unsigned char CFR1_EXT_PWRDN_MODE;		// Set the behavior of PWRDN-Pin: 0 = Full PowerDown, 1 = Hot Standby
	unsigned char CFR1_AUTO_PWRDN;			// 1 = If TX-PIN = LOW, chip goes into PowerDown-Mode
	unsigned char CFR1_SDIO_IN_ONLY;			// Configure SDIO-Pin: 1 = 3 Wire mode, SDIO-Pin is INPUT ONLY (In case of 3-Wire connect set to 1)
	unsigned char CFR1_LSB_FIRST;			// 0 = MSB First (default, also the way the SPI is sending data - Leave at Default)
	// CFR 2 - Config-Settings
	unsigned char CFR2_BLACKFIN_ACTIVE;		// 0 = Parallel Data-Port configured as Parallel Port (default, don't change)
	unsigned char CFR2_BLACKFIN_BIT_ORDER;	// Blackfin-Config Parameter - Don't care
	unsigned char CFR2_BLACKFIN_EARLY_FRAME_SYNC;
	unsigned char CFR2_EN_PROFILE_REG_ASF_SRC;	// 1 = Enables the Amplitude Scaling of the output by the ASF-Register in the Profile-Configuration
	unsigned char CFR2_INT_IOUP_ACTIVE;		// 0 = Internal IOUPDATE Inactive
	unsigned char CFR2_SYNC_CLK_EN;			// 0 = SYNC_CLK OFF / 1 = SYNC-CLK ON <- Good to check if Chip is working for debug. Normal Operation: Turn Output OFF
	unsigned char CFR2_READ_EFF_FTW;			// 0 = Read Operation shows FTW / 1 = Read Operation shows Input-Value to DDS Phase Accumulator
	unsigned char CFR2_IOUP_RATE_DIV;		// IOUpdate Rate Divider-Setting for Internal IOUPDATE (Don't care if IOUPDATE = External)
	unsigned char CFR2_PDCLK_RATE;			// 0 = PDCLK Full Speed / 1 = PDCLK Half Speed
	unsigned char CFR2_DATA_FORMAT;			// 0 = Input-Data is twos Complement
	unsigned char CFR2_PDCLK_ENABLE;			// 0 = PDCLK-Output OFF / 1 = PDCLK-Output ON (default, should not be changed)
	unsigned char CFR2_PDCLK_INVERT;			// 0 = Normal (PDCLK HIGH = Q-Data, PDCLK LOW = I-Data) / 1 = Inverted
	unsigned char CFR2_TXENABLE_INVERT;		// 0 = Logic 0 is Standby, 1 is Transmit
	unsigned char CFR2_Q_FIRST_DATA_PROC;	// 0 = I-Data First, then Q-data
	unsigned char CFR2_DATA_ASSEMB_HOLD_LAST;	// 0 = If TXENABLE is False, then send only 0, 1 = If TXENABLE is True, send last Baseband Package
	unsigned char CFR2_SYNC_TIMING_VAL_DISABLE;	// Don't care, Syncronization-Parameter
	// CFR 3 - Config-Register
	unsigned char CFR3_DRV0;					// Control REFCLK_OUT (See Table)
	unsigned char CFR3_VCO_SELECT;			// Selects the VCO-Band (See Table)
	unsigned char CFR3_ICP;					// Select Charge Pump-Current
	unsigned char CFR3_REFCLK_DIV_BYPASS;	// 0 = Divider Selected
	unsigned char CFR3_REFCLK_DIV_RESET;		// 1 = Reset the REFCLK-Divider
	unsigned char CFR3_PLL_ENABLE;			// 0 = PLL OFF (default) / 1 = PLL ON
	unsigned char CFR3_N;					// 7bit Modulus Divider to reset
	unsigned char CFR3_PFD_IN_DOUBLER;
	// AUX_DAC-Register
	unsigned char FSC;					// Full Scale Output Current of Main DAC
	// IOUpdate-Register
	unsigned int IOUPDATE_RATE;			// IOUpdate-Rate - Don't care, we're using external IOUpdate
	// RAM Segment-Register 0
	unsigned int RAM_ADDRESS_STEP_RATE_0;	// Rate at which the RAM State-Machine steps through the RAM.
	unsigned int RAM_END_ADDR_0;		// End Address for RAM-State Machine
	unsigned int RAM_START_ADDR_0;		// Start-Address for RAM-State Machine
	unsigned int RAM_PB_MODE_0;			// 3 Bit number selects Playback-Mode for RAM-State-Machine
	// RAM Segment-Register 1
	unsigned int RAM_ADDRESS_STEP_RATE_1;	// Rate at which the RAM State-Machine steps through the RAM.
	unsigned int RAM_END_ADDR_1;		// End Address for RAM-State Machine
	unsigned int RAM_START_ADDR_1;		// Start-Address for RAM-State Machine
	unsigned int RAM_PB_MODE_1;			// 3 Bit number selects Playback-Mode for RAM-State-Machine
	// Amplitude Scale Factor
	unsigned int AMP_RAMP_RATE;			// Rate at which the OSK Controller updates DDS Amplitude
	unsigned int AMP_ASF;				// Amplitude Scale Factor
	unsigned char AMP_STEP_SIZE;		// Step Size for Amplitude Changes to DDS
	// Multichip Sync Register
	/*
	 * Nothing to do here, since we don't want to synchronize with other chips
	 */
	// Single Tone Profile Register - Each Profile contains this Register
	uint16_t ST_ASF;	// Amplitude Scale Factor for Single Tone Mode
	unsigned int ST_POW;	// Phase Offset Word for Single Tone MOde
	unsigned int ST_FTW;	// Frequency Tuning Word for Single Tone Mode
	// QDUC Profile Register
	unsigned char QDUC_CCI_INTERPOLATION_RATE;
	unsigned char QDUC_SPECTRAL_INVERT;
	unsigned char QDUC_CCI_INVERSE_BYPASS;
	unsigned char QDUC_OSK;
	unsigned int QDUC_POW;
	unsigned int QDUC_FTW;

} AD9957_CONF;
// Datastructure for the AD9957 Subroutines - Calls from external functions write data to this structure that is then read by internal functions to update private variables and update the AD9957-Chip.
typedef struct AD9957_SysData {
	double freq_singleTone;			// Frequency-Value of output-signal
	double phase_singleTone; 		// Phase-Angle in degrees
	double amp_singleTone;			// Amplitude-Value (fraction of Full Scale - Max Value = 1.0)
	double freq_qduc;
	double phase_qduc;
	double amp_qduc;
} AD9957_DATA;
// Datastructure that mirrors the AD9957 Data-Registers necessary to program the outputs.
typedef struct AD9957_DataWord {
	// Config-Registers
	uint32_t REG_CFR1;
	uint32_t REG_CFR2;
	uint32_t REG_CFR3;
	uint32_t REG_AUX_DAC;
	uint32_t REG_IOUP_RATE;
	// RAM SEGMENT-Registers
	uint16_t REG_RAMSEG0_1;	// Upper 16bit of the RAMSEGMENT-Register
	uint32_t REG_RAMSEG0_0;	// Lower 32bit of the RAMSEGMENT-Register
	uint16_t REG_RAMSEG1_1;
	uint32_t REG_RAMSEG1_0;
	uint32_t REG_AMP;
	uint32_t REG_MULTICHIP_SYNC;
	// Registers for SINGLE TONE-Mode
	uint32_t REG_ST1;	// Phase Offset-Word and Amplitude Scale Factor (Amplitude)
	uint32_t REG_ST0;	// Frequency Tuning-Word

	// Register-Values for QDUC-Mode
	uint32_t REG_QDUC1;	// Factor for CCI-Filter (6bit), Selector for Spectral Invert (1bit), Selector for Filter-Bypass (1bit), Output Scale Factor (Amplitude), Phase Offset-Word for QDUC-Mode
	uint32_t REG_QDUC0; // Frequency Tuning-Word for QDUC-Mode (Carrier-Frequency)

	// Register-Value for the RAM-Register
	uint32_t REG_RAM;
} AD9957_REG;

typedef struct AD9957_ChipData{
	// Config-Registers
	uint32_t REG_CFR1;
	uint32_t REG_CFR2;
	uint32_t REG_CFR3;
	uint32_t REG_AUX_DAC;
	uint32_t REG_IOUP_RATE;
	// RAM SEGMENT-Registers
	uint16_t REG_RAMSEG0_1;	// Upper 16bit of the RAMSEGMENT-Register
	uint32_t REG_RAMSEG0_0;	// Lower 32bit of the RAMSEGMENT-Register
	uint16_t REG_RAMSEG1_1;
	uint32_t REG_RAMSEG1_0;
	uint32_t REG_AMP;
	uint32_t REG_MULTICHIP_SYNC;

	uint64_t REG_PROFILE7;
	uint64_t REG_PROFILE6;
	uint64_t REG_PROFILE5;
	uint64_t REG_PROFILE4;
	uint64_t REG_PROFILE3;
	uint64_t REG_PROFILE2;
	uint64_t REG_PROFILE1;
	uint64_t REG_PROFILE0;
	// Register-Value for the RAM-Register
	uint32_t REG_RAM;
} AD9957_INTREG;

HAL_StatusTypeDef AD9957_Setup(long refCLK, uint8_t chip_mode, uint8_t int_RAM_mode);	// Setup of the AD9957-Chip
HAL_StatusTypeDef AD9957_PLLCheck();	// Checks the Lock-Signal of the AD9957 PLL
HAL_StatusTypeDef AD9957_CWFreq(int frequency);
HAL_StatusTypeDef AD9957_CWAmplitude(float amplitude);
HAL_StatusTypeDef AD9957_CWPhase(float phase);
HAL_StatusTypeDef AD9957_FillIntRAM(int *databuffer);


#endif /* INC_AD9957_H_ */
