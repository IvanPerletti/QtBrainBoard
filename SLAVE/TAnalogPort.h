
#ifndef TANALOGPORT_H
#define TANALOGPORT_H
//	PA0		ADC123_IN0
//	PA3		ADC123_IN3
//	PA4		ADC123_IN4
//	PA5		ADC12_IN5
//	PA6		ADC12_IN6		DacFil
//	PB0		ADC123_IN8
//	PB1		ADC123_IN9 		Dac kV
//	PC0		ADC123_IN10		Multiplexer
// Shared Definitions --------------------------------------------------------
typedef enum {
	AN_24V       	= 0  /* Alimentazione +24V             */ ,
	AN_RAD       	= 1  /* Filamento rad (mA 1V@50mA)     */ ,
	AN_FLUO      	= 2  /* Filamento scopia (mA 1V@1mA)   */ ,
	AN_VCAP      	= 3  /* Feedback Carica condensatori   */ ,
	AN_SET_FIL   	= 4  /* FeedBack Fil                   */ ,
	AN_2V5       	= 5  /* Alimentazione +2,5V            */ ,
	AN_SET_KV    	= 6  /* FeedBack kV                    */ ,
	AN_SW        	= 7  /* Multiplexer Address PC0*       */ ,
	AN_GND       	= 8  /* GND singal                     */ ,
	AN_AEC2      	= 9  /* Feedback Camera AEC2           */ ,
	AN_5V        	= 10 /* Alimentazione +5V              */ ,
	AN_AEC1      	= 11 /* Feedback Camera AEC1           */ ,
	AN_p15V      	= 12 /* Alimentazione +15              */ ,
	AN_NTC1      	= 13 /* Negative Termocouple           */ ,
	AN_m15V      	= 14 /* Alimentazione -15V             */ ,
	AN_NTC2      	= 15 /* Negative Termocouple           */ ,
	ANALOG_NUMEL 	,// Number of different analog channels */ ,
	AN_AUX1      	= AN_2V5,
	AN_AUX2      	= AN_5V

} enumAnalogPort; // please modify TAnalogPort.read( )too

#define IS_ANALOG_PORT(X)	(( X < ANALOG_NUMEL ))

class TAnalogPort {
private:
	int kk;
	volatile unsigned short  u16aAdcValue[ANALOG_NUMEL]; /*! array with ADC values*/
	bool bIsOpen;
public:
	TAnalogPort(void);
	~TAnalogPort(void);
    long read ( enumAnalogPort portNum );
#ifndef STM32F4XX
    void write( enumAnalogPort portNum, long val) { u16aAdcValue[portNum] = val; }
#endif
	void deInit(void);
	void open(void);
	void close(void);
	bool isOpen(void);
private:
	void init(void);
	void configDMA(void);
	void configGpioForADC(void);
	void configADC(void);
	void configMultPlx(void);
	void selectPlexer(short s16PlxSelect);
	long plexerAdcRead(void);
};

extern TAnalogPort tAnalogPort;
#endif /*!< TANALOGPORT_H */
