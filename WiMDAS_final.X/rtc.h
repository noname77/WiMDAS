#ifndef RTC_H
#define	RTC_H

#include <Rtcc.h>

void RTCC_Tasks();
void initRtcc(rtccTime *pRtccTime, rtccTime *pRtccTimeSet);
void printTime(rtccTime *timstruct);
void RTCC_reset();
void printTime(rtccTime *ts);

extern int rtcc_enable;
extern int rtc_print_enable;
extern rtccTime RtccTime;

#endif	/* RTC_H */

