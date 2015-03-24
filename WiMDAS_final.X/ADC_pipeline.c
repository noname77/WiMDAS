// taken from http://www.microchip.com/forums/FindPost/823368
// this might be helpful about high speed setup: http://www.microchip.com/forums/m749312.aspx
// ADC Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/30686a.pdf

#include <xc.h>

#include "ADC_Pipeline.h"
#include "USB/usb.h"
#include "USB/usb_my.h"
#include "USB/usb_device_cdc.h"
#include "xprintf.h"
#include "rtc.h"

// this will help with adc dma: http://esca.atomki.hu/PIC24/code_examples/docs/manuallyCreated/Appendix_H_ADC_with_DMA.pdf
// TODO: setup for automatic sdampling
// conversion order mode (extended buffer mode)

unsigned int offset;
unsigned int ADC_enable;
unsigned int ADC_finished;
unsigned int ADCdata_p0[1024] __at(0x1800);
unsigned int ADCdata_p1[1024] __at(0x2000);

void ADC_Pipeline_Init(unsigned int ADCReferences, unsigned int ADCFormat)
{
    unsigned int IPLSave;
    PMD1bits.ADC1MD = 0; //Clear PMD bit to allow ADC operation
    ADCON1bits.ADON = 0; //Disable ADC, if it was previously on
    ADCON3 = 0x0002; //ADC clock is instruction clock derived (TAD = 4*TCY - adc running at 4 MHz)
    ADCON2 = 0x0300 | ADCReferences; //Select the ADC references, buffer mode (fifo), powered down in sleep and idle
    ADCON1 = 0x0001 | ADCFormat; //full power + Select ADC result formatting and other settings.
    ADCON1bits.ADON = 1;


    while(ADSTATHbits.ADREADY == 0);//Wait for hardware self calibration to complete

    IPLSave = SR & 0x00E0; //Apply A0 silicon workaround, to ensure self calibration gets loaded internally
    //Save interrupt settings
    SRbits.IPL = 7; //Set CPU to max priority, so as to disable interrupts for
    //below workaround code (which must not be interrupted to
    //ensure it works correctly).
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;
    ADCON1bits.ADCAL = 1;

    SR = (SR & 0xFF1F) | IPLSave; //Okay to restore previous interrupt settings now

    while(ADSTATHbits.ADREADY == 0); //Wait for hardware self calibration to complete

    IFS0bits.AD1IF = 0; // clear ADC interrupt flag
    IPC3bits.AD1IP = 7; // adc has highest priority
    IEC0bits.AD1IE = 1; // enable adc interrupts

    //The ADC is now ready to use. Set up a sample list and generate a trigger event to get result data.
}

void ADC_Pipeline_Disable(void)
{
     PMD1bits.ADC1MD = 0; //Disable the ADC module and configure for minimum current consumption
     ADCON1bits.ADON = 0; //Disable ADC module
     ADCON2 = 0xC000;     //Reserved setting: A0 workaround to minimize IPD current.
}

void ADC_Pipeline_configure_samplelist()
{
    // clear the registers
    ADL0CONH = 0;
    ADL0CONL = 0;

    ADL0CONHbits.ASEN = 1;      // Auto Scan Enabled
    //ADL0CONHbits.ASEN = 0;      // Sequential mode
    ADL0CONHbits.SLINT = 1;     // Interrupt after auto scan completion
    //ADL0CONHbits.SLINT = 2;     // Interrupt after all samples in the list converted
    ADL0CONHbits.WM = 0;        // All results saved to ADRES buffer
    ADL0CONHbits.CM = 0;        // Matching disabled
    ADL0CONHbits.CTMEN = 0;     // CTMU disabled
    ADL0CONHbits.MULCHEN = 0;   // Multiple chanel disabled
    ADL0CONHbits.SAMC = 0;      // S/H cap charge time = 0.5 TAD

    //ADL0CONLbits.SLTSRC = 7;    // sample list triggered by internal timer
    ADL0CONLbits.SLTSRC = 2;    // manual trigger - trig on every adc clock

    ADL0CONLbits.SLSIZE = LIST_SIZE;  // using 31 now
    //ADL0CONLbits.SLSIZE = 15;   // 16 items in the list to fill half the buffer
    //ADL0CONLbits.SLSIZE = 31;   // 32 items in the list to fill full buffer

    ADL0PTR = 0; // Point to start of sample list 0

    // doing this so that I can generate an interrupt when the buffer is half full / full
    /*
    ADTBL0 = 12; // AN12
    ADTBL1 = 12; // AN12   | 14-E

    ADTBL2 = 12;
    ADTBL3 = 12;
    ADTBL4 = 12;
    ADTBL5 = 12;
    ADTBL6 = 12;
    ADTBL7 = 12;
    ADTBL8 = 12;
    ADTBL9 = 12;
    ADTBL10 = 12;
    ADTBL11 = 12;
    ADTBL12 = 12;
    ADTBL13 = 12;
    ADTBL14 = 12;
    ADTBL15 = 12;

    ADTBL16 = 12;
    ADTBL17 = 12;
    ADTBL18 = 12;
    ADTBL19 = 12;
    ADTBL20 = 12;
    ADTBL21 = 12;
    ADTBL22 = 12;
    ADTBL23 = 12;
    ADTBL24 = 12;
    ADTBL25 = 12;
    ADTBL26 = 12;
    ADTBL27 = 12;
    ADTBL28 = 12;
    ADTBL29 = 12;
    ADTBL30 = 12;
    ADTBL31 = 12;
    */
   
    ADTBL0 = 12; // AN12
    ADTBL1 = 13; // AN13   | 14-E

    ADTBL2 = 12;
    ADTBL3 = 13;
    ADTBL4 = 12;
    ADTBL5 = 13;
    ADTBL6 = 12;
    ADTBL7 = 13;
    ADTBL8 = 12;
    ADTBL9 = 13;
    ADTBL10 = 12;
    ADTBL11 = 13;
    ADTBL12 = 12;
    ADTBL13 = 13;
    ADTBL14 = 12;
    ADTBL15 = 13;

    ADTBL16 = 12;
    ADTBL17 = 13;
    ADTBL18 = 12;
    ADTBL19 = 13;
    ADTBL20 = 12;
    ADTBL21 = 13;
    ADTBL22 = 12;
    ADTBL23 = 13;
    ADTBL24 = 12;
    ADTBL25 = 13;
    ADTBL26 = 12;
    ADTBL27 = 13;
    ADTBL28 = 12;
    ADTBL29 = 13;
    ADTBL30 = 12;
    ADTBL31 = 13;
  
    ACCONH = 0; // No accumulation
    // not using this in manual trigger mode
    ADTMRPR = 0; // 16 - period for the trigger timer, increments once every TAD, have to be long enough to allow all entries in the sample list to be converted. each conversion should take 1 tad times 16 channels
    //ADTMRPR = 16; // 16 - period for the trigger timer, increments once every TAD, have to be long enough to allow all entries in the sample list to be converted. each conversion should take 1 tad times 16 channels
    //ADTMRPR = 32; // 32 - period for the trigger timer, increments once every TAD, have to be long enough to allow all entries in the sample list to be converted. each conversion should take 1 tad times 32 channels
}


void ADC_Tasks()
{
    if(ADC_finished)
    {
        rtc_print_enable = 0;
        static unsigned int off = 0;

        if (off == 2048)
        {
            off = 0;
            ADC_finished = 0;
            rtc_print_enable = 1;
            //ADC_Pipeline_Start();
            return;
        }

        if( USBUSARTIsTxTrfReady() == true)
        {
            int i;
            for(i=0; i<64; i+=8)
            {
                xsprintf((char*)&writeBuffer[i], "%04u  \n\r",ADCdata_p0[off+(i>>3)]);  // this should be >>3, but doesnt work ;/
                //xsprintf(&writeBuffer[i],(uint8_t*)" %04u \n\r",off+(i>>3));
            }
            //putUSBUSART((uint8_t*)buf,64);
            off += 8;
        }
    }
}