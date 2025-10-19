/******************************************************************************
 *
 *  \brief		Messages queue template class
 *  \details	This class is used to implement queues for arbitrary type of messages
 *  \author 	Matteo De Silvestri - General Medical Merate
 *  GMM.spa - Seriate - Italy
 *  \version	1.0
 *  \date		June 3rd, 2019
 *  \pre		When instantiating need to specify type (IQ_Generic<*type*>)
 *  \warning	Improper use can crash your application
 *  \copyright	GMM.spa - All Rights Reserved
 *
 *****************************************************************************/

#ifndef I_CAN_H
#define I_CAN_H

#ifdef STM32F4XX
extern "C"{
#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include <string.h>
}
#else
/**
  * @brief  CAN Tx message structure definition
  */
typedef struct
{
    uint32_t StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

    uint32_t ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

    uint8_t IDE;     /*!< Specifies the type of identifier for the message that
                        will be transmitted. This parameter can be a value
                        of @ref CAN_identifier_type */

    uint8_t RTR;     /*!< Specifies the type of frame for the message that will
                        be transmitted. This parameter can be a value of
                        @ref CAN_remote_transmission_request */

    uint8_t DLC;     /*!< Specifies the length of the frame that will be
                        transmitted. This parameter can be a value between
                        0 to 8 */

    uint8_t Data[8]; /*!< Contains the data to be transmitted. It ranges from 0
                        to 0xFF. */
} CanTxMsg;

/**
  * @brief  CAN Rx message structure definition
  */
typedef struct
{
    uint32_t StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

    uint32_t ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

    uint8_t IDE;     /*!< Specifies the type of identifier for the message that
                        will be received. This parameter can be a value of
                        @ref CAN_identifier_type */

    uint8_t RTR;     /*!< Specifies the type of frame for the received message.
                        This parameter can be a value of
                        @ref CAN_remote_transmission_request */

    uint8_t DLC;     /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */

    uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to
                        0xFF. */

    uint8_t FMI;     /*!< Specifies the index of the filter the message stored in
                        the mailbox passes through. This parameter can be a
                        value between 0 to 0xFF */
} CanRxMsg;
#endif
#include "IQ_Generic.h"
//#include "ICom.h"

#define MAX_Q_NUM 5
#define MAX_CB_NUM MAX_Q_NUM
#define MAX_FILTERS_NUM	52 //13 * 4

#ifdef STM32F4XX
typedef struct {
    CAN_TypeDef* pCAN;
	uint8_t u8RxPP;
	uint8_t u8RxSP;
	uint8_t u8TxPP;
	uint8_t u8TxSP;
} CAN_Struct;
#endif

class ICan{

public:

	typedef void (*pmf_t)(void);

	typedef enum {
#ifdef STM32F4XX
        MODE_NORMAL = CAN_Mode_Normal,
		MODE_LOOPBACK = CAN_Mode_LoopBack,
		MODE_SILENT = CAN_Mode_Silent,
		MODE_SILENT_LOOPBACK = CAN_Mode_Silent_LoopBack
#else
        MODE_NORMAL,
        MODE_LOOPBACK,
        MODE_SILENT,
        MODE_SILENT_LOOPBACK
#endif
    } enCanMode;

	typedef enum
	{
		CAN_BUS_SPEED_UNKNOWN = -1 /*! Unknown Speed*/,
		CAN_BUS_SPEED_125 = 125 /*! SPEED 125 kb/s */,
		CAN_BUS_SPEED_250 = 250 /*! SPEED 250 kb/s */,
		CAN_BUS_SPEED_500 = 500 /*! SPEED 500 kb/s */,
	} enCanSpeed;

#ifdef STM32F4XX
    ICan(CAN_Struct st_init) {;}
#else
    ICan() {;}
#endif
    virtual ~ICan(void) {;}
    bool init(enCanMode mode) {;}
    void open(long qID) {;}
    void write (const unsigned int msgID, const char *pMsg, unsigned int nBytes) {;}
    void write (CanTxMsg* msg) {;}
    long read(CanRxMsg* msg, long qID = 0) {;}

    bool setMode(enCanMode mode) {;}
	bool isToRead(long qID) { return rxQ[qID].size() > 0; }
#ifdef STM32F4XX
    bool isInError(void)
	{ return ( CAN_GetLastErrorCode(pCANx) != CAN_ErrorCode_NoErr ); }
	uint8_t getErrCntr(void)
	{ return ( CAN_GetReceiveErrorCounter(pCANx) ); }
#else
    bool isInError(void)
    { return false; }
    uint8_t getErrCntr(void)
    { return 0; }
#endif
	bool txPending(void){ return ( txQ.isFull() ); }
	bool rxStatus(long qID = 0) {
		bool result = false;
		if (qID < MAX_Q_NUM)
			result = !rxQ[qID].isFull();

		return result;
	}

	int getRxSize(long qID = 0) { return rxQ[qID].size(); }
	int getTxSize(void) { return txQ.size(); }

#ifdef STM32F4XX
    virtual void rxIRQ(void);
	virtual void txIRQ(void);

	void pauseIT_TX(void);
	void releaseIT_TX(void);
	void pauseIT_RX(void);
	void releaseIT_RX(void);
#endif
	bool isOpen() { return bRunning; }
	bool isOpen(long qID) { return (  isOpen() && qOpened.has(qID) ); }
    void close(long qID) {;}
	enCanSpeed getSpeed(void) { return eCanSpeed; }
    void setSpeed(enCanSpeed speed) { eCanSpeed = speed; }

	static long getInstance(ICan* &pCan);

    bool registerRxCallback(pmf_t pFun) {;}
    bool registerTxCallback(pmf_t pFun) {;}

    bool registerFilter(long qID, uint16_t u16ID) { return true; }
    bool registerFilter(long qID, uint16_t u16ID0, uint16_t u16ID1) { return true; }
    bool registerFilter(long qID, uint16_t u16ID0, uint16_t u16ID1, uint16_t u16ID2) { return true; }
    bool registerFilter(long qID, uint16_t u16ID0, uint16_t u16ID1, uint16_t u16ID2, uint16_t u16ID3) { return true; }

private:

#ifdef STM32F4XX
    CAN_TypeDef* pCANx;
#endif

	uint8_t u8RxPP;
	uint8_t u8RxSP;
	uint8_t u8TxPP;
	uint8_t u8TxSP;
	uint8_t u8FilterNumb;

	bool bInitialized;
	bool bRunning;

#ifdef STM32F4XX
    GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	CAN_InitTypeDef  CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#endif
	enCanSpeed eCanSpeed;
	long lErrCounter;
	static long s32CurrentQ;
	long s32CurrentRxCB;
	long s32CurrentTxCB;

	pmf_t apRxCallbackList[MAX_CB_NUM];
	pmf_t apTxCallbackList[MAX_CB_NUM];
	IQ_Generic<CanRxMsg> rxQ[MAX_Q_NUM];
	IQ_Generic<CanTxMsg> txQ;

	IQ_Generic<long, MAX_Q_NUM> qOpened;
	IQ_Generic<uint16_t, 13 * 4> qFilters[MAX_Q_NUM];

#ifdef STM32F4XX
    void CAN_GPIO_Config(void);
	void CAN_NVIC_Config(void);
	void CAN_Mode_Config(enCanMode CAN_Mode);

	void CAN_IT_Config(void);
	
	void receiveData(void);
	void transmitData(void);

	void SetupTxMessage (const unsigned int msgID, const char *pMsg, unsigned int nBytes);
	void increaseErrorCounter(void);

	void setSpeed(enCanSpeed speed);

	bool filterByIdList(
			uint16_t u16Id01,
			uint16_t u16Id02,
			uint16_t u16Id03,
			uint16_t u16Id04 );
	bool filterByIdMask( uint16_t u16IdAndMask, uint16_t u16IdComp)   ;
	bool filterByRange( uint16_t u16CanFiltLow, uint16_t u16CanFiltHigh);

	void openComm(void);
	void closeComm(void);
	void initFilters(void);

	void getUniqueFilters(IQ_Generic<uint16_t, MAX_FILTERS_NUM>& qDst);
#endif
};


ICan iCan;

extern "C" {
	void CAN1_RX0_IRQHandler(void);
	void CAN1_TX_IRQHandler(void);
}

#endif
