#include <xc.h>
#include <stdint.h>
#include <Rtcc.h>
#include "GenericTypeDefs.h"
#include "rtc.h"
#include "tinysh.h"
#include "xprintf.h"
#include "lea6t.h"

int rtcc_enable;
int rtc_print_enable;
rtccTime RtccTime;

void initRtcc(rtccTime *pRtccTime, rtccTime *pRtccTimeSet)
{
    RtccWrOn();     // RTCC enable bit can't be changed unless WENA is set
    mRtccOff();      // Turn off the module.
    mRtccWrOff();   // Prevent inadvertant writes to RTCC settings.
    // Start with time zero
    //
    pRtccTime->f.hour = pRtccTimeSet->f.hour;
    pRtccTime->f.min  = pRtccTimeSet->f.min;
    pRtccTime->f.sec  = pRtccTimeSet->f.sec;

    RtccWriteTime(pRtccTime, TRUE);

    // Turn on RTCC clock source (secondary oscillator)
    RtccInitClock();
    // enable secondary oscilator -> RtccInitClock(); doesnt seem to work
     __builtin_write_OSCCONL(OSCCON | 0x02);

    RtccWrOn();     // RTCC enable bit can't be changed unless WENA is set
    mRtccOn();      // Turn on the module.
    mRtccWrOff();   // Prevent inadvertant writes to RTCC settings.

    rtcc_enable = 1;
    rtc_print_enable = 1;
} // End of InitRtcc()

void RTCC_reset()
{
    RtccWrOn();     // RTCC enable bit can't be changed unless WENA is set
    mRtccOff();      // Turn on the module. (need to do this?

    RtccTime.f.hour = 0;
    RtccTime.f.min = 0;
    RtccTime.f.sec = 0;
    RtccWriteTime(&RtccTime, TRUE);
    mRtccOn();      // Turn on the module.
    mRtccWrOff();   // Prevent unwanted writes to RTCC settings.
    uint8_t buf[] = "time reseted\n\r";
    puts(buf);
}


void RTCC_Tasks()
{
    static rtccTime InitialTime;
    static uint8_t old_sec = 0;

    // do this only once
    if(rtcc_enable == 1)
    {
        InitialTime = RtccTime;
        rtcc_enable = 0;
    }

    RtccReadTime(&RtccTime);
    uint8_t s = RtccTime.f.sec;

    if (s != old_sec && rtc_print_enable) {
        
        printTime(&InitialTime);
        //puts((unsigned char*) " ---> ");
        printTime(&RtccTime);
        puts((unsigned char*) "\n\r");
        old_sec = s;
    }
}

void printTime(rtccTime *ts)
{
    static char buf[11];
    xsprintf(buf, "%02x:%02x:%02x\n\r", ts->f.hour, ts->f.min, ts->f.sec);
    int i;
    for (i=0; i<strlen(buf); i++)
        tinysh_char_out(buf[i]);

} // End of printTime()