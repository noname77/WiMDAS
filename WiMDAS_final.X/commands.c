#include <xc.h>
#include "USB/usb.h"
#include "USB/usb_device_cdc.h"
#include "USB/usb_my.h"
#include "rtc.h"
#include "commands.h"
#include "ADC_pipeline.h"
#include "tinysh.h"

uint8_t write_index;

void tinysh_char_out(unsigned char c)
{
    if(!ADC_finished)
    {
        if (write_index == 0)
        {
            memset(writeBuffer, '\0', sizeof(writeBuffer));
        }
        writeBuffer[write_index++] = c;
        if(write_index == sizeof(writeBuffer))
        {
            write_index = 0;
        }
    }
}

void test_fnt (int argc, char **argv)
{
    uint8_t buf[] = "tested";
    int i;
    for(i=0; i<strlen(buf); i++)
    {
        writeBuffer[write_index++] = buf[i];
    }
}

void ADC_start()
{
    ADC_enable = 1;
}

tinysh_cmd_t testcmd = {0,"test","test command","[args]", test_fnt,0,0,0};
tinysh_cmd_t rtcrcmd = {0, "rtcr", "reset rtc", "[args]", RTCC_reset,0,0,0};
tinysh_cmd_t adcscmd = {0, "adcs", "start adc", "[args]", ADC_start, 0,0,0};


void register_commands()
{
    tinysh_add_command(&testcmd);
    tinysh_add_command(&rtcrcmd);
    tinysh_add_command(&adcscmd);
}
