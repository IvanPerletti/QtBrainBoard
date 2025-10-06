/******************************************************************************************
 *
 *  \brief              DIGITAL PORT Class
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

#ifndef TDIGITALPORT_H_
#define TDIGITALPORT_H_

#ifdef STM32F4XX
extern "C" {
#include <stm32f4xx.h>
#include <stm32f4xx_spi.h>
#include <stm32f4xx_gpio.h>
}
#endif

#include "ISpiPedal.h"

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
/**
 * @brief Pin status for digital input and output:
 * 			- LOW value
 * 			- HIGH value
 */
typedef enum {LOW = 0, HIGH = 1, NOPE =-1 } pinStatus;
//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
/**
 * @brief Input pin enumeration mapping
 */
typedef enum {
	DI_TON	= 1, //	Tasto di accensione
	DI_TOFF	, //	Tasto spegnimento
	DI_PREP	, //	Tasto Preparazione Raggi
	DI_RAD	, //	Tasto Raggi
	DI_PULS	, //	Tasto Pulsata - sviluppi futuri
	DI_FLUO	, //	Tasto Fluoro scopia - sviluppi futuri
	DI_EN_RAD	, //	sicurezza che viene dal campo e garantisce docking ok (tipo porta chiusa, cassetta in sede)
	DI_EMG	, //	Segnale Emergency
	DI_ONE_SH	, //	Tasto One Shot  comando emissione a impulso singolo utilizzata negli archi
	DI_ST_OK	, //	anodo in rotazione: abbassa un pin della scheda starter - vedi IMD china schema
	DI_KVpp	, //	Kv+ per sviluppi futura interfaccia scopia
	DI_KVll	, //	Kv- per sviluppi futura interfaccia scopia
	DI_POTTER2	, //	tecnica-potter 2 in vibrazione: caso “basic generator”
	DI_POTTER1	, //	tecnica-potter 1 in vibrazione: caso “basic generator”
	DI_JP5	, //
	DI_JP6	, //
	DI_CAP_OK	, //	Condensatori ok - solo per nuova scheda condensatori
	DI_FIL_ERR	, //	Filament error, se corrente sale troppo (> 600 mA)
	DI_FIL_OK	, //	scheda funziona e filamento ready - con uscita segnale digitale fil_ok
	DI_KV85	, //	sorveglia che la scarica condensatori non vada sotto 85%
	DI_RAD_FAULT	, //	Tempo di emissione dei raggi superiore al consentito il timer hw scatta e blocca raggi disabilitato nella scopia
	DI_TH_MON	, //	Sicurezza Termica / intrerruttore termico su inverter
	DI_INV_FAULT	, //	Inverter Fault
	DI_CAP_FLT	, //	Fault Banco condensatori  - solo per nuova scheda condensatori
} enumDigitalIn;

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
/**
 * @brief Output pin enumeration mapping
 */
typedef enum {
	DO_K1=1	, //	Fa scattare teleruttore di potenza, Carica Inverter e condensatore, cavallotta le resistenze
	DO_K2	, //	K1 accende la potenza e k2 fa jump a 320V, dopo 20” cavallotta la resistenza per limitare inrush current. 
	DO_LRAD	, //	Aziona Led Emissione Raggi in corso
	DO_BUZ	, //	Buzzer Emissione raggi - Esiste anche in HW selection tipo oscillatore e collegato a comando raggi
	DO_ST1	, //	Starter interface 1 grafia [ st1 e st2 per accensione anodo rotante a 3000 o 9000 rpm ]
	DO_ST2	, //	Starter interface 2 scopia
	DO_Z2	, //	Zoom2 IB Uscita digitale a piacere
	DO_Z1	, //	Zoom1 IB Uscite digitali a piacere
	DO_COLL	, //	Abilita luce collimatore
	DO_LAS	, //	Abilita laser del collimatore
	DO_CAP_ON	, //	Carica condensatori: utlizzata dove esiste una scheda di carica condensatori intelligente e/o per accendere la carica dei condensatori o per commutare da bassa tensione di carica a alta tensione.
	DO_CAP_EN	, //	Abilitazione Condesatori -  per scheda intelligente
	DO_A1	, //	n.u.
	DO_A2	, //	n.u.
	DO_POTTER2	, //	avvia vibrazione potter 1
	DO_POTTER1	, //	avvia vibrazione potter 2
	DO_A6	, //	n.u.
	DO_RAD_MODE	, //	Inverter in Rad Mode in certi inverteer serve per selezionare modalita scopia modalita grafia ( commuta segnale di feedback della corrente anodica )
	DO_F_SHD	, //	Filament Shut down, se la scheda lavora a 24 V e abilita swith che deve spegnere filamento in caso di errore
	DO_FL	, //	LOW = Filamento piccolo, HIGH = filamento grande
	DO_A3	, //	n.u.
	DO_A4	, //	n.u.
	DO_A5	, //	n.u.
	DO_FLUO	, //	Se alto attiva Fluoroscopia
}enumDigitalOut;

typedef enum {
	DO_COMRAD	= GPIO_Pin_9	,
	DO_COMPREP	= GPIO_Pin_10	,
	DO_LED_GN	= GPIO_Pin_11	,
	DO_LED_RD	= GPIO_Pin_12	,
	TOGLE_WD	= GPIO_Pin_14	,
}enumDirectOut;
//-----------------------------------------------------------------------------

class TDigitalPort {
private:
	ISpiPedal * pSpiPedalIn;
	ISpiPedal * pSpiPedalOut;
	unsigned long
	ulInputWord,//  Use TSpi words instead
	ulOutputWord;//  Use TSpi words instead
	unsigned short
	u16ArrTx[3],
	u16ArrRx[3];
	bool bDirectOut_initialized;
public:
	TDigitalPort( ISpiPedal *pSpiPedalIn = &spiPedal,
			ISpiPedal *spiLineOUT = &spiPedal);
	~TDigitalPort();
	void init(){ pSpiPedalIn->init(); pSpiPedalOut->init(); }
	uint32_t updateIN(void);
    void updateIN(enumDigitalIn inputPortNum, pinStatus status)
    {
        if (status == HIGH)
            ulInputWord |= (1 << inputPortNum);
        else
            ulInputWord &= ~(1 << inputPortNum);
    }
    char updateOUT(void);
	char updateOUT(long lloutWord);
	char setNow(enumDigitalOut outputPortNum);
	char resetNow(enumDigitalOut outputPortNum);
	char setNow(enumDirectOut outputPortNum);
	char resetNow(enumDirectOut outputPortNum);
	pinStatus check(enumDigitalIn inputPortNum);
	pinStatus check(enumDigitalOut inputPortNum);
	pinStatus check(enumDirectOut inputPortNum);
	void set(enumDigitalOut outputPortNum);
	void reset(enumDigitalOut outputPortNum);
	void assign(enumDigitalOut outputPortNum, pinStatus lowOrHigh);
	void resetAllOUT(void);
private:
	void initDirectOut(void);
	char writeOnSPI(unsigned long ulTxSPIword);
	char readFromSPI(unsigned long * pulRxSPIword);
	//	void operator >> (enumDigitalOut outputPortNum, pinStatus status) { outputPortNum};
	//	inline bool operator> (const int& lhs, const int& rhs) {};
	//	char operator [] (enumDigitalOut inputPortNum)	{return (	check(inputPortNum)	);  };
};

extern TDigitalPort digitalPort;

#endif /*!< TDIGITALPORT_H_ */
