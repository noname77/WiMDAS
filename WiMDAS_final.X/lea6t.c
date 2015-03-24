#include <xc.h>
#include <i2c.h>
#include "lea6t.h"
#include "rtc.h"

// ubx protocol parser state machine
#define	UBXSTATE_IDLE       0
#define	UBXSTATE_SYNC1      1
#define	UBXSTATE_SYNC2      2
#define	UBXSTATE_CLASS_TIM  3
#define	UBXSTATE_LEN1       4
#define	UBXSTATE_LEN2       5
#define UBXSTATE_DATA       6
#define	UBXSTATE_CKA        7
#define	UBXSTATE_CKB        8

#define UBX_ID_SVIN     0x04
#define UBX_ID_TM2      0x03
#define UBX_ID_TP       0x01

#define	UBX_ID_POSLLH	0x02
#define UBX_ID_SOL	0x06
#define	UBX_ID_VELNED	0x12

#define	UBX_SYNC1_CHAR	0xB5
#define	UBX_SYNC2_CHAR	0x62



UBX_TP_t        UbxTP     = {0,0,0,0,0,0, INVALID};
UBX_SOL_t       UbxSol	  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, INVALID};
UBX_POSLLH_t    UbxPosLlh = {0,0,0,0,0,0,0, INVALID};
UBX_VELNED_t    UbxVelNed = {0,0,0,0,0,0,0,0,0, INVALID};
GPS_INFO_t      GPSInfo   = {0,0,0,0,0,0,0,0,0,0, INVALID};

volatile uint8_t GPSTimeout = 0;


#define LEA6T_I2C_ADDRESS_READ  0x85
#define LEA6T_I2C_ADDRESS_WRITE 0x84

#define LEA6T_REG_ADDR          0xFD   //TODO: should this be 0xFC ?
#define BUF_SIZE 512
unsigned char UBX_buf[BUF_SIZE];

// start survey in with min time 120 s and accuracy 1 m
unsigned char UBX_MSG_SVIN_START[] = {0xB5, 0x62, 0x06, 0x3D, 0x1C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x02, 0xB1, 0x17, 0xFF, 0xA3,
0xEA, 0xFF, 0x50, 0x09, 0xA2, 0x1D, 0xAE, 0x2F, 0x15, 0x00, 0x78, 0x00, 0x00, 0x00, 0xE8, 0x03,
0x00, 0x00, 0x72, 0xFA};

unsigned char UBX_MSG_TP5_POLL[] = {0xB5, 0x62, 0x06, 0x31, 0x01, 0x00, 0x00, 0x38, 0xE5};
// this will probably useful. c++ code for ubx protocol (for neo6m) https://github.com/cTn-dev/Phoenix-FlightController/blob/master/libraries/P_GPS/GPS_ublox.h

// protocol info in this file http://www.kayraelektronik.com/download/gps-moduller/LEA/u-blox6_ReceiverDescriptionProtocolSpec_(GPS.G6-SW-10018).pdf
// timing modes on p. 36

// about u-center config: https://code.google.com/p/ardupirates/wiki/UbloxGPSTutorial

// from https://www.u-blox.com/images/downloads/Product_Docs/u-blox6-FW7.03_ReleaseNote_(GPS.G6-SW-11013).pdf
/*
3.15 Timing Survey-in Mode: Incorrect time accuracy estimation of Fixed Positions
When Timing Receivers are in Survey-in mode the time accuracy estimate (when a Position is fixed) is incorrect.
The estimate is the square of what should normally be estimated. (e.g. 100 m instead of 10 m).
Although the time base and performance of the Timepulse signal is not affected, this limitation can affect
migration from earlier timing product versions where timing applications rely on an estimation of the maximum
timing accuracy. As the Timing accuracy estimate is worse than it is in reality, customers may see more outages
during poor signal conditions than what would otherwise be expected (i.e. the estimated time accuracy exceeds
the configured time accuracy mask).
To avoid this, the following workaround can be implemented:
In Time Mode use Survey-in until the position is determined (when switching to Fixed Mode). Once in Fixed
Mode read the fixed position using TMODE2, calculate the square root of the estimated accuracy and return
this value to the Timing Receiver. The time accuracy estimate will then be correct on the following epochs
and as long as stored/maintained in memory.
*/


// initialize lea6t gps
void lea6t_init(void)
{
    //printf("Initializing GPS...\n\r");
    // reset opendrained
    TRISDbits.TRISD11 = 1;
    // check if i2c comm works

    // at the moment the configuration is done through u-center

    // timing:
    // configure timepulses (CFG-TP5 , CFG-TP)
    // get information about the next timepulse (UBX-TIM-TP)

    // if we have stored data about the position, time, or received satelites info through radio supply it with UBX-AID-INI mesasge
    // initialize survey in: set minimum observation time, set required position standard deviation
    // wait until survey in finished (determined by UBX-TIM-SVIN)

}

int ubx_read()
{
    unsigned char bytesToRead[2] = {0,0};

    IdleI2C1();              //wait for the I2C to be Idle
    StartI2C1();
    while(I2C1CONbits.SEN);      //Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag

    MasterWriteI2C1(LEA6T_I2C_ADDRESS_WRITE);   // Write Slave address and set master for reception
    while(I2C1STATbits.TBF);                    // 8 clock cycles
    while(!IFS1bits.MI2C1IF);                   // Wait for ninth clock cycle
    while(I2C1STATbits.ACKSTAT);                // assumes everything went ok, i think?
    MI2C1_Clear_Intr_Status_Bit;                // Clear interrupt flag

    // should i do a start here again?
    //StartI2C1();
    MasterWriteI2C1(LEA6T_REG_ADDR);            // Write register address
    while(I2C1STATbits.TBF);                    // 8 clock cycles
    while(!IFS1bits.MI2C1IF);                   // Wait for ninth clock cycle
    while(I2C1STATbits.ACKSTAT);                // assumes everything went ok, i think?
    MI2C1_Clear_Intr_Status_Bit;                // Clear interrupt flag

    StopI2C1();              //Terminate communication protocol with stop signal
    IdleI2C1();              //wait for the I2C to be Idle

    StartI2C1();
    while(I2C1CONbits.SEN );  //Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag

    MasterWriteI2C1(LEA6T_I2C_ADDRESS_READ);        //Write Slave address and set master for reception
    while(!IFS1bits.MI2C1IF);    //Wait for ninth clock cycle
    while(I2C1STATbits.ACKSTAT);
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag

    MastergetsI2C1(2,bytesToRead,1000); // read two bytes that contain the number of bytes in the buffer available to read
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag
    IdleI2C1();              //wait for the I2C to be Idle
    StopI2C1();              //Terminate communication protocol with stop signal
    IdleI2C1();              //wait for the I2C to be Idle

    unsigned int len = (bytesToRead[0] << 8) + bytesToRead[1];

    // if nothings there, return
    if (len == 0) return 0;
    else if (len > BUF_SIZE) len = BUF_SIZE;
    // else read the messages

    StartI2C1();
    while(I2C1CONbits.SEN);      //Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag

    MasterWriteI2C1(LEA6T_I2C_ADDRESS_READ);        //Write Slave address and set master for reception
    while(!IFS1bits.MI2C1IF);    //Wait for ninth clock cycle
    while(I2C1STATbits.ACKSTAT);
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag

    MastergetsI2C1(len,UBX_buf,1000); //Master recieves from Slave upto len bytes
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag
    StopI2C1();              //Terminate communication protocol with stop signal
    IdleI2C1();              //wait for the I2C to be Idle

    return len;
}

void ubx_write(unsigned char* message, unsigned int len)
{
    if (len < 2) return; // we have to write at least 2 bytes to make the transfer valid (writing 1 would just set the register address)

    unsigned int i;
    unsigned char* p = message;

    StartI2C1();
    while(I2C1CONbits.SEN);      //Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag

    MasterWriteI2C1(LEA6T_I2C_ADDRESS_WRITE);   // Write Slave address and set master for reception
    while(I2C1STATbits.TBF);                    // 8 clock cycles
    while(!IFS1bits.MI2C1IF);                   // Wait for ninth clock cycle
    while(I2C1STATbits.ACKSTAT);                // assumes everything went ok, i think?
    MI2C1_Clear_Intr_Status_Bit;                // Clear interrupt flag

    for (i=0; i<len; i++)
    {
        MasterWriteI2C1(*p);         // Write data
        while(!IFS1bits.MI2C1IF);    // Wait for ninth clock cycle
        while(I2C1STATbits.ACKSTAT);
        MI2C1_Clear_Intr_Status_Bit; // Clear interrupt flag
    }

    StopI2C1();              //Terminate communication protocol with stop signal
    IdleI2C1();              //wait for the I2C to be Idle
}


// ubx parser adapted from: http://svn.mikrokopter.de/mikrosvn/FlightCtrl/branches/V0.70d%20CRK%20HexaLotte/ubx.c

void UpdateGPSInfo (void)
{

	if ((UbxSol.Status == NEWDATA) && (UbxPosLlh.Status == NEWDATA) && (UbxVelNed.Status == NEWDATA))
	{
		if(GPSInfo.status != NEWDATA)
		{
			GPSInfo.status = INVALID;
			// NAV SOL
			GPSInfo.flags = UbxSol.Flags;
			GPSInfo.satfix = UbxSol.GPSfix;
			GPSInfo.satnum = UbxSol.numSV;
			GPSInfo.PAcc = UbxSol.PAcc;
			GPSInfo.VAcc = UbxSol.SAcc;
			// NAV POSLLH
			GPSInfo.longitude = UbxPosLlh.LON;
			GPSInfo.latitude = UbxPosLlh.LAT;
			GPSInfo.altitude = UbxPosLlh.HEIGHT;

			GPSInfo.veleast = UbxVelNed.VEL_E;
			GPSInfo.velnorth = UbxVelNed.VEL_N;
			GPSInfo.veltop = -UbxVelNed.VEL_D;
			GPSInfo.velground = UbxVelNed.GSpeed;

			GPSInfo.status = NEWDATA;

		}
		// set state to collect new data
		UbxSol.Status = PROCESSED;			// never update old data
		UbxPosLlh.Status = PROCESSED;		// never update old data
		UbxVelNed.Status = PROCESSED;		// never update old data
	}


}


// this function should be called within the UART RX ISR
void ubx_parser(uint8_t c)
{
	static uint8_t ubxstate = UBXSTATE_IDLE;
	static uint8_t cka, ckb;
	static uint16_t msglen;
	static int8_t *ubxP, *ubxEp, *ubxSp; // pointers to data currently transfered

	switch(ubxstate)
	{
		case UBXSTATE_IDLE: // check 1st sync byte
			if (c == UBX_SYNC1_CHAR) ubxstate = UBXSTATE_SYNC1;
			else ubxstate = UBXSTATE_IDLE; // out of synchronization
			break;

		case UBXSTATE_SYNC1: // check 2nd sync byte
			if (c == UBX_SYNC2_CHAR) ubxstate = UBXSTATE_SYNC2;
			else ubxstate = UBXSTATE_IDLE; // out of synchronization
			break;

		case UBXSTATE_SYNC2: // check msg class to be NAV
                        switch(c)
                        {
                            case UBX_CLASS_TIM:
                                ubxstate = UBXSTATE_CLASS_TIM;
                                break;
                            default:
				ubxstate = UBXSTATE_IDLE;
				break;
                        }
			break;

		case UBXSTATE_CLASS_TIM: // check message identifier
			switch(c)
			{
				case UBX_ID_TP: // timepulse data
					ubxP =  (int8_t *)&UbxTP; // data start pointer
					ubxEp = (int8_t *)(&UbxTP + 1); // data end pointer
					ubxSp = (int8_t *)&UbxTP.Status; // status pointer
					break;

				default:			// unsupported identifier
					ubxstate = UBXSTATE_IDLE;
					break;
			}
			if (ubxstate != UBXSTATE_IDLE)
			{
				ubxstate = UBXSTATE_LEN1;
				cka = UBX_CLASS_TIM + c;
				ckb = UBX_CLASS_TIM + cka;
			}
			break;

		case UBXSTATE_LEN1: // 1st message length byte
			msglen = c;
			cka += c;
			ckb += cka;
			ubxstate = UBXSTATE_LEN2;
			break;

		case UBXSTATE_LEN2: // 2nd message length byte
			msglen += ((uint16_t)c)<<8;
			cka += c;
			ckb += cka;
			// if the old data are not processed so far then break parsing now
			// to avoid writing new data in ISR during reading by another function
			if ( *ubxSp == NEWDATA )
			{
				UpdateGPSInfo(); //update GPS info respectively
                                gps_getTime(&UbxTP);
				ubxstate = UBXSTATE_IDLE;
			}
			else // data invalid or allready processd
			{
				*ubxSp = INVALID;
				ubxstate = UBXSTATE_DATA;
			}
			break;

		case UBXSTATE_DATA:
			if (ubxP < ubxEp) *ubxP++ = c; // copy curent data byte if any space is left
			cka += c;
			ckb += cka;
			if (--msglen == 0) 	ubxstate = UBXSTATE_CKA; // switch to next state if all data was read
			break;

		case UBXSTATE_CKA:
			if (c == cka) ubxstate = UBXSTATE_CKB;
			else
			{
				*ubxSp = INVALID;
				ubxstate = UBXSTATE_IDLE;
			}
			break;

		case UBXSTATE_CKB:
			if (c == ckb)
			{
				*ubxSp = NEWDATA; // new data are valid
                                gps_getTime(&UbxTP);
				//UpdateGPSInfo(); //update GPS info respectively
				GPSTimeout = 255;
			}
			else
			{	// if checksum not fit then set data invalid
				*ubxSp = INVALID;
			}
			ubxstate = UBXSTATE_IDLE; // ready to parse new data
			break;

		default: // unknown ubx state
			ubxstate = UBXSTATE_IDLE;
			break;
	}

}

BYTE toBCD(unsigned char val)
{
    BYTE bcd;
    bcd = (val / 10) << 4;
    bcd = bcd | (val % 10);
    return (bcd);
}

// adapted from http://www.novatel.com/support/knowledge-and-learning/published-papers-and-documents/unit-conversions/
// no leap seconds yet
// TODO: need to make sure its synchronized to the timepulse (is not atm)
void gps_getTime(UBX_TP_t* pUbxTP)
{
    unsigned char sec,min,hour;
    rtccTime new_time;
    uint32_t tow = pUbxTP->towMS / 1000;
    hour = (unsigned char)((tow%86400)/3600);
    min = (unsigned char)(((tow%86400)%3600)/60);
    sec = (unsigned char)(((tow%86400)%3600)%60);
    // need to converrt these to bcd
    new_time.f.hour = toBCD(hour);
    new_time.f.min = toBCD(min);
    new_time.f.sec = toBCD(sec);
    pUbxTP->Status = INVALID;
    // update the rtc only if the time is off by 2 s
    if(new_time.f.sec - RtccTime.f.sec > 2)
        initRtcc(&RtccTime, &new_time);
}

void GPS_Tasks()
{
    ubx_write(UBX_MSG_TP5_POLL, sizeof(UBX_MSG_TP5_POLL));  // should probably do something more elaborate to ask for data
    static unsigned int len = 0;
    static unsigned int i = 0;
    if(len == 0)
    {
        len = ubx_read();
        i = 0;
    }
    else
    {
        while(len)
        {
            ubx_parser(UBX_buf[i++]);
            len--;
        }
    }
}