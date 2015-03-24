#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

/*******************************************************************/
/******** USB stack hardware selection options *********************/
/*******************************************************************/
//This section is the set of definitions required by the MCHPFSUSB
//  framework.  These definitions tell the firmware what mode it is
//  running in, and where it can find the results to some information
//  that the stack needs.
//These definitions are required by every application developed with
//  this revision of the MCHPFSUSB framework.  Please review each
//  option carefully and determine which options are desired/required
//  for your application.

// this thread might be useful if something doesnt work http://www.microchip.com/forums/m495623.aspx
//#define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISAbits.TRISA2    // Input TODO: dont have it hardwired, what should i do?
#define self_power          1

#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISFbits.TRISF3
#define USB_BUS_SENSE       1//PORTFbits.RF3
//#define tris_usb_bus_sense  U1OTGSTATbits.SESVD
//#define USB_BUS_SENSE       U1OTGSTATbits.SESVD

//Uncomment this to make the output HEX of this project
//   to be able to be bootloaded using the HID bootloader
//#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER

//If the application is going to be used with the HID bootloader
//  then this will provide a function for the application to
//  enter the bootloader from the application (optional)
#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
    #define EnterBootloader() __asm__("goto 0x400")
#endif

/*******************************************************************/
/******** MDD File System selection options ************************/
/*******************************************************************/
#define MEDIA_SOFT_DETECT
#define USE_SD_INTERFACE_WITH_SPI
#define USE_PIC24F
#define USE_16BIT

// Sample definitions for 16-bit processors (modify to fit your own project)
#define SD_CS				LATDbits.LATD2
#define SD_CS_TRIS			TRISDbits.TRISD2

//TODO: dont have these, what should i do?
#define SD_CD				PORTDbits.RD1
#define SD_CD_TRIS			TRISDbits.TRISD1

#define SD_WE				PORTDbits.RD1
#define SD_WE_TRIS			TRISDbits.TRISD1

// Registers for the SPI module you want to use
#define SPICON1				SPI2CON1
#define SPISTAT				SPI2STAT
#define SPIBUF				SPI2BUF
#define SPISTAT_RBF			SPI2STATbits.SPIRBF
#define SPICON1bits			SPI2CON1bits
#define SPISTATbits			SPI2STATbits
#define SPIENABLE                       SPI2STATbits.SPIEN

// Tris pins for SCK/SDI/SDO lines
#define SPICLOCK			TRISDbits.TRISD0
#define SPIIN				TRISDbits.TRISD3
#define SPIOUT                          TRISFbits.TRISF4


/** I/O pin definitions ********************************************/
#define INPUT_PIN 1
#define OUTPUT_PIN 0

//TODO: check all! the values
#define CLOCK_FREQ 32000000
#define GetSystemClock() CLOCK_FREQ
#define GetPeripheralClock() CLOCK_FREQ/2
#define GetInstructionClock() CLOCK_FREQ/2
#define BRG_DIV2    4

#endif  //HARDWARE_PROFILE_H