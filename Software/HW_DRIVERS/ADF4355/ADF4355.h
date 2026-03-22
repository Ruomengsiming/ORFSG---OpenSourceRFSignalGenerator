/*
 * ADF4355.h
 *
 *  Created on: Aug 23, 2020
 *      Author: voyag
 */

#ifndef INC_ADF4355_H_
#define INC_ADF4355_H_

#include "stm32f4xx_hal.h"
#include "math.h"

typedef struct ADF4355_Configuration{	// Configuration-Data and Mirror of ADF4355-Registers in CPU-RAM. Last 4 bit of each 32bit value indicate the Register-Address and are constant
	double REFCLK;			// Reference Clock for the PLL
	double PFD;				// Phase Frequency Detector-Frequency
	unsigned char RFDIV;	// RF-Divider-Configuration (VCOs canot generate low frequencies, so the high frequencies need to be divided down)
	double N;				// N-Counter Configuration for the PLL
	double FRAC1;			// Fractional 1 Counter Config for PLL
	double FRAC2;			// Fractional 2 Counter Config for PLL
	unsigned int MOD1;		// Modulus 1
	unsigned int MOD2;		// Modulus 2
	uint8_t ERROR;			// Error-Variable (Subroutines store error-message here)
	uint8_t MODE;			// Operation-Mode
	int AMPLITUDE;			// Amplitude-Configuration (selects the Amplitude from the chip (Avaiable: -5dBm to +5dBm)
	double FREQUENCY;		// Currently generated Frequency
	double PHASE;			// Currently set Phase

	// REGISTER 0
	int R0_PRESCAL;			// Dual Modulus Prescaler
	int R0_AUTOCAL;			// Autocal-Select (Should only be off for fixed frequency applications)
	int R0_N;				// INTEGER-Value of the Fractional N-PLL
	int R0_Nmin; 			// Minimal Value of N. Determined by Prescaler-Value. Nmin = 23 if Prescaler = 4/5-Mode

	// REGISTER 1
	int R1_FRAC1;			// 24bit Main Fractional Value

	// REGISTER 2
	int R2_FRAC2_LSB;			// 14bit Auxilliary Fractional Value
	int R2_MOD2_LSB;			// Auxiliary Modulus Value

	// REGISTER 3
	int R3_SD_LOAD_RESET;	// Sigma Delta Modulator-RESET ENABLE <- Disable for Operating Modes that involve Phase Modulation
	int R3_PHASE_RESYNC;	// Phase Resync ENABLE - ONLY WORKS WHEN FRAC2 = 0 and needs PLL-Feedback to come from Divider if Frequency is less than 3.4 GHz
	int R3_PHASE_ADJUST;	// Phase Adjust ENABLE - A 0 here turns off Phase Adjust Features
	int R3_PHASE_VALUE;		// Phase Value in Steps of 0.000021457672119140625 degrees

	// REGISTER 4
	int R4_MUXOUT;			// Muxout-Selector
	int R4_REFDOUBLE;		// Reference-Doubler
	int R4_REFDIV2;			// Reference-Prescaler (Divide by 2)
	int R4_RCOUNTER;		// 10bit Reference-Counter
	int R4_DOUBLEBUF;		// Enable or Disable Double Buffering
	int R4_CHGPUMP;			// Chargepump-Current
	int R4_REFMODE;			// Differential or Single Ended Reference
	int R4_MUXLOG;			// MuxOUT Level Select (1.8V or 3.3V)
	int R4_PDPOLA;			// Phase Detector Polarity
	int R4_PWRDN;			// Powerdown-Configuration Bit
	int R4_CP3STATE;		// Chargepump Threestate
	int R4_COUNTRES;		// Counter Reset

	// REGISTER 5
	// REGISTER 5 IS RESERVED AND NEEDS TO BE SET TO 0x00800025

	// REGISTER 6
	int R6_BLEED_POLARITY;	// Set the Bleed Polarity
	int R6_GATED_BLEED;		// Gated Bleed: Bleed Feature only enabled AFTER Lock
	int R6_NEGATIVE_BLEED;	// Negative Bleed Enable
	int R6_RFOUTB_SELECT;	// What will be sent out at RFOUTB
	int R6_FEEDBACK_SELECT;	// Feedback-Selector: 1 = VCO, 0 = Divided
	int R6_RF_DIV_SEL;		// RF-Divider: 0 - 64
	int R6_BLEED_CURRENT;	// Bleed-Current Select: 3.75uA Steps from 3.75uA up to 956.25uA. Best Value = 4 / N < Ibleed / IchargePump < 10 / N with N being the value of the Feedback-Counter of the PFD
	int R6_MUTE_TILL_LOCK;	// Disable Output until LOCK is Achieved
	int R6_AUXRF_ENABLE;	// Enable/Disable RFOUT_B
	int R6_AUXRF_PWR;		// Select the Output-Power of RFOUT_B: -4dBm (0), -1dBm (1), +1dBm (2), +5dBm (3)
	int R6_RFOUT_ENABLE;	// Enable/Disable RFOUT_A
	int R6_RFOUT_PWR;		// Select the Output-Power of RFOUT_B: -4dBm, -1dBm, +1dBm, +5dBm

	// REGISTER 7
	int R7_LE_SYNC_EDGE;
	int R7_LE_SYNC;			// Load Enable Sync: If Enabled, the Latch of the shift register is Synchronized with the Reference-Clock
	int R7_LDC;				// Lock Detect-Count: How many Cycles until Lock-Detect Circuit reports Lock
	int R7_LOL_MODE;		// Loss of Lock-Mode: SET TO 0! Datasheet states LOL-Mode does not work with differential RefCLK
	int R7_LDP;				// Lock Detect Precision: Should be set to 12ns = 11
	int R7_LDM;				// Lock Detect-Mode: Set to 0 if Fractional N-Mode is used

	// REGISTER 8:
	// REGISTER 8 IS RESERVED AND NEEDS TO BE SET TO 0x102D0428
	int R8;

	// REGISTER 9:
	unsigned char R9_VCO_BAND_DIV;	// 8bit Divider-Value for VCO Band Division-Clock. Determine by: (PFD / (Band-Division * 16)) < 150kHz
	int R9_TIMEOUT;			// Timeout for VCO Band-Division Select - Used as variable in the next Timeout-Settings
	int R9_AUTOLVL_CAL_TIMEOUT;	// Timeout for the VCO Automatic Level Calibration. Configure thus: ((Timeout * ALC WAIT) / PFD Frequency) > 50us
	int R9_SYNTH_LOCK_TIMEOUT;	// Timeout for the Synthersizer Lock Timeout: Configure thus: ((Timeout * Synth Lock Timeout) / PFD Frequency) > 20us

	// REGISTER 10
	int R10_ADC_CLK_DIV;	// Divider for the internal ADC. ADC-CLK = R counter / ADC_CLK_DIV. Configure thus: ADC_CLK_DIV = (((PFD Frequency / 100000) - 2) / 4) and Round up to nearest Integer
	int R10_ADC_CONV;		// ADC Conversion Enable: A 1 here starts an ADC-Conversion. Always write a 1 here
	int R10_ADC_ENABLE;		// Enable or Disable the ADC. Always Enable the ADC (Write 1)!

	// REGISTER 11
	// REGISTER 11 IS RESERVED AND NEEDS TO BE SET TO 0x0061300B
	int R11;

	// REGISTER 12
	int R12_PHASE_RESYNC_CLK_DIV;	// Timeout-Counter for Phase Resync Activation. Set to: Phase Resync Cycles / PFD Frequency > PLL Lock-Time (See ADISIMPll for Lock-Times + Experimentation)

	// REGISTER 13
	uint16_t R13_FRAC2_MSB;
	uint16_t R13_MOD2_MSB;

	unsigned int REG0;
	unsigned int REG1;
	unsigned int REG2;
	unsigned int REG3;
	unsigned int REG4;
	unsigned int REG5;
	unsigned int REG6;
	unsigned int REG7;
	unsigned int REG8;
	unsigned int REG9;
	unsigned int REG10;
	unsigned int REG11;
	unsigned int REG12;
	unsigned int REG13;
}ADF_CONF;

#define ADF_DIRECT_CTRL 0xDC	// Sets the DIRECT-Control: This will send data through the FPGA directly to the ADF4355
#define ADF_FPGA_CTRL 0xFC		// Sets the FPGA-Control: This will send data to a Buffer inside the FPGA and set up the FPGA

#define FREQ_UPDATE 0xF0
#define PHASE_UPDATE 0xE0
#define AMPLITUDE_UPDATE 0xA0

#define ADF_COLDSTART 0x01
#define ADF_WARMSTART 0x00

#define ADF_MODE_ACT 0x01
#define ADF_MODE_STDBY 0x00

#define ADF4355_CS_Pin GPIO_PIN_13
#define ADF4355_CS_GPIO_Port GPIOE
#define ADF4355_LE_Pin GPIO_PIN_14
#define ADF4355_LE_GPIO_Port GPIOE
#define ADF4355_MUX_Pin GPIO_PIN_15
#define ADF4355_MUX_GPIO_Port GPIOE

HAL_StatusTypeDef ADF4355_Setup(uint8_t startup, long refCLK, uint8_t mode, int MOD2);
HAL_StatusTypeDef ADF4355_SetFrequency(double frequency);	// Sets the output-frequency.
HAL_StatusTypeDef ADF4355_SetPhase(float phase);
HAL_StatusTypeDef ADF4355_GetConfig(int *responseBuffer);

#endif /* INC_ADF4355_H_ */
