/******************************************************************************************
 *
 *  \brief		SPI Pedal Class
 *  \details	This class is used to allow digital Port communication onto MainController
 *  by using SPI2 to talk with 5 shift register for output and listen to 4 shift register for
 *  Input. Developed for STM32F4 chip
 *  \author		Ivan Perletti - General Medical Merate- GMM.spa - Seriate - Italy
 *  \version	1.00
 *  \date		Nov 11th, 2014
 *  \pre
 *  \bug		Not all memory is freed when deleting an object of this class.
 *  \warning	Improper use can crash your application
 *  \copyright GMM.spa - All Rights Reserved
 *
 ******************************************************************************************/
#ifndef ISPIPEDAL_H_
#define ISPIPEDAL_H_

#ifdef STM32F4XX
extern "C"
{
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
}
#else
#include <stdint.h>

#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */
#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /* All pins selected */
#endif

/* Private Definitions ------------------------------------------------------*/

#define nCS_DIN_Pin		(int) (GPIO_Pin_0)	/* Chip Select pin for Digital Input*/
#define nCS_DOUT_Pin	(int) (GPIO_Pin_1)	/* Chip Select pin for Digital Output*/

typedef enum { eCS_DI = nCS_DIN_Pin, eCS_DO = nCS_DOUT_Pin} enumSpiPedalCS; /*! enum Chip Select */


class ISpiPedal {
public:
	ISpiPedal();
	virtual ~ISpiPedal();
	static void init(void);
	static void deInit(void);
	static char sendWord(unsigned short u16Val2Send);
	static unsigned short readWord(void);
	static void enableCS(enumSpiPedalCS enCS); // to be public
	static void disableCS(enumSpiPedalCS enCS); // to be public
	static char write(unsigned short * u16aVal2Send,
			unsigned short numChars,
			enumSpiPedalCS enChipSel = eCS_DO);
	static char read(unsigned short * u16aVal2Read,
			unsigned short numChars,
			 enumSpiPedalCS enChipSel = eCS_DI);
private:
	//-----------------------------------------------------------------------------

	static void clockUp(void);
	static void clockDwn(void);
	static void mosiUp(void);
	static void mosiDwn(void);
	static unsigned char misoState(void);

	//	void restart(void);
	//	void open(void);
	//	void close(void);
	//	unsigned char isOpen(void);
	//	unsigned char read ( unsigned short * u16pArray, char s8ArraySz, enumSpiCS spiCS);
	//	unsigned char write ( unsigned short * u16pArray, char s8ArraySz, enumSpiCS eSpiCS);
	//	unsigned char readWrite (
	//			unsigned short * u16ArrRx,
	//			unsigned short * u16ArrTx,
	//			char s8ArraySz, enumSpiCS eSpiCS);
	//	void sendWord(unsigned short u16Val2Send);
	//private:

	//	void configCS(void);
	//	void enableCS(enumSpiCS u16CS_Pin);
	//	void disableAllCS(void);
	//	void SpiEnableChannel (enumRxTxEnabl enRxTx, long s32BuffSz);
	//	void SpiDisableChannel (void);
};

extern ISpiPedal spiPedal;
#endif /* ISPIPEDAL_H_ */
