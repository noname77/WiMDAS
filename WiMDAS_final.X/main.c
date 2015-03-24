/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#include <xc.h>

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */
#include "HardwareProfile.h"

#include "USB/usb.h"
#include "USB/usb_my.h"
#include "USB/usb_device_cdc.h"

#include "ADC_pipeline.h"
#include "lea6t.h"
#include "rtc.h"
#include "commands.h"
#include "xprintf.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
int16_t main(void)
{
    // Configure the oscillator for the device
    ConfigureOscillator();

    // Initialize IO ports and peripherals
    InitApp();
    
    while(1)
    {
        // User Application USB tasks
        if(USB_BUS_SENSE && (USBGetDeviceState() == DETACHED_STATE))
            USBDeviceAttach();
        else if((USB_BUS_SENSE != 1) && (USBGetDeviceState() != DETACHED_STATE))
            USBDeviceDetach();

        if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1))
        {
            // we cant talk to usb, do appropierate code
            //int test = ubx_read();
        }
        else
        {
            // we are connected to usb, do appropierate code
            RTCC_Tasks(); // maintain RTC and printout current time to serial
            GPS_Tasks();  // decode messages and update rtc
            ADC_Tasks();  // print out adc pages 0 and 1 to serial
            CDC_Tasks();  // perform read and write to usb com port
            // TODO: if in ublox pass through mode, follow the received data onto i2c and send back the reply
        }

        // if connected to usb start data transfer (device info, boot config, status)

        // wait for radio message start capture
        // capture ADC
        // move data to ram using dma when registers full
        // move dma to another page when whole page of data is available
        // move data from ram to flash

        // when ADC sampling finished move contents form flash to sd card

    }
}
