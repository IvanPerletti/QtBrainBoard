/**
 *  \brief		Analog Port Class for STM32F4 S74 board
 *  \details	This class is used to configure PE14 PWM at 1kHz
 *  \author		Ivan Perletti - General Medical Merate- GMM.spa - Seriate
 *  \version	1.00
 *  \date		December 25th, 2014
 *  \pre		Simply include the header file "watchDog_i1.h"
 *  \warning	Improper use may crash your application
 *  \copyright	GMM.spa - All Rights Reserved
 */
///-----------------------------------------------------------------------------
#include "TAnalogPort.h"

//Private Inclusions ----------------------------------------------------------
#ifdef STM32F4XX
extern "C" {
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
}

#ifndef _7X_S74_BOARD_
#error "This file is actually for S74 board"
#endif
#endif

TAnalogPort tAnalogPort; // class definition

//_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_
// Class methods
TAnalogPort::TAnalogPort(void)
: bIsOpen (0)
{
	for(int ii=0; ii< ANALOG_NUMEL; ii++)
		u16aAdcValue[ii]=0;

	init();
}
//-----------------------------------------------------------------------------
TAnalogPort::~TAnalogPort(void)
{
	deInit();
	bIsOpen = 0;
}
//-----------------------------------------------------------------------------
/**
 * @brief Reads the analog port and gives the value
 * @param portNum	portNum to be read
 */
long TAnalogPort::read ( enumAnalogPort portNum )
{
	short u16Position = portNum;
#ifdef STM32F4XX
	assert_param(IS_ANALOG_PORT(portNum));
#endif

	if (portNum > AN_SW)
	{
		u16Position = AN_SW;
		selectPlexer(portNum - AN_SW -1);
		return( plexerAdcRead () );
	}
	return (u16aAdcValue[u16Position]);
}

//-----------------------------------------------------------------------------
/**
 * @brief Reset and De-initializes the analog peripheral
 */
void TAnalogPort::deInit(void)
{
	close();
#ifdef STM32F4XX
	DMA_DeInit(DMA2_Stream2); // please refers to DM310020 -> "DMA2 request mapping"
	ADC_DeInit();
#endif
	//	don't type GPIO_DeInit: it will de init other Pins Functions'
}
//-----------------------------------------------------------------------------
/**
 * @brief Opens the Analog Port
 * @pre The port has to be initialized
 */
void TAnalogPort::open(void)
{
#ifdef STM32F4XX
	ADC_DMACmd(ADC2, ENABLE);	/* Enable ADC2 DMA */
#endif
	bIsOpen = true;

}
//-----------------------------------------------------------------------------
/**
 * @brief Clsoes the current analog channel. The Peripheral is not de-initialized.
 * @remark The analog port can be then opened calling TAnalogPort::open(void)
 */
void TAnalogPort::close(void)
{
#ifdef STM32F4XX
	ADC_DMACmd(ADC2, DISABLE);	/* Enable ADC2 DMA */
#endif
	bIsOpen = false;

}
/**
 * @brief Tells if the analog port is still open
 * @return TRUE or FALSE
 */
bool TAnalogPort::isOpen(void)  {return (bIsOpen);}
//Private Functions -----------------------------------------------------------
/**
 * @brief Configuration set for Dynamic Memory Access, GPIO and ADC
 */
void TAnalogPort::init(void)
{
#ifdef STM32F4XX
	GPIO_InitTypeDef GPIO_InitStructure;

	deInit();
	/* Enable ADC2, DMA2 and GPIO clocks ***************/
	RCC_AHB1PeriphClockCmd(
			RCC_AHB1Periph_DMA2		|
			RCC_AHB1Periph_GPIOA	|
			RCC_AHB1Periph_GPIOB	|
			RCC_AHB1Periph_GPIOC	,
			ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	configDMA();

	/* Configure ADC2 Channel12 pin as analog input *****/

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin =
			GPIO_Pin_0 |
			GPIO_Pin_3 |
			GPIO_Pin_4 |
			GPIO_Pin_5 |
			GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0	|	GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	configADC();
	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE);
	open();
	configMultPlx();

	/* Enable ADC2 */
	ADC_Cmd(ADC2, ENABLE);
	ADC_SoftwareStartConv(ADC2);
#endif
}
//-----------------------------------------------------------------------------
/**
 * @brief Configuration function for Direct Memory Access 2
 */
void TAnalogPort::configDMA(void)
{
#ifdef STM32F4XX
	DMA_InitTypeDef DMA_InitStructure;
	/* DMA2 Stream0 channel0 configuration ****/
	DMA_DeInit(DMA2_Stream2); // please refers to DM310020 -> "DMA2 request mapping"
	DMA_InitStructure.DMA_Channel = DMA_Channel_1;// please refers to DM310020 -> "DMA2 request mapping"
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (uint32_t)&ADC2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) & u16aAdcValue[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 8; // Same as ADC_InitStructure.ADC_NbrOfConversion
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // orig dis
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //orig dis
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream2, ENABLE);
#endif
}
//------------------------------------------------------------------------------
/**
 * @brief Configuration for GPIO to become Analog Input
 */
void TAnalogPort::configGpioForADC(void)
{
#ifdef STM32F4XX
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure ADC2 Channel12 pin as analog input *****/
	GPIO_InitStructure.GPIO_Pin =
			GPIO_Pin_0 |
			GPIO_Pin_3 |
			GPIO_Pin_4 |
			GPIO_Pin_5 |
			GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // PA0, PA3, PA4, PA5, PA6

	GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0	|	GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);// PB0, PB1

	GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0;
	GPIO_Init(GPIOC, &GPIO_InitStructure);// PC0
#endif
}
//------------------------------------------------------------------------------
/**
 * @brief Configuration for ADC
 */
void TAnalogPort::configADC(void)
{
#ifdef STM32F4XX
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	/* IMPORTANT: populates structures with reset values */
	ADC_StructInit(&ADC_InitStructure);
	ADC_CommonStructInit(&ADC_CommonInitStructure);

	/* ADC Common Init ****/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//We will convert multiple channels
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC2 Init ****/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; // select Scan conversion mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // select continuous conversion mode
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//select no external triggering
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;// right 12-bit data alignment in ADC data register
	ADC_InitStructure.ADC_NbrOfConversion = 8; // same num as 	DMA_InitStructure.DMA_BufferSize = 8;
	ADC_Init(ADC2, &ADC_InitStructure);

	/* ADC2 regular channel12 configuration ****/
	//	Check STM32F4 "Extension Connectors"
	//	ADC123_IN0		PA0		Alimentazione +24V
	//	ADC123_IN3		PA3		Filamento rad (mA 1V@50mA)
	//	ADC123_IN4		PA4		Filamento scopia (mA 1V@1mA)
	//	ADC12_IN5		PA5		Feedback Carica condensatori
	//	ADC12_IN6		PA6		FeedBack Dac Fil
	//	ADC123_IN8		PB0		Alimentazione +2,5V
	//	ADC123_IN9 		PB1		FeedBack Dac kV
	//	ADC123_IN10		PC0		Multiplexer

	ADC_RegularChannelConfig(ADC2, ADC_Channel_0,  1, ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_3,  2, ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_4,  3, ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_5,  4, ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_6,  5, ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_8,  6, ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_9,  7, ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 8, ADC_SampleTime_15Cycles);

	/* Enable ADC1 to work with Plexer*****************************************/
	/* IMPORTANT: populates structures with reset values */
	ADC_StructInit(&ADC_InitStructure);
	ADC_CommonStructInit(&ADC_CommonInitStructure);
	/* enable ADC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/* init ADCs in independent mode, div clock by two */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* init ADC1: 12bit, single-conversion */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
	ADC_InitStructure.ADC_ExternalTrigConv = 0;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Enable ADC1 ***********************************************************/
	ADC_Cmd(ADC1, ENABLE);
#endif
}
//------------------------------------------------------------------------------
/**
 * @brief Setup for the pins to be used as multiplexer channel selection
 */
void TAnalogPort::configMultPlx(void)
{
#ifdef STM32F4XX
	GPIO_InitTypeDef GPIO_InitStruct;
	// according to elec scheme the pin PE8, PD13, PD15 has to be used as
	// multiplexer channel selection
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;		//Set pins to output
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  //Set GPIO clock speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//Set pin type to push/pull
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;	//Set pullup/pulldown resistors to be inactive
	GPIO_Init(GPIOD, &GPIO_InitStruct);		//Initialize GPIOD}
#endif
}
//-----------------------------------------------------------------------------
/**
 * @brief	Private method select the plexer channel by moving 3 plx pins
 */
void TAnalogPort::selectPlexer(short s16PlxSelect)
{
#ifdef STM32F4XX
	if ((s16PlxSelect>>3) != 0x00 ) // == between 0 and 7
		s16PlxSelect = 0;
	
	GPIOD->BSRRH = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	
	if(s16PlxSelect & 0x01)
		GPIOD->BSRRL = GPIO_Pin_13;// GPIO SetBits ==> BSRRL

	if(s16PlxSelect & 0x02)
		GPIOD->BSRRL = GPIO_Pin_14;// GPIO SetBits ==> BSRRL

	if(s16PlxSelect & 0x04)
		GPIOD->BSRRL = GPIO_Pin_15;// GPIO SetBits ==> BSRRL
#endif
	
}
//-----------------------------------------------------------------------------
long TAnalogPort::plexerAdcRead(void)
{
	long lCntDwn = 100;
#ifdef STM32F4XX
	/* Configure Channel */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_15Cycles);

	/* check if conversion was started, if not start */
	ADC_SoftwareStartConv(ADC1);

	/* wait for end of conversion */
	while(	lCntDwn --> 0										&&
			(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)	);

	if (lCntDwn > 0)
		lCntDwn = ADC_GetConversionValue(ADC1);
	else
		lCntDwn = 0;
#endif
	return (lCntDwn);
}
//-----------------------------------------------------------------------------

