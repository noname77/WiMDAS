/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#include <xc.h>

#include <stdint.h>        /* Includes uint16_t definition */
#include <stdbool.h>       /* Includes true/false definition */

#include "user.h"
#include "ADC_pipeline.h"
/******************************************************************************/
/* Interrupt Vector Options                                                   */
/******************************************************************************/
/*                                                                            */
/* Refer to the C30 (MPLAB C Compiler for PIC24F MCUs and dsPIC33F DSCs) User */
/* Guide for an up to date list of the available interrupt options.           */
/* Alternately these names can be pulled from the device linker scripts.      */
/*                                                                            */
/* PIC24F Primary Interrupt Vector Names:                                     */
/*                                                                            */
/* _INT0Interrupt      _IC4Interrupt                                          */
/* _IC1Interrupt       _IC5Interrupt                                          */
/* _OC1Interrupt       _IC6Interrupt                                          */
/* _T1Interrupt        _OC5Interrupt                                          */
/* _Interrupt4         _OC6Interrupt                                          */
/* _IC2Interrupt       _OC7Interrupt                                          */
/* _OC2Interrupt       _OC8Interrupt                                          */
/* _T2Interrupt        _PMPInterrupt                                          */
/* _T3Interrupt        _SI2C2Interrupt                                        */
/* _SPI1ErrInterrupt   _MI2C2Interrupt                                        */
/* _SPI1Interrupt      _INT3Interrupt                                         */
/* _U1RXInterrupt      _INT4Interrupt                                         */
/* _U1TXInterrupt      _RTCCInterrupt                                         */
/* _ADC1Interrupt      _U1ErrInterrupt                                        */
/* _SI2C1Interrupt     _U2ErrInterrupt                                        */
/* _MI2C1Interrupt     _CRCInterrupt                                          */
/* _CompInterrupt      _LVDInterrupt                                          */
/* _CNInterrupt        _CTMUInterrupt                                         */
/* _INT1Interrupt      _U3ErrInterrupt                                        */
/* _IC7Interrupt       _U3RXInterrupt                                         */
/* _IC8Interrupt       _U3TXInterrupt                                         */
/* _OC3Interrupt       _SI2C3Interrupt                                        */
/* _OC4Interrupt       _MI2C3Interrupt                                        */
/* _T4Interrupt        _U4ErrInterrupt                                        */
/* _T5Interrupt        _U4RXInterrupt                                         */
/* _INT2Interrupt      _U4TXInterrupt                                         */
/* _U2RXInterrupt      _SPI3ErrInterrupt                                      */
/* _U2TXInterrupt      _SPI3Interrupt                                         */
/* _SPI2ErrInterrupt   _OC9Interrupt                                          */
/* _SPI2Interrupt      _IC9Interrupt                                          */
/* _IC3Interrupt                                                              */
/*                                                                            */
/* PIC24H Primary Interrupt Vector Names:                                     */
/*                                                                            */
/* _INT0Interrupt      _SPI2Interrupt                                         */
/* _IC1Interrupt       _C1RxRdyInterrupt                                      */
/* _OC1Interrupt       _C1Interrupt                                           */
/* _T1Interrupt        _DMA3Interrupt                                         */
/* _DMA0Interrupt      _IC3Interrupt                                          */
/* _IC2Interrupt       _IC4Interrupt                                          */
/* _OC2Interrupt       _IC5Interrupt                                          */
/* _T2Interrupt        _IC6Interrupt                                          */
/* _T3Interrupt        _OC5Interrupt                                          */
/* _SPI1ErrInterrupt   _OC6Interrupt                                          */
/* _SPI1Interrupt      _OC7Interrupt                                          */
/* _U1RXInterrupt      _OC8Interrupt                                          */
/* _U1TXInterrupt      _DMA4Interrupt                                         */
/* _ADC1Interrupt      _T6Interrupt                                           */
/* _DMA1Interrupt      _T7Interrupt                                           */
/* _SI2C1Interrupt     _SI2C2Interrupt                                        */
/* _MI2C1Interrupt     _MI2C2Interrupt                                        */
/* _CNInterrupt        _T8Interrupt                                           */
/* _INT1Interrupt      _T9Interrupt                                           */
/* _ADC2Interrupt      _INT3Interrupt                                         */
/* _IC7Interrupt       _INT4Interrupt                                         */
/* _IC8Interrupt       _C2RxRdyInterrupt                                      */
/* _DMA2Interrupt      _C2Interrupt                                           */
/* _OC3Interrupt       _DCIErrInterrupt                                       */
/* _OC4Interrupt       _DCIInterrupt                                          */
/* _T4Interrupt        _U1ErrInterrupt                                        */
/* _T5Interrupt        _U2ErrInterrupt                                        */
/* _INT2Interrupt      _DMA6Interrupt                                         */
/* _U2RXInterrupt      _DMA7Interrupt                                         */
/* _U2TXInterrupt      _C1TxReqInterrupt                                      */
/* _SPI2ErrInterrupt   _C2TxReqInterrupt                                      */
/*                                                                            */
/* PIC24E Primary Interrupt Vector Names:                                     */
/*                                                                            */
/* __INT0Interrupt     __C1RxRdyInterrupt      __U3TXInterrupt                */
/* __IC1Interrupt      __C1Interrupt           __USB1Interrupt                */
/* __OC1Interrupt      __DMA3Interrupt         __U4ErrInterrupt               */
/* __T1Interrupt       __IC3Interrupt          __U4RXInterrupt                */
/* __DMA0Interrupt     __IC4Interrupt          __U4TXInterrupt                */
/* __IC2Interrupt      __IC5Interrupt          __SPI3ErrInterrupt             */
/* __OC2Interrupt      __IC6Interrupt          __SPI3Interrupt                */
/* __T2Interrupt       __OC5Interrupt          __OC9Interrupt                 */
/* __T3Interrupt       __OC6Interrupt          __IC9Interrupt                 */
/* __SPI1ErrInterrupt  __OC7Interrupt          __DMA8Interrupt                */
/* __SPI1Interrupt     __OC8Interrupt          __DMA9Interrupt                */
/* __U1RXInterrupt     __PMPInterrupt          __DMA10Interrupt               */
/* __U1TXInterrupt     __DMA4Interrupt         __DMA11Interrupt               */
/* __AD1Interrupt      __T6Interrupt           __SPI4ErrInterrupt             */
/* __DMA1Interrupt     __T7Interrupt           __SPI4Interrupt                */
/* __NVMInterrupt      __SI2C2Interrupt        __OC10Interrupt                */
/* __SI2C1Interrupt    __MI2C2Interrupt        __IC10Interrupt                */
/* __MI2C1Interrupt    __T8Interrupt           __OC11Interrupt                */
/* __CM1Interrupt      __T9Interrupt           __IC11Interrupt                */
/* __CNInterrupt       __INT3Interrupt         __OC12Interrupt                */
/* __INT1Interrupt     __INT4Interrupt         __IC12Interrupt                */
/* __AD2Interrupt      __C2RxRdyInterrupt      __DMA12Interrupt               */
/* __IC7Interrupt      __C2Interrupt           __DMA13Interrupt               */
/* __IC8Interrupt      __DMA5Interrupt         __DMA14Interrupt               */
/* __DMA2Interrupt     __RTCCInterrupt         __OC13Interrupt                */
/* __OC3Interrupt      __U1ErrInterrupt        __IC13Interrupt                */
/* __OC4Interrupt      __U2ErrInterrupt        __OC14Interrupt                */
/* __T4Interrupt       __CRCInterrupt          __IC14Interrupt                */
/* __T5Interrupt       __DMA6Interrupt         __OC15Interrupt                */
/* __INT2Interrupt     __DMA7Interrupt         __IC15Interrupt                */
/* __U2RXInterrupt     __C1TxReqInterrupt      __OC16Interrupt                */
/* __U2TXInterrupt     __C2TxReqInterrupt      __IC16Interrupt                */
/* __SPI2ErrInterrupt  __U3ErrInterrupt        __ICDInterrupt                 */
/* __SPI2Interrupt     __U3RXInterrupt                                        */
/*                                                                            */
/*                                                                            */
/* For alternate interrupt vector naming, simply add 'Alt' between the prim.  */
/* interrupt vector name '_' and the first character of the primary interrupt */
/* vector name.  There are no Alternate or 'Alt' vectors for the 24E family.  */
/*                                                                            */
/* For example, the vector name _ADC2Interrupt becomes _AltADC2Interrupt in   */
/* the alternate vector table.                                                */
/*                                                                            */
/* Example Syntax:                                                            */
/*                                                                            */
/* void __attribute__((interrupt,auto_psv)) <Vector Name>(void)               */
/* {                                                                          */
/*     <Clear Interrupt Flag>                                                 */
/* }                                                                          */
/*                                                                            */
/* For more comprehensive interrupt examples refer to the C30 (MPLAB C        */
/* Compiler for PIC24 MCUs and dsPIC DSCs) User Guide in the                  */
/* <compiler installation directory>/doc directory for the latest compiler    */
/* release.                                                                   */
/*                                                                            */
/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

void __attribute__((__interrupt__)) _OC1Interrupt(void)
{
    IFS0bits.OC1IF = 0; //Reset OC1 interrupt flag

#if DMA_TOGGLE != 1
    /*
    static unsigned int toggle = 0;
    TRISBbits.TRISB3 = toggle;  // output ground / high impedance
    toggle ^= 1;
     */
#endif
}

//******************** Interrupt sevice rotuine for ADC ***************************/
// this should be 64 - if we sample at 8 MHz, 32 samples take 4 us, than multiply this by 15998520 clock ticks and we get 64
// the actual value that comes up is 132 - twice as much. either my reasoning is wrong, there is a bug in the code or the datasheet is lying
static unsigned int adc_clock_ticks[100];
void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt(void)
{
    //static unsigned int prev_cap;
    //unsigned int new_cap;
    //static unsigned int i = 0;
    //LATBbits.LATB14 = 1;

#if ADC_TOGGLE
    static unsigned int toggle = 0;
    LATBbits.LATB14 = toggle;   // chanel 14 high/low
    toggle ^= 1;
#endif
    IFS0bits.AD1IF = 0;  //Clear Interrupt status of ADC

    /*
    if(oc1_enable)
    {
        OC1CON1bits.OCM = 5;    // start oc1 timer
        oc1_enable = 0;
    }*/
}

//******************** Interrupt sevice rotuine for DMA ***************************/
static unsigned int dma_toggle = 0;
static unsigned int dma_clock_ticks[100];
/* backup
void __attribute__((interrupt,no_auto_psv)) _DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0;  //Clear Interrupt status of DMA

    // check if the source of the interrupt is done transfer
    if(DMAINT0bits.DONEIF)
    {
        // dma transfer finished
        DMAINT0bits.DONEIF = 0;
#if DMA_TOGGLE
        TRISBbits.TRISB3 = dma_toggle;  // output ground / high impedance
        dma_toggle ^= 1;
#endif
        //offset += (LIST_SIZE + 1) << 1; // update the offset
        offset += 32; // update the offset
        if (offset == 2048)
        {
            // TODO: prepare flash to write a page of data and start dma1 transfer of ADCdata_p0 to epmp - should prob just set a flag here and do that outside of the interrupt
        }
        else if (offset == 4096)
        {
            offset = 0;
            //LATBbits.LATB14 = toggle;   // chanel 14 high/low
            //toggle ^= 1;
        }
        //DMASRC0 = ((unsigned int) &ADRES0) + (offset%64);   // update the source value - we are reading from bottom or upper 16 words of adc fifo
        //DMADST0 = ((unsigned int) ADCdata_p0) + offset; // update the destination address
#if LIST_SIZE < 31
        DMASRC0 = 0360 + (offset%64);   // update the source value - we are reading from bottom or upper 16 words of adc fifo
#endif
        DMADST0 = 0x1800 + offset; // update the destination address
    }
}
*/

void __attribute__((interrupt,no_auto_psv)) _DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0;  //Clear Interrupt status of DMA

    // check if the source of the interrupt is done transfer
    if(DMAINT0bits.DONEIF)
    {
        // dma transfer finished
        DMAINT0bits.DONEIF = 0;
#if DMA_TOGGLE
        LATBbits.LATB13 = dma_toggle;
        //TRISBbits.TRISB3 = dma_toggle;  // output ground / high impedance
        dma_toggle ^= 1;
#endif
        //offset += (LIST_SIZE + 1) << 1; // update the offset
        offset += 64; // update the offset
        if (offset == 2048)
        {
            // TODO: prepare flash to write a page of data and start dma1 transfer of ADCdata_p0 to epmp - should prob just set a flag here and do that outside of the interrupt
        }
        else if (offset == 4096)
        {
            offset = 0;
            //LATBbits.LATB14 = toggle;   // chanel 14 high/low
            //toggle ^= 1;
            ADC_Pipeline_Stop();
        }
        //DMASRC0 = ((unsigned int) &ADRES0) + (offset%64);   // update the source value - we are reading from bottom or upper 16 words of adc fifo
        //DMADST0 = ((unsigned int) ADCdata_p0) + offset; // update the destination address
#if LIST_SIZE < 31
        DMASRC0 = 0360 + (offset%64);   // update the source value - we are reading from bottom or upper 16 words of adc fifo
#endif
        DMADST0 = 0x1800 + offset; // update the destination address
    }
}

void __attribute__((interrupt,no_auto_psv)) _DMA1Interrupt(void)
{
    IFS0bits.DMA1IF = 0;  //Clear Interrupt status of DMA

    // check if the source of the interrupt is done transfer
    if(DMAINT1bits.DONEIF)
    {
        // dma transfer finished
        DMAINT1bits.DONEIF = 0;

#if DMA_TOGGLE
        TRISBbits.TRISB3 = dma_toggle;  // output ground / high impedance
        dma_toggle ^= 1;
#endif
        //offset += (LIST_SIZE + 1) << 1; // update the offset
        offset += 32; // update the offset
        if (offset == 2048)
        {
            // TODO: prepare flash to write a page of data and start dma1 transfer of ADCdata_p0 to epmp - should prob just set a flag here and do that outside of the interrupt
        }
        else if (offset == 4096)
        {
            offset = 0;
            //LATBbits.LATB14 = toggle;   // chanel 14 high/low
            //toggle ^= 1;

            // temporary
            ADC_Pipeline_Stop();
        }
        DMADST0 = 0x1800 + offset; // update the destination address
    }
}


static unsigned long clock_ticks[100]; // this should be 16 000 000 - the IC timer is clocked from Fcy

void __attribute__((interrupt,no_auto_psv)) _IC1Interrupt(void)
{
  static unsigned long prev_capture;
  unsigned long new_capture;
  static unsigned int i = 0;

  //static int LED = 0;

  IFS0bits.IC1IF = 0; //Clear Interrupt status of IC1

  if(IC_enable)
  {
      IC2CON2bits.TRIGSTAT = 1;
      IC1CON2bits.TRIGSTAT = 1;
      IC_enable = 0;
  }

  if(ADC_enable)
  {
      OC1CON1bits.OCM = 5;    // start oc1 timer
      ADC_Pipeline_Start();
      ADC_enable = 0;
      LATBbits.LATB14 = 1;   // chanel 14 high
  }

  new_capture = IC2BUF;
  new_capture = (new_capture << 16) | IC1BUF;
  clock_ticks[i++] = new_capture - prev_capture;
  prev_capture = new_capture;
  

  // this should happen as many times as many seconds the experiment will run (max is determined by flash capacity ~100 s)
  if(i == 100)
  {
      i = 0;
  }

  // debug - TODO: remove this at some point
  // toggle led on every rising edge the led is not already on (ie, battery charging)
  /*  if (LED && PORTBbits.RB3)
    TRISBbits.TRISB3 = 0;  // output ground
  else
    TRISBbits.TRISB3 = 1;  // high impedance
  LED ^= 1;
  */
}
