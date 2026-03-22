/*
 * AD9957.c
 *
 *  Created on: Aug 23, 2020
 *      Author: voyag
 */

#include "AD9957.h"

// Variables
AD9957_CONF ad9957_config;
AD9957_DATA data;
AD9957_REG reg;
HAL_SPI_StateTypeDef error;
extern SPI_HandleTypeDef hspi2;

// Prototypes
HAL_SPI_StateTypeDef AD9957_GenerateRegisters(uint8_t UpdateMode,
		uint8_t profile);
HAL_SPI_StateTypeDef AD9957_WriteRegister(uint8_t regAddr);
HAL_SPI_StateTypeDef AD9957_ReadRegister(uint32_t *dataTarget, uint8_t regAddr);

HAL_StatusTypeDef AD9957_Setup(long refCLK, uint8_t chip_mode,
		uint8_t int_RAM_mode) {	// Setup of the AD9957-Chip
	error = 0x00;
	if (refCLK < AD_MIN_REF_CLK) {
		ad9957_config.REFCLK = AD_MIN_REF_CLK;
	} else {
		ad9957_config.REFCLK = refCLK;
	}
	// Setting up the Chip-Configuration
	ad9957_config.CFR1_OP_MODE = chip_mode;	// Set the Chip-Mode (IDAC, QDUC, SINGLE TONE, OFF)
	ad9957_config.CFR1_SDIO_IN_ONLY = 1;// Configure the SDIO-Pin to be only Input
	switch (chip_mode) { // Update the config-variable according to selected Chip Mode
	case AD_QDUC:
		ad9957_config.CFR1_DIGI_PWRDN = 0;				// Switch the Chip ON
		ad9957_config.CFR1_DAC_PWRDN = 0;				// Switch DAC ON
		ad9957_config.CFR1_REFCLK_IN_PWRDN = 0;		// Switch Clock Inputs ON
		ad9957_config.CFR1_AUXDAC_PWRDN = 0;			// Switch AuxDAC ON
		ad9957_config.CFR1_EXT_PWRDN_MODE = 0;// Switch EXT Power Down-Control to FAST RECOVERY-Mode
		ad9957_config.CFR2_SYNC_CLK_EN = 1;
		ad9957_config.CFR2_PDCLK_ENABLE = 1;// Enable PDCLK (Parallel Data Clock)
		ad9957_config.CFR2_SYNC_TIMING_VAL_DISABLE = 1;
		ad9957_config.CFR3_DRV0 = 0x01;	// Set the REFCLK_OUT to LOW CURRENT-Mode
		ad9957_config.CFR3_VCO_SELECT = 0x05;// Select VCO5 (guaranteed to operate at 1 GHZ)
		ad9957_config.CFR3_ICP = 0x03;// Set Charge Pump Current for the PLL to 287uA TODO: Experiment with this value to get maximum performance (minimal Phase Noise)
		ad9957_config.CFR3_REFCLK_DIV_BYPASS = 0;// Set the Bypass-Path to OFF
		ad9957_config.CFR3_REFCLK_DIV_RESET = 1;// Deselect the Divider in the Bypass-Path (Reduces noise)
		ad9957_config.CFR3_PLL_ENABLE = 1;				// ENABLE the PLL
		ad9957_config.CFR3_N = 50;// N-Value of the PLL (Multiplier for the RefCLK)
		ad9957_config.FSC = 0x7F;// Set the Full Scale output-current to Maximum
		ad9957_config.AMP_RAMP_RATE = 128;// Set the Amp RAMP-Rate (Arbitrary Value - Can be Updated later)
		ad9957_config.AMP_ASF = (int) (1.0 * (pow(2, 14) - 1));	// Set the Amplitude Scale-Factor to 1.0 (100%)
		ad9957_config.AMP_STEP_SIZE = 0;// Set Amplitude Step-Size to 0 // TODO: FIND OUT THE ACTUAL STEP SIZE
		break;
	case AD_ST:
		ad9957_config.CFR1_DIGI_PWRDN = 0;
		ad9957_config.CFR1_DAC_PWRDN = 0;
		ad9957_config.CFR1_REFCLK_IN_PWRDN = 0;
		ad9957_config.CFR1_AUXDAC_PWRDN = 0;
		ad9957_config.CFR1_EXT_PWRDN_MODE = 0;
		ad9957_config.CFR2_EN_PROFILE_REG_ASF_SRC = 1;
		ad9957_config.CFR2_SYNC_CLK_EN = 1;
		ad9957_config.CFR2_SYNC_TIMING_VAL_DISABLE = 1;
		ad9957_config.CFR2_PDCLK_ENABLE = 1;
		ad9957_config.CFR3_DRV0 = 0x02;
		ad9957_config.CFR3_VCO_SELECT = 0x05;
		ad9957_config.CFR3_ICP = 0x03;
		ad9957_config.CFR3_REFCLK_DIV_BYPASS = 0;
		ad9957_config.CFR3_REFCLK_DIV_RESET = 1;
		ad9957_config.CFR3_PLL_ENABLE = 1;
		ad9957_config.CFR3_N = 50;
		ad9957_config.FSC = 0x7F;
		ad9957_config.AMP_RAMP_RATE = 128;
		ad9957_config.AMP_ASF = (int) (1.0 * (pow(2, 14) - 1));
		ad9957_config.AMP_STEP_SIZE = 0;
		break;
	case AD_IDAC:
		ad9957_config.CFR1_DIGI_PWRDN = 0;
		ad9957_config.CFR1_DAC_PWRDN = 0;
		ad9957_config.CFR1_REFCLK_IN_PWRDN = 0;
		ad9957_config.CFR1_AUXDAC_PWRDN = 0;
		ad9957_config.CFR1_EXT_PWRDN_MODE = 0;
		ad9957_config.CFR2_EN_PROFILE_REG_ASF_SRC = 1;
		ad9957_config.CFR2_SYNC_CLK_EN = 1;
		ad9957_config.CFR2_PDCLK_ENABLE = 1;
		ad9957_config.CFR2_SYNC_TIMING_VAL_DISABLE = 1;
		ad9957_config.CFR3_DRV0 = 0x03;
		ad9957_config.CFR3_VCO_SELECT = 0x05;
		ad9957_config.CFR3_ICP = 0x03;
		ad9957_config.CFR3_REFCLK_DIV_BYPASS = 0;
		ad9957_config.CFR3_REFCLK_DIV_RESET = 1;
		ad9957_config.CFR3_PLL_ENABLE = 1;
		ad9957_config.CFR3_N = 50;
		ad9957_config.FSC = 0x7F;
		ad9957_config.AMP_RAMP_RATE = 128;
		ad9957_config.AMP_ASF = (int) (1.0 * (pow(2, 14) - 1));
		ad9957_config.AMP_STEP_SIZE = 0;
		break;
	case AD_OFF:	// Power OFF-Mode: Switch OFF the Output-DAC.
		ad9957_config.CFR1_DIGI_PWRDN = 0;
		ad9957_config.CFR1_DAC_PWRDN = 1;
		ad9957_config.CFR1_REFCLK_IN_PWRDN = 0;
		ad9957_config.CFR1_AUXDAC_PWRDN = 0;
		ad9957_config.CFR1_EXT_PWRDN_MODE = 1;
		break;
	}
	switch (int_RAM_mode) {
	// TODO: Configure Chip to use internal RAM depending on the settings
	case AD_RAM_OFF:
		ad9957_config.CFR1_RAM_EN = 0;
		break;
	default:
		ad9957_config.CFR1_RAM_EN = 0;
		break;
	}
	HAL_GPIO_WritePin(AD9957_RESET_GPIO_Port, AD9957_RESET_Pin, GPIO_PIN_SET);	// RESET the Chip
	HAL_Delay(1);
	HAL_GPIO_WritePin(AD9957_RESET_GPIO_Port, AD9957_RESET_Pin, GPIO_PIN_RESET);	// End the Reset-Condition, put chip back into working-mode.
	HAL_GPIO_WritePin(AD9957_IORESET_GPIO_Port, AD9957_IORESET_Pin, GPIO_PIN_SET);	// RESET the Chip's IO-Statemachine
	HAL_Delay(1);
	HAL_GPIO_WritePin(AD9957_IORESET_GPIO_Port, AD9957_IORESET_Pin, GPIO_PIN_RESET);	// Clear RESET of IO-Statemachine
	ad9957_config.IOUPDATE_RATE = 0xFFFFFFFF;
	ad9957_config.SYSCLK = ad9957_config.REFCLK * ad9957_config.CFR3_N;
	AD9957_CWAmplitude(0.9);
	AD9957_CWPhase(90.0);
	AD9957_CWFreq(50000000);
	AD9957_GenerateRegisters(0xFF, PROFILE_0);
	for (int i = 0; i < 7; i++) {
		AD9957_GenerateRegisters(0x01, PROFILE_0 + i);
	}
	error = AD9957_PLLCheck();
	return error;
}

HAL_StatusTypeDef AD9957_PLLCheck() {// Checks the Lock-Signal of the AD9957 PLL
	if (HAL_GPIO_ReadPin(AD9957_PLL_LOCK_GPIO_Port, AD9957_PLL_LOCK_Pin)
			== GPIO_PIN_SET) {
		return HAL_OK;
	} else {
		return HAL_ERROR;
	}
}

HAL_StatusTypeDef AD9957_CWFreq(int frequency) {
	double ftw = pow(2, 32) - 1;
	ftw = ftw * (frequency / ad9957_config.SYSCLK);
	ad9957_config.ST_FTW = (int) ftw;
	ad9957_config.QDUC_FTW = ad9957_config.ST_FTW;
	return error;
}

HAL_StatusTypeDef AD9957_CWAmplitude(float amplitude) {
	float asf = pow(2, 14) - 1;
	ad9957_config.ST_ASF = (int) (asf * amplitude);
	return error;
}

HAL_StatusTypeDef AD9957_CWPhase(float phase) {
	// Calculate the Phase Offset Word: 360°/14bit breites Register = 0.022°/LSB
	ad9957_config.ST_POW = (int) (phase / 0.022);
	return error;
}

HAL_StatusTypeDef AD9957_FillIntRAM(int *databuffer) {
	return error;
}

HAL_SPI_StateTypeDef AD9957_GenerateRegisters(uint8_t UpdateMode,
		uint8_t profile) {
	switch (UpdateMode) {
	case 0xFF:
		reg.REG_CFR1 = (int) ((ad9957_config.CFR1_LSB_FIRST * pow(2, 0))
				+ (ad9957_config.CFR1_SDIO_IN_ONLY * pow(2, 1))
				+ (ad9957_config.CFR1_AUTO_PWRDN * pow(2, 2))
				+ (ad9957_config.CFR1_EXT_PWRDN_MODE * pow(2, 3))
				+ (ad9957_config.CFR1_AUXDAC_PWRDN * pow(2, 4))
				+ (ad9957_config.CFR1_REFCLK_IN_PWRDN * pow(2, 5))
				+ (ad9957_config.CFR1_DAC_PWRDN * pow(2, 6))
				+ (ad9957_config.CFR1_DIGI_PWRDN * pow(2, 7))
				+ (ad9957_config.CFR1_OSK_AUTO * pow(2, 8))
				+ (ad9957_config.CFR1_OSK_ENABLE * pow(2, 9))
				+ (ad9957_config.CFR1_LOAD_ARR_IOUP * pow(2, 10))
				+ (ad9957_config.CFR1_CLR_PHASE_ACCU * pow(2, 11))
				+ (ad9957_config.CFR1_AUTOCLR_PHASE_ACCU * pow(2, 13))
				+ (ad9957_config.CFR1_SEL_DDS_OUTPUT * pow(2, 16))
				+ (ad9957_config.CFR1_CLEAR_CCI * pow(2, 21))
				+ (ad9957_config.CFR1_INV_SINC_FILT * pow(2, 22))
				+ (ad9957_config.CFR1_MAN_OSK_CTRL * pow(2, 23))
				+ (ad9957_config.CFR1_OP_MODE * pow(2, 24))
				+ (ad9957_config.CFR1_RAM_PB_DEST * pow(2, 28))
				+ (ad9957_config.CFR1_RAM_EN * pow(2, 31)));// Calculate CFR1
		reg.REG_CFR2 = (int) ((ad9957_config.CFR2_SYNC_TIMING_VAL_DISABLE
				* pow(2, 5))
				+ (ad9957_config.CFR2_DATA_ASSEMB_HOLD_LAST * pow(2, 6))
				+ (ad9957_config.CFR2_Q_FIRST_DATA_PROC * pow(2, 8))
				+ (ad9957_config.CFR2_TXENABLE_INVERT * pow(2, 9))
				+ (ad9957_config.CFR2_PDCLK_INVERT * pow(2, 10))
				+ (ad9957_config.CFR2_PDCLK_ENABLE * pow(2, 11))
				+ (ad9957_config.CFR2_DATA_FORMAT * pow(2, 12))
				+ (ad9957_config.CFR2_PDCLK_RATE * pow(2, 13))
				+ (ad9957_config.CFR2_IOUP_RATE_DIV * pow(2, 14))
				+ (ad9957_config.CFR2_READ_EFF_FTW * pow(2, 16))
				+ (ad9957_config.CFR2_SYNC_CLK_EN * pow(2, 22))
				+ (ad9957_config.CFR2_INT_IOUP_ACTIVE * pow(2, 23))
				+ (ad9957_config.CFR2_EN_PROFILE_REG_ASF_SRC * pow(2, 24))
				+ (ad9957_config.CFR2_BLACKFIN_EARLY_FRAME_SYNC * pow(2, 29))
				+ (ad9957_config.CFR2_BLACKFIN_BIT_ORDER * pow(2, 30))
				+ (ad9957_config.CFR2_BLACKFIN_ACTIVE * pow(2, 31)));
		reg.REG_CFR3 = (int) ((ad9957_config.CFR3_N * pow(2, 1))
				+ (ad9957_config.CFR3_PLL_ENABLE * pow(2, 8))
				+ (ad9957_config.CFR3_REFCLK_DIV_RESET * pow(2, 14))
				+ (ad9957_config.CFR3_REFCLK_DIV_BYPASS * pow(2, 15))
				+ (7 * pow(2, 16))
				+ (ad9957_config.CFR3_ICP * pow(2, 19))
				+ (ad9957_config.CFR3_VCO_SELECT * pow(2, 24))
				+ (1 * pow(2, 27))
				+ (ad9957_config.CFR3_DRV0 * pow(2, 28)));
		reg.REG_AUX_DAC = (int) ad9957_config.FSC;
		reg.REG_AUX_DAC = reg.REG_AUX_DAC + 0x7f00;
		reg.REG_IOUP_RATE = (int) ad9957_config.IOUPDATE_RATE;
		reg.REG_RAMSEG0_1 = (int) ad9957_config.RAM_ADDRESS_STEP_RATE_0;
		reg.REG_RAMSEG0_0 = (int) ((ad9957_config.RAM_PB_MODE_0 * pow(2, 0))
				+ (ad9957_config.RAM_START_ADDR_0 * pow(2, 6))
				+ (ad9957_config.RAM_END_ADDR_0 * pow(2, 22)));
		reg.REG_RAMSEG1_1 = (int) ad9957_config.RAM_ADDRESS_STEP_RATE_1;
		reg.REG_RAMSEG1_0 = (int) ((ad9957_config.RAM_PB_MODE_1 * pow(2, 0))
				+ (ad9957_config.RAM_START_ADDR_1 * pow(2, 6))
				+ (ad9957_config.RAM_END_ADDR_1 * pow(2, 22)));
		reg.REG_AMP = (int) ((ad9957_config.AMP_STEP_SIZE * pow(2, 0))
				+ (ad9957_config.AMP_ASF * pow(2, 2))
				+ (ad9957_config.AMP_RAMP_RATE * pow(2, 16)));
		// reg.REG_MULTICHIP_SYNC = Assemble Multichip-Sync Register - Nothing to do here, since only one chip used.
		reg.REG_ST1 = (int) ((ad9957_config.ST_POW * pow(2, 0))
				+ (ad9957_config.ST_ASF * pow(2, 16)));
		reg.REG_ST0 = (int) ad9957_config.ST_FTW;
		reg.REG_QDUC1 = (int) ((ad9957_config.QDUC_POW * pow(2, 0))
				+ (ad9957_config.QDUC_OSK * pow(2, 16))
				+ (ad9957_config.QDUC_CCI_INVERSE_BYPASS * pow(2, 24))
				+ (ad9957_config.QDUC_SPECTRAL_INVERT * pow(2, 25))
				+ (ad9957_config.QDUC_CCI_INTERPOLATION_RATE * pow(2, 26)));
		reg.REG_QDUC0 = (int) ad9957_config.QDUC_FTW;
		reg.REG_RAM = (int) 0;// TODO: Generate the content of the RAM-Register
		// TODO: Update All Registers
		AD9957_WriteRegister(CFR1);
		AD9957_WriteRegister(CFR2);
		AD9957_WriteRegister(CFR3);
		AD9957_WriteRegister(AUXDAC);
		AD9957_WriteRegister(IOUPRATE);
		break;
	case 0x01:
		// Update Only Registers for Single Tone-Mode
		reg.REG_ST1 = (int) ((ad9957_config.ST_POW * pow(2, 0))
				+ (ad9957_config.ST_ASF * pow(2, 16)));
		reg.REG_ST0 = (int) ad9957_config.ST_FTW;
		AD9957_WriteRegister(AMP);
		AD9957_WriteRegister(profile);
		break;
	case 0x00:
		// TODO: Update the Registers necessary for QDUC-Mode
		break;
	case 0x10:
		// TODO: Update the Registers necessary for Interpolating DAC-Mode
		break;
		// TODO: Add more Cases to speed up the Register-Calculation Process by leaving out even more registers (e.g. Only Update Frequency Tuning Word)
	}
	return error;
}

HAL_SPI_StateTypeDef AD9957_WriteRegister(uint8_t regAddr) { // Subroutine that Writes a Register-Set
	// TODO: Assemble the Register-Values into 32bit Datawords
	uint8_t result;	// Success? Yes / NO
	uint8_t reglength;
	uint32_t *datawordH;
	uint32_t *datawordL;
	switch (regAddr) {
	case PROFILE_7:
		reglength = 9; // Profile-Register is 8 Bytes long, plus  Instruction-Byte
		switch (ad9957_config.CFR1_OP_MODE) {
		case AD_QDUC:
			datawordL = &reg.REG_QDUC1;
			datawordH = &reg.REG_QDUC0;
			break;
		case AD_ST:
			datawordL = &reg.REG_ST1;
			datawordH = &reg.REG_ST0;
			break;
		}
		break;
	case PROFILE_6:
		reglength = 9; // Profile-Register is 8 Bytes long, plus  Instruction-Byte
		switch (ad9957_config.CFR1_OP_MODE) {
		case AD_QDUC:
			datawordL = &reg.REG_QDUC1;
			datawordH = &reg.REG_QDUC0;
			break;
		case AD_ST:
			datawordL = &reg.REG_ST1;
			datawordH = &reg.REG_ST0;
			break;
		}
		break;
	case PROFILE_5:
		reglength = 9; // Profile-Register is 8 Bytes long, plus  Instruction-Byte
		switch (ad9957_config.CFR1_OP_MODE) {
		case AD_QDUC:
			datawordL = &reg.REG_QDUC1;
			datawordH = &reg.REG_QDUC0;
			break;
		case AD_ST:
			datawordL = &reg.REG_ST1;
			datawordH = &reg.REG_ST0;
			break;
		}
		break;
	case PROFILE_4:
		reglength = 9; // Profile-Register is 8 Bytes long, plus  Instruction-Byte
		switch (ad9957_config.CFR1_OP_MODE) {
		case AD_QDUC:
			datawordL = &reg.REG_QDUC1;
			datawordH = &reg.REG_QDUC0;
			break;
		case AD_ST:
			datawordL = &reg.REG_ST1;
			datawordH = &reg.REG_ST0;
			break;
		}
		break;
	case PROFILE_3:
		reglength = 9; // Profile-Register is 8 Bytes long, plus  Instruction-Byte
		switch (ad9957_config.CFR1_OP_MODE) {
		case AD_QDUC:
			datawordL = &reg.REG_QDUC1;
			datawordH = &reg.REG_QDUC0;
			break;
		case AD_ST:
			datawordL = &reg.REG_ST1;
			datawordH = &reg.REG_ST0;
			break;
		}
		break;
	case PROFILE_2:
		reglength = 9; // Profile-Register is 8 Bytes long, plus  Instruction-Byte
		switch (ad9957_config.CFR1_OP_MODE) {
		case AD_QDUC:
			datawordL = &reg.REG_QDUC1;
			datawordH = &reg.REG_QDUC0;
			break;
		case AD_ST:
			datawordL = &reg.REG_ST1;
			datawordH = &reg.REG_ST0;
			break;
		}
		break;
	case PROFILE_1:
		reglength = 9; // Profile-Register is 8 Bytes long, plus  Instruction-Byte
		switch (ad9957_config.CFR1_OP_MODE) {
		case AD_QDUC:
			datawordL = &reg.REG_QDUC1;
			datawordH = &reg.REG_QDUC0;
			break;
		case AD_ST:
			datawordL = &reg.REG_ST1;
			datawordH = &reg.REG_ST0;
			break;
		}
		break;
	case PROFILE_0:
		reglength = 9; // Profile-Register is 8 Bytes long, plus  Instruction-Byte
		switch (ad9957_config.CFR1_OP_MODE) {
		case AD_QDUC:
			datawordL = &reg.REG_QDUC1;
			datawordH = &reg.REG_QDUC0;
			break;
		case AD_ST:
			datawordL = &reg.REG_ST1;
			datawordH = &reg.REG_ST0;
			break;
		}
		break;
	case RAMSEG0:
		reglength = 7; // Ramsegment-Register is 6 Bytes (48 Bits) long, plus Instruction-Byte
		break;
	case RAMSEG1:
		reglength = 7; // Ramsegment-Register is 6 Bytes (48 Bits) long, plus Instruction-Byte
		break;
	case CFR1:
		reglength = 5;
		datawordL = &reg.REG_CFR1;
		break;
	case CFR2:
		reglength = 5;
		datawordL = &reg.REG_CFR2;
		break;
	case CFR3:
		reglength = 5;
		datawordL = &reg.REG_CFR3;
		break;
	case AUXDAC:
		reglength = 5;
		datawordL = &reg.REG_AUX_DAC;
		break;
	case AMP:
		reglength = 5;
		datawordL = &reg.REG_AMP;
		break;
	case IOUPRATE:
		reglength = 5;
		datawordL = &reg.REG_IOUP_RATE;
		break;
	default:
		return HAL_ERROR;
	}
	uint8_t buf[reglength];
	uint8_t transmitSize = reglength;
	buf[0] = regAddr;
	reglength--;

	// TODO: REWORK THIS SECTION TO MAKE IT COMPATIBLE WITH 6 BYTE LONG REGISTERS!
	if (reglength > 6) {
		for (int i = 0; i < 4; i++) {
			buf[reglength - i] = (uint8_t) (*datawordH >> (i * 8)); // Split the value into 4 Bytes
		}
		reglength = reglength - 4;
	}
	if (reglength > 4) {
		for (int i = 0; i < 2; i++) {
			buf[reglength - i] = (uint8_t) (*datawordH >> (i * 8)); // Split the value into 4 Bytes
		}
		reglength = reglength - 2;
	}
	for (int i = 0; i < 4; i++) {
		buf[reglength - i] = (uint8_t) (*datawordL >> (i * 8)); // Split the value into 4 Bytes
	}
	HAL_GPIO_WritePin(AD9957_IORESET_GPIO_Port, AD9957_IORESET_Pin, GPIO_PIN_SET);	// RESET the Data IO-System
	HAL_GPIO_WritePin(AD9957_IORESET_GPIO_Port, AD9957_IORESET_Pin, GPIO_PIN_RESET);	// Clear Data IO-System Reset
	HAL_GPIO_WritePin(AD9957_CS_GPIO_Port, AD9957_CS_Pin, GPIO_PIN_RESET); // Select the Chip (Start new transfer of data)
	HAL_GPIO_WritePin(AD9957_IOUP_GPIO_Port, AD9957_IOUP_Pin, GPIO_PIN_RESET); // Set the IOUpdate-Pin to LOW
	result = HAL_SPI_Transmit(&hspi2, buf, transmitSize, 800); // Send the Data to the Chip via SPI2
	HAL_GPIO_WritePin(AD9957_IOUP_GPIO_Port, AD9957_IOUP_Pin, GPIO_PIN_SET); // Set the IOUpdate-Pin HIGH to load the data from the shift register into the register set by the instruction-byte
	HAL_GPIO_WritePin(AD9957_CS_GPIO_Port, AD9957_CS_Pin, GPIO_PIN_SET); // Transfer over, Deselect Chip
	return result;
}

HAL_SPI_StateTypeDef AD9957_ReadRegister(uint32_t *dataTarget, uint8_t regAddr) {
	// Read one 32bit Register
	uint8_t regReadAddr = regAddr + 0x80;
	uint8_t buf[4];
	uint32_t data;
	HAL_GPIO_WritePin(AD9957_CS_GPIO_Port, AD9957_CS_Pin, GPIO_PIN_RESET); // Select the Chip
	HAL_GPIO_WritePin(AD9957_IOUP_GPIO_Port, AD9957_IOUP_Pin, GPIO_PIN_RESET); // Set the IOUpdate-Pin to LOW
	HAL_SPI_Transmit(&hspi2, &regReadAddr, 1, 800); // Send the Data to the Chip via SPI2
	HAL_Delay(1);
	HAL_GPIO_WritePin(AD9957_IOUP_GPIO_Port, AD9957_IOUP_Pin, GPIO_PIN_SET); // Set the IOUpdate-Pin HIGH to load the data from the shift register into the register set by the instruction-byte
	HAL_SPI_Receive(&hspi2, buf, 4, 800);
	HAL_GPIO_WritePin(AD9957_CS_GPIO_Port, AD9957_CS_Pin, GPIO_PIN_SET); // Transfer over, Deselect Chip
	data = buf[3] << 24;
	data = data + (buf[2] << 16);
	data = data + (buf[1] << 8);
	data = data + buf[0];
}
