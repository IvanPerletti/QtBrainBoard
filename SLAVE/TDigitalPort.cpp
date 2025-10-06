/******************************************************************************************
 *
 *  \brief      DIGITAL PORT Class
 *  \details    This class is used to configure digital Port communication onto MainController
 *  by using the 4 shift register for Input and 5 shift register for output. Developed for
 *  STM32F4 chip
 *  \author     Ivan Perletti - General Medical Merate- GMM.spa - Seriate - Italy
 *  \version    1.02
 *  \date       October 14th, 2014
 *  \pre        SpiLineINPUT e SpiLinOUTPUT to be initialized 16B per Word
 *  \bug        Not all memory is freed when deleting an object of this class.
 *  \warning    Improper use can crash your application
 *  \copyright GMM.spa - All Rights Reserved
 *
 ******************************************************************************************/

#include "TDigitalPort.h"
#ifdef STM32F4XX
#include "core_cmInstr.h"

#ifndef _7X_S74_BOARD_
#error "This file is actually for S74 board"
#endif
#endif

TDigitalPort digitalPort(&spiPedal,&spiPedal);
//-----------------------------------------------------------------------------------------
/**
 * @brief Constructor
 * @param tSpixAdrres	Address to @TSpi (tSpi have to be already initialized)
 */
TDigitalPort::TDigitalPort( ISpiPedal *pSpiPedalIN , ISpiPedal *pSpiPedalOUT )
:	pSpiPedalIn(pSpiPedalIN),
 	pSpiPedalOut(pSpiPedalOUT),
 	ulInputWord(0),
 	ulOutputWord(0)
{

	resetAllOUT();
	updateOUT();
	bDirectOut_initialized = 0;
	initDirectOut();
}
//------------------------------------------------------------------------------
TDigitalPort::~TDigitalPort()
{
	ulInputWord = 0x00;
	ulOutputWord = 0x00;
	bDirectOut_initialized = 0;
}
//------------------------------------------------------------------------------
/**
 * @brief <b> Suspensive code</b>: it reads the actual digital Input on the
 * shift register via SPI2
 * @return  actual input word (32bit) read from shift register
 */
uint32_t TDigitalPort::updateIN(void)
{
	//	char error = readFromSPI(&llInputWord);
    readFromSPI(&ulInputWord);
	return(ulInputWord);
}
//------------------------------------------------------------------------------
/**
 * @brief <b> Suspensive code</b>: it transfers the actual long long spiWord to
 *  the shift register via SPI2
 * @return      error code:
 * 			- 0x00 no error
 * 			- 0x01 timeout error
 */
char TDigitalPort::updateOUT(void)
{
	char error = writeOnSPI(ulOutputWord);
	return ( error );
}
//------------------------------------------------------------------------------
/**
 * @brief <b> Suspensive code</b>: it transfers the actual long long spiWord to
 *  the shift register via SPI2
 * @return error status
 */
char TDigitalPort::updateOUT(long int llTxSPIword)
{
	char error = writeOnSPI(llTxSPIword);
	return ( error );
}
//------------------------------------------------------------------------------
/**
 * @brief <b> Suspensive code </b>: Immediately set the shiftRegister output
 *  to previously set value
 * @param 	outputPortNum
 * @return	error code:
 * 						- 0x00 no error
 * 						- 0x01 timeout error
 */
char TDigitalPort::setNow(enumDigitalOut outputPortNum)
{
	char error;
	set(outputPortNum);
	error = writeOnSPI(ulOutputWord);
	return(error);
}
//------------------------------------------------------------------------------
/**
 * @brief <b> Suspensive code </b>: Immediately set all the shiftRegister
 * output to ZERO
 * @param outputPortNum
 * @return	error code:
 * 						- 0x00 no error
 * 						- 0x01 timeout error
 */
char TDigitalPort::resetNow(enumDigitalOut outputPortNum)
{
	char error;
	reset(outputPortNum);
	error = writeOnSPI(ulOutputWord);
	return(error);
}
//------------------------------------------------------------------------------
/**
 * @brief <b> Suspensive code </b>: Immediately set the shiftRegister output
 *  to previously set value
 * @param 	outputPortNum
 * @return	error code:
 * 						- 0x00 no error
 * 						- 0x01 timeout error
 */
char TDigitalPort::setNow(enumDirectOut outputPortNum)
{
#ifdef STM32F4XX
    GPIO_SetBits(GPIOE, outputPortNum);
#endif
	return(0);
}
//------------------------------------------------------------------------------
/**
 * @brief <b> Suspensive code </b>: Immediately set all the shiftRegister
 * output to ZERO
 * @param outputPortNum
 * @return	error code:
 * 						- 0x00 no error
 * 						- 0x01 timeout error
 */
char TDigitalPort::resetNow(enumDirectOut outputPortNum)
{
#ifdef STM32F4XX
    GPIO_ResetBits(GPIOE, outputPortNum);
#endif
	return(0);
}

//------------------------------------------------------------------------------
/**
 * @brief Reads the shift-register Input Port Number.
 * @remark Value is referred to last historical update (made by
 *  "TDigitalPort::updateIN()" * call)
 * @param inputPortNum		number of the input port to be read
 * @return Input Port Status
 * 						- 0x00 LOW
 * 						- 0x01 HIGH
 * 						- 0xFF inputPortNum does not correspond to real port
 */
pinStatus TDigitalPort::check(enumDigitalIn inputPortNum)
{
	int llBitMask = 0x00000001;

	llBitMask = llBitMask << (inputPortNum-1);
	if (0<inputPortNum && inputPortNum<=32)
	{
		if (ulInputWord & llBitMask)
			return(HIGH);// result > 0
		else
			return(LOW);// result == 0
	}
	return(NOPE);// error inputPortNum does not correspond to real port
}
//------------------------------------------------------------------------------
/**
 * @brief Reads the shift-register Input Port Number.
 * @overload pinStatus TDigitalPort::check(enumDigitalOut inputPortNum)
 * @remark Value is referred to last historical update (made by
 *  "TDigitalPort::updateIN()" call)
 * @param outPortNum		number of the input port to be read
 * @return Input Port Status
 * 						- 0x00 LOW
 * 						- 0x01 HIGH
 * 						- 0xFF inputPortNum does not correspond to real port
 */
pinStatus TDigitalPort::check(enumDigitalOut outPortNum)
{
	unsigned long ulBitMask = 0x01;

	ulBitMask = ulBitMask << (outPortNum-1);
	if (0<outPortNum && outPortNum<=32){
		if (ulOutputWord & ulBitMask)
			return(HIGH);// result > 0
		else
			return(LOW);// result == 0
	}
	return(NOPE);// error outPortNum does not correspond to real port
}
//------------------------------------------------------------------------------
/**
 * @brief Reads the shift-register Input Port Number.
 * @overload pinStatus TDigitalPort::check(enumDigitalOut inputPortNum)
 * @remark Value is referred to last historical update (made by
 *  "TDigitalPort::updateIN()" call)
 * @param outPortNum		number of the input port to be read
 * @return Input Port Status
 * 						- 0x00 LOW
 * 						- 0x01 HIGH
 * 						- 0xFF inputPortNum does not correspond to real port
 */
pinStatus TDigitalPort::check(enumDirectOut enputPortNum)
{
	if (enputPortNum)
		return(HIGH);// result > 0
	else
		return(LOW);// result == 0

}
//------------------------------------------------------------------------------
/**
 * @brief Prepare the output port to be set HIGH.
 * @remark The effectiveness will come after TDigitalPort::updateOUT(void) call
 * @param outputPortNum		number of the output port to be set
 * @return error status
 */
void TDigitalPort::set(enumDigitalOut outputPortNum)
{
	unsigned long ulBitMask = 0x01; // this prevent 0xffffff000000000 error;
	if (0<outputPortNum && outputPortNum<=32)
	{
		ulBitMask = ulBitMask << (outputPortNum-1);
		ulOutputWord |= ulBitMask;
	}
}

//------------------------------------------------------------------------------
/**
 * @brief Prepare the output port to be set LOW.
 * @remark The effectiveness will come after TDigitalPort::updateOUT(void) call
 * @param outputPortNum		number of the output port to be set
 * @return error status:
 * 		-	0x00 no error
 * 		- 	0x01 no valid output port number
 */
void TDigitalPort::reset(enumDigitalOut outputPortNum)
{
	unsigned long ulBitMask = 0x01;// this prevent 0xffffff000000000 error;
	if (0<outputPortNum && outputPortNum<=32)
	{
		ulBitMask = ulBitMask << (outputPortNum-1);
		ulBitMask = ~ulBitMask;
		ulOutputWord &= ulBitMask;
	}
}
//------------------------------------------------------------------------------
/**
 * @brief Prepare the output port to be set LOW or HIGH.
 * @remark The effectiveness will come after TDigitalPort::updateOUT(void) call
 * @param outputPortNum		number of the output port to be set
 * @return error status:
 * 		-	0x00 no error
 * 		- 	0x01 no valid output port number
 */
void TDigitalPort::assign(enumDigitalOut outputPortNum, pinStatus lowOrHigh)
{
	if (lowOrHigh == LOW)
		reset( outputPortNum);
	else //  (lowOrHigh == 'HIGH')
		set( outputPortNum);
}
//------------------------------------------------------------------------------
/**
 * @brief <b>NON Suspensive code</b>: it wipes the output status message
 * @remark The effectiveness will come after
 * TDigitalPort::setNow(enumDigitalOut outputPortNum) call
 */
void TDigitalPort::resetAllOUT(void)
{
	ulOutputWord = 0x0000000000;
	// shift left error (so see senWord error legend and shift )
}
//-----------------------------------------------------------------------------
/**
 * @brief Initializes the Direct Out pins
 */
void TDigitalPort::initDirectOut(void)
{
#ifdef STM32F4XX
    GPIO_InitTypeDef  GPIO_InitStructure;
    /*
    PE_5 nu
    PE_8 nu
    PE9		 COM RAD
    PE10	 COM PREP
    PE11 	 LED GREEN LED
    PE12 	 LED RED LED
    PE_13 nu
    PE14	 LED WDOG
     */
    if (bDirectOut_initialized == false)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10  | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14;
        GPIO_Init(GPIOE, &GPIO_InitStructure);

        bDirectOut_initialized = true;
    }
#endif
}
//------------------------------------------------------------------------------
char TDigitalPort::writeOnSPI(unsigned long ulTxSPIword)
{
	char error;

	u16ArrTx[0] = (ulTxSPIword>>16) & 0xFFFF;
	u16ArrTx[1] = ulTxSPIword & 0xFFFF;

	error = pSpiPedalOut->write(u16ArrTx, 2);

	return(error);
}
//------------------------------------------------------------------------------
char TDigitalPort::readFromSPI(unsigned long * pulRxSPIword)
{
	unsigned short error = 0x00; // this prevent 0xffffff000000000 error;

	error = pSpiPedalIn->read(u16ArrRx, 2);
    *pulRxSPIword = (uint32_t)((u16ArrRx[1]<<16) +u16ArrRx[0]);
	return(error);
}

