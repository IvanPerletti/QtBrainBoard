/******************************************************************************************
 *
 *  \brief		SPI Pedal Class
 *  \details	This class is used to allow digital Port communication onto S74 board
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

#include "ISpiPedal.h"

//Private Definitions -------------------------------------------------------
#define		SPI_PERIP		GPIOE			/* not Chip Select PORT for SPI2 */
#define		SCK_SDIO		GPIO_Pin_2
#define		MOSI_IO			GPIO_Pin_3
#define		MISO_IO			GPIO_Pin_4
ISpiPedal spiPedal;
//Private functions ----------------------------------------------------------
static void delay()
{
	unsigned long counter = 1;
	while (counter)
		counter--;
}

//-----------------------------------------------------------------------------
ISpiPedal::ISpiPedal() {
	init();

}
//-----------------------------------------------------------------------------
ISpiPedal::~ISpiPedal() {
	// TODO Auto-generated destructor stub
}
//-----------------------------------------------------------------------------
/**
 * @brief Initializes the Pedal SPI pins as GPIO IN & OUT
 */
void ISpiPedal::init(void)
{
#ifdef STM32F4XX
    GPIO_InitTypeDef		GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = nCS_DIN_Pin|nCS_DOUT_Pin|SCK_SDIO|MOSI_IO;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = MISO_IO;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
#endif
	mosiDwn();
	clockDwn();
}

//-----------------------------------------------------------------------------
/**
 * @brief De-Init for the SPI Pedal Port
 */
void ISpiPedal::deInit(void)
{

}
//-----------------------------------------------------------------------------
/**
 * @brief Send a unsigned short word via Pedal SPI selecting the right Chip Select
 * @param u16Val2Send	data to be transferred onto the Pedal SPI Line
 * @param enChipSel		chip select to enable
 */
char ISpiPedal::sendWord(unsigned short u16Val2Send )
{
	long qq=0;
	bool bVal;
	//u16Val2Send = __REV16(u16Val2Send);
	for(qq=15;qq>=0;qq--)
	{
		clockDwn();
		bVal = ((u16Val2Send>>qq)&1);
		if(bVal)
			mosiUp();
		else
			mosiDwn();
		clockUp();
	}

	clockDwn();
	return (0);
}
//-----------------------------------------------------------------------------
/**
 * @brief Method to read 16 bit from the Pedal SPI
 * @param enChipSel		chip select to be enabled for this class
 */
unsigned short ISpiPedal::readWord( void )
{
	signed long qq=0; // wished to be signed
	unsigned short u16Val2Read=0;
	long misoVal;

	for(qq=15;qq>=0;qq--)	{
		clockDwn();
		misoVal = misoState();
		if(misoVal)
			u16Val2Read |= (misoVal<<qq);
		clockUp();
	}
	clockDwn();
#ifdef STM32F4XX
	u16Val2Read = __REV16(u16Val2Read);
#endif
	return(u16Val2Read);
}
//-----------------------------------------------------------------------------
char ISpiPedal::write(unsigned short * u16aVal2Send,
		unsigned short numChars,
		enumSpiPedalCS enChipSel)
{
	int ii;
	char error = 1;

	enableCS(enChipSel);
	if(numChars < 1024)	{
		for(ii=0; ii<numChars; ii++)
			sendWord(u16aVal2Send[ii]);
	}
	disableCS(enChipSel);
	return(error);
}
//-----------------------------------------------------------------------------
char ISpiPedal::read(unsigned short * u16aVal2Read,
		unsigned short numChars,
		enumSpiPedalCS enChipSel)
{
	int ii;
	char error = 1;
	disableCS(enChipSel);
	enableCS(enChipSel);
	disableCS(enChipSel);
	if(numChars < 1024)
	{	
		error = 0;
		for(ii=0; ii<numChars; ii++)
			u16aVal2Read[ii] = readWord();
		
	}
	return(error);
}
//Private Class methods -------------------------------------------------------
/**
 * @brief Private method to enable the SPI
 * @param enCS
 */
inline void ISpiPedal::enableCS(enumSpiPedalCS enCS)
{
#ifdef STM32F4XX
	SPI_PERIP->BSRRH = enCS;
#endif
}
//-----------------------------------------------------------------------------
/**
 * @brief	Private method to disable chip select
 * @param enCS
 */
inline void ISpiPedal::disableCS(enumSpiPedalCS enCS)
{
#ifdef STM32F4XX
    SPI_PERIP->BSRRL = enCS;
#endif
}
//-----------------------------------------------------------------------------
/**
 * @brief Private method to rise clock
 */
inline void ISpiPedal::clockUp(void)
{
#ifdef STM32F4XX
	SPI_PERIP->BSRRL=SCK_SDIO;
#endif
	delay();
}
//-----------------------------------------------------------------------------
/**
 * @brief Private method to drop clock
 */
inline void ISpiPedal::clockDwn(void)
{
#ifdef STM32F4XX
	SPI_PERIP->BSRRH=SCK_SDIO;
#endif
	delay();
}
//-----------------------------------------------------------------------------
/**
 * @brief	Private method to rise MOSI pin
 */
inline void ISpiPedal::mosiUp(void)
{
#ifdef STM32F4XX
	SPI_PERIP->BSRRL=MOSI_IO;
#endif
}
//-----------------------------------------------------------------------------
/**
 * @brief	Private method to drop MOSI pin
 */
inline void ISpiPedal::mosiDwn(void)
{
#ifdef STM32F4XX
    SPI_PERIP->BSRRH=MOSI_IO;
#endif
}
//-----------------------------------------------------------------------------
/**
 * @brief	Private method to read MISO pin state
 */
inline unsigned char ISpiPedal::misoState(void)
{
#ifdef STM32F4XX
    return ( GPIO_ReadInputDataBit(SPI_PERIP, MISO_IO) );
#else
    return 0;
#endif
}
