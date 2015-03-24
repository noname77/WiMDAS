// adapted from https://github.com/cTn-dev/Phoenix-FlightController/blob/master/libraries/P_GPS/GPS_ublox.h
// and from here: http://people.openmoko.org/tony_tu/src/u-blox/AssistNowOfflineServerSampleImplementation/ubx.h
// big thanks for the authors!

#ifndef LEA6T_H
#define	LEA6T_H

#include <stdint.h>

#define BUF_SIZE 512

extern unsigned char UBX_MSG_SVIN_START[36];
extern unsigned char UBX_MSG_TP5_POLL[9];
extern unsigned char UBX_buf[BUF_SIZE];

typedef enum
{
	INVALID,
	NEWDATA,
	PROCESSED
} Status_t;

typedef struct {
	uint32_t		towMS;		// ms Timepulse time of week according to time base
	uint32_t		towSubMS;	// 2^-32 ms Submillisecond part of TOWMS
	int32_t			qErr;		// ps Quantization error of timepulse.
	uint16_t		week;           // weeks Timepulse week number according to time base
	uint8_t			flags;		// mm Height above mean sea level
        uint8_t                  res1;		// reserved
	Status_t		Status;
} UBX_TP_t;

typedef struct {
	uint32_t		ITOW;		// ms GPS Millisecond Time of Week
	int32_t			Frac;		// ns remainder of rounded ms above
	int16_t			week;		// GPS week
	uint8_t			GPSfix;		// GPSfix Type, range 0..6
	uint8_t			Flags;		// Navigation Status Flags
	int32_t			ECEF_X;		// cm ECEF X coordinate
	int32_t			ECEF_Y;		// cm ECEF Y coordinate
	int32_t			ECEF_Z;		// cm ECEF Z coordinate
	uint32_t		PAcc;		// cm 3D Position Accuracy Estimate
	int32_t			ECEFVX;		// cm/s ECEF X velocity
	int32_t			ECEFVY;		// cm/s ECEF Y velocity
	int32_t			ECEFVZ;		// cm/s ECEF Z velocity
	uint32_t		SAcc;		// cm/s Speed Accuracy Estimate
	uint16_t		PDOP;		// 0.01 Position DOP
	uint8_t			res1;		// reserved
	uint8_t			numSV;		// Number of SVs used in navigation solution
	uint32_t		res2;		// reserved
	Status_t		Status;
} UBX_SOL_t;

typedef struct {
	uint32_t		ITOW;		// ms GPS Millisecond Time of Week
	int32_t			LON;		// 1e-07 deg Longitude
	int32_t			LAT;		// 1e-07 deg Latitude
	int32_t			HEIGHT;		// mm Height above Ellipsoid
	int32_t			HMSL;		// mm Height above mean sea level
	uint32_t		Hacc;		// mm Horizontal Accuracy Estimate
	uint32_t		Vacc;		// mm Vertical Accuracy Estimate
	Status_t		Status;
} UBX_POSLLH_t;

typedef struct {
	uint32_t		ITOW;  		// ms  GPS Millisecond Time of Week
	int32_t			VEL_N; 		// cm/s  NED north velocity
	int32_t			VEL_E; 		// cm/s  NED east velocity
	int32_t			VEL_D; 		// cm/s  NED down velocity
	uint32_t		Speed; 		// cm/s  Speed (3-D)
	uint32_t		GSpeed; 	// cm/s  Ground Speed (2-D)
	int32_t			Heading; 	// 1e-05 deg  Heading 2-D
	uint32_t		SAcc;		// cm/s  Speed Accuracy Estimate
	uint32_t		CAcc; 		// deg  Course / Heading Accuracy Estimate
	Status_t		Status;
} UBX_VELNED_t;

void GPS_Tasks();
void lea6t_init(void);
int ubx_read(void);
void ubx_write(unsigned char* message, unsigned int len);
void gps_getTime(UBX_TP_t* pUbxTP);

#define	UBX_CLASS_NAV 0x01
#define UBX_CLASS_ACK 0x05 // Ack/Nack Messages
#define UBX_CLASS_AID 0x0B // AssistNow Aiding Messages
#define UBX_CLASS_CFG 0x06 // Configuration Input Messages
#define UBX_CLASS_ESF 0x10 // External Sensor Fusion Messages
#define UBX_CLASS_INF 0x04 // Information Messages
#define UBX_CLASS_MON 0x0A // Monitoring Messages
#define UBX_CLASS_RXM 0x02 // Receiver Manager Messages
#define UBX_CLASS_TIM 0x0D // Timing Messages

// UBX Protocol Header
typedef struct UBX_HEADER
{
	unsigned int prefix;	//!< Prefix, always #GPS_UBX_PREFIX
	unsigned char classId;					//!< UBX Class Id
	unsigned char msgId;					//!< UBX Message Id
	unsigned int size;					//!< Payload Size
} UBX_HEAD, *UBX_HEAD_pt;



// Satfix types for GPSData.satfix
#define SATFIX_NONE 			 0x00
#define SATFIX_DEADRECKOING 	 0x01
#define SATFIX_2D 				 0x02
#define SATFIX_3D				 0x03
#define SATFIX_GPS_DEADRECKOING  0x04
#define SATFIX_TIMEONLY			 0x05
// Flags for interpretation of the GPSData.flags
#define FLAG_GPSFIXOK			0x01 // (i.e. within DOP & ACC Masks)
#define FLAG_DIFFSOLN			0x02 // (is DGPS used)
#define FLAG_WKNSET				0x04 // (is Week Number valid)
#define FLAG_TOWSET				0x08 //	(is Time of Week valid)


/* enable the UBX protocol at the gps receiver with the following messages enabled
  01-02 NAV - POSLLH
  01-06 Nav - SOL
  01-12 NAV - VELNED */

typedef struct
{
	uint8_t		flags;		// flags
	uint8_t		satnum;		// number of satelites
	uint8_t 	satfix;		// type of satfix
	int32_t		longitude;  // in 1e-07 deg
	int32_t		latitude;	// in 1e-07 deg
	int32_t		altitude;   // in mm
	uint32_t    PAcc;		// in cm 3d position accuracy
	int32_t		velnorth;	// in cm/s
	int32_t		veleast;	// in cm/s
	int32_t		veltop;	    // in cm/s
	uint32_t    velground;  // 2D ground speed in cm/s
	uint32_t	VAcc;		// in cm/s 3d velocity accuracy
	Status_t	status;	    // status of data: invalid | valid
} GPS_INFO_t;

//here you will find the current gps info
extern GPS_INFO_t GPSInfo;	// measured position (last gps record)

// this variable should be decremted by the application
extern volatile uint8_t GPSTimeout; // is reset to 255 if a new UBX msg was received

// this function should be called within the UART RX ISR
extern void ubx_parser(uint8_t c);



#endif	/* LEA6T_H */

