/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#include <xc.h>

#include <stdint.h>          /* For uint32_t definition */
#include <stdbool.h>         /* For true/false definition */

#include "user.h"            /* variables/params used by user.c */

#include "USB/usb.h"
#include "ADC_pipeline.h"
#include "lea6t.h"
#include "rtc.h"
#include "commands.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

// initializes inputs and outputs - http://www.engscope.com/pic24-tutorial/5-inputs-and-outputs/
void gpio_init(void)
{
    //printf("Initializing GPIO...\n\r");
    // analog inputs
    TRISBbits.TRISB12 = 1; // AN12(RB12)
    TRISBbits.TRISB13 = 1; // AN13(RB13) // TODO: changed temporarly to digital output to test stuff
    TRISBbits.TRISB14 = 0; // AN14(RB14) // TODO: changed temporarly to digital output to test stuff
    //LATBbits.LATB13 = 0;   // chanel 13 low at startup
    LATBbits.LATB14 = 0;   // chanel 14 low at startup

    ANSBbits.ANSB12 = 1;
    ANSBbits.ANSB13 = 1;
    ANSBbits.ANSB14 = 0;

    // digital inputs
    TRISBbits.TRISB3 = 1;  // battery chgn
    TRISBbits.TRISB7 = 1;  // gps timepulse0
    TRISBbits.TRISB15 = 1; // gps timepulse1
    TRISGbits.TRISG6 = 1;  // flash R/B
    TRISFbits.TRISF3 = 1;  // VBUS sense

    ANSBbits.ANSB3 = 0;
    ANSBbits.ANSB7 = 0;
    ANSBbits.ANSB15 = 0;
    ANSGbits.ANSG6 = 0;
    ANSFbits.ANSF3 = 0;

    // outputs
    // TODO: setup flash pins
    TRISFbits.TRISF0 = 0;  // radio CEn
    TRISFbits.TRISF1 = 0;  // radio CSn
    TRISDbits.TRISD2 = 0;  // sdcard CS
    TRISDbits.TRISD11 = 0; // gps reset_n
    TRISBbits.TRISB6 = 0;  // gps extint
    TRISGbits.TRISG7 = 0;  // flash CLE
    TRISGbits.TRISG8 = 0;  // flash ALE
    TRISGbits.TRISG9 = 0;  // flash WP
    // for new pcb rev
    //TRISBbits.TRISB0 = 0; // flash CE

    LATFbits.LATF0 = 1;   // radio off
    LATFbits.LATF1 = 1;   // radio spi deselected
    LATDbits.LATD2 = 1;   // sdcard spi deselect
    LATDbits.LATD11 = 0;  // gps reset (dont drive high! - needs open drain)
    LATBbits.LATB6 = 0;   // gps extint low
    LATGbits.LATG7 = 0;   // flash CLE low
    LATGbits.LATG8 = 0;   // flash ALE low
    LATGbits.LATG9 = 0;   // flash WP low
    LATBbits.LATB3 = 0;   // CHGn low
    // for new pcb rev
    //LATBbits.LATB0 = 0; // flash CE low
}

void i2c_init(void)
{
    I2C1CONbits.I2CEN = 0;   // disable i2c
    I2C1CONbits.I2CSIDL = 1; // continue operation in idle mode
    I2C1CONbits.IPMIEN = 0;  // we are master, so disable
    I2C1CONbits.A10M = 0;    // 7 bit slave address
    I2C1CONbits.DISSLW = 1;  // slew rate control disabled
    I2C1CONbits.SMEN = 0;    // SMBus input thresholds disabled
    I2C1CONbits.ACKDT = 1;   // master sends a nack

    I2C1BRG = 0x9D;          // 100 kHz clock

    I2C1CONbits.I2CEN = 1;   // enable the module
}

unsigned int IC_enable;
// initialize input capture for gps 1 pps
void IC_init(void)
 {
    //printf("Initializing Input Capture...\n\r");

    // Force IC module off just in case
    IC1CON1 = 0x0000;
    IC2CON1 = 0x0000;

    IC2CON1bits.ICSIDL = 0;     // doesn't stop at idle CPU
    IC2CON1bits.ICTSEL = 0b111; // select system clock as counter TODO: is it 32 or 16 MHz? prob. Fosc/2 so 16
    IC2CON1bits.ICI = 0;        // interrupt every capture event TODO: maybe better to interrupt every 4th or 16th for longer term stability?
    IC2CON1bits.ICM = 0;        // capture disabled

    IC2CON2bits.IC32 = 1;       // 32 bit mode
    IC2CON2bits.ICTRIG = 1;     // Trigger IC Mode
    IC2CON2bits.TRIGSTAT = 0;   // timer off
    IC2CON2bits.SYNCSEL = 0;    // Select No Sync Source

    IC1CON1bits.ICSIDL = 0;     // doesn't stop at idle CPU
    IC1CON1bits.ICTSEL = 0b111; // select system clock as counter TODO: is it 32 or 16 MHz? prob. Fosc/2 so 16
    IC1CON1bits.ICI = 0;        // interrupt every capture event TODO: maybe better to interrupt every 4th or 16th for longer term stability?
    IC1CON1bits.ICM = 0;        // capture disabled

    IC1CON2bits.IC32 = 1;       // 32 bit mode
    IC1CON2bits.ICTRIG = 1;     // Trigger IC Mode
    IC1CON2bits.TRIGSTAT = 0;   // timer off
    IC1CON2bits.SYNCSEL = 0;    // Select No Sync Source

    RPINR7bits.IC1R = 7;        // Set IC1 to RP7/RB7
    IPC0bits.IC1IP = 1;         // interrupt priority 1 - lowest priority
    IEC0bits.IC1IE = 1;         // Enable IC1 Interrupt

    // TODO: make sure I dont need to flush the buffers here

    IC2CON1bits.ICM = 0b011;  // enable capture of every rising edge
    IC1CON1bits.ICM = 0b011;  // enable capture of every rising edge

    IC_enable = 1;
}

//setup timer4 to generate an interrupt every 4 us - so in the middle of adc buffer full
/*
unsigned int timer4_enable = 0;
void timer4_init(void)
{
    T4CON = 0x00; //Stops the Timer1 and reset control reg.
    TMR4 = 0x00; //Clear contents of the timer register
    PR4 = 0x40; //Load the Period register with the value 0x40 for 4 us interval generation

    IPC6bits.T4IP = 6; //Setup Timer1 interrupt for desired priority level
    // (This example assigns level 1 priority)
    IFS1bits.T4IF = 0; //Clear the Timer1 interrupt status flag
    IEC1bits.T4IE = 1; //Enable Timer1 interrupts
    T4CONbits.TON = 1; //Start Timer4 with prescaler settings at 1:1 and clock source set to the internal instruction cycle

    timer4_enable = 1;
}
*/

unsigned int oc1_enable = 0;
// used to generate an interrupt every 4 us - so in the middle of adc buffer full
// instead of timer - couldnt get the timer to work well
void oc1_init(void)
{
    OC1CON1 = 0;                // It is a good practice to clear off the control bits initially
    OC1CON2 = 0;
    OC1CON1bits.OCSIDL = 0;     // continue operation in idle mode
    OC1CON1bits.OCTSEL = 0x07;  // This selects the peripheral clock as the clock input to the OC module
    OC1CON1bits.TRIGMODE = 1;   // TRIGSTAT is cleared when OCxRS = OCxTMR

    OC1CON2bits.OCTRIG = 1;       // Trigger OCx from:
    //OC1CON2bits.OCTRIG = 0;     // sync OCx to:
    //OC1CON2bits.SYNCSEL = 0x1f; // self
    OC1CON2bits.SYNCSEL = 0x1B;   // A/D module
    OC1R = 0x1e;                  // this could be anything really
    OC1RS = 0x3a;                 // interrupt on match here
    
    IFS0bits.OC1IF = 0;
    IPC0bits.OC1IP = 5;
    IEC0bits.OC1IE = 1;

    oc1_enable = 1;
}

// initialize dma0 to transfer between adc higher buffer and ram
void dma0_init()
{
// configure dma for continous repeted transfer of 16 samples from adc to ram
    DMACH0bits.CHEN = 0;        // disable dma channel
    DMACH0bits.NULLW = 0;       // no dummy write
    DMACH0bits.RELOAD = 1;      // SRC, DST, CNT not reloaded to their init values
    DMACH0bits.SAMODE = 1;      // DMASRC is incremented by SIZE after the transfer;
    DMACH0bits.DAMODE = 1;      // DMADST is incremented by SIZE after the transfer
    DMACH0bits.TRMODE = 3;      // Repeated Continous operation
    DMACH0bits.SIZE = 0;        // Word size (16 bit)

    DMAINT0 = 0;                // clear the reg - getting halfif interrupt when shouldnt..
    DMAINT0bits.CHSEL = 0x2F;   // pipeline adc interrupt is the trigger (see p. 90 of device datasheet)
    DMAINT0bits.HALFEN = 0;     // no interrupt in the halfway through the transfer

    DMASRC0 = (unsigned int) &ADRES0;     // source address for dma (upper 16 samples)
    DMADST0 = (unsigned int) ADCdata_p0;   // destination address, has to be somewhere in ram - lets point to the first allocated page (see the header file)
    //DMACNT0 = LIST_SIZE + 1;             // transaction counter TODO: maybe this should be 32? for now lets hope transfering 16 words takes less than filling them by adc
    DMACNT0 = 32;                          // hardcode transfer of half the buffer
    DMACH0bits.CHEN = 1;                   // enable dma channel
    // enable and trigger source interrupt now
}

// initialize dma0 to transfer between adc higher buffer and ram
/* backup
void dma0_init()
{
// configure dma for continous repeted transfer of 16 samples from adc to ram
    DMACH0bits.CHEN = 0;        // disable dma channel
    DMACH0bits.NULLW = 0;       // no dummy write
    DMACH0bits.RELOAD = 1;      // SRC, DST, CNT not reloaded to their init values
    DMACH0bits.SAMODE = 1;      // DMASRC is incremented by SIZE after the transfer;
    DMACH0bits.DAMODE = 1;      // DMADST is incremented by SIZE after the transfer
    DMACH0bits.TRMODE = 3;      // Repeated Continous operation
    DMACH0bits.SIZE = 0;        // Word size (16 bit)
    
    DMAINT0 = 0;                // clear the reg - getting halfif interrupt when shouldnt..
    DMAINT0bits.CHSEL = 0x2F;   // pipeline adc interrupt is the trigger (see p. 90 of device datasheet)
    DMAINT0bits.HALFEN = 0;     // no interrupt in the halfway through the transfer

    DMASRC0 = (unsigned int) &ADRES15;     // source address for dma (upper 16 samples)
    DMADST0 = (unsigned int) ADCdata_p0;   // destination address, has to be somewhere in ram - lets point to the first allocated page (see the header file)
    //DMACNT0 = LIST_SIZE + 1;             // transaction counter TODO: maybe this should be 32? for now lets hope transfering 16 words takes less than filling them by adc
    DMACNT0 = 16;                          // hardcode transfer of half the buffer
    DMACH0bits.CHEN = 1;                   // enable dma channel
    // enable and trigger source interrupt now
}
*/


// initialize dma1 to transfer between adc lower buffer and ram
void dma1_init()
{
    DMACH1bits.CHEN = 0;        // disable dma channel
    DMACH1bits.NULLW = 0;       // no dummy write
    DMACH1bits.RELOAD = 1;      // SRC, DST, CNT not reloaded to their init values
    DMACH1bits.SAMODE = 1;      // DMASRC is incremented by SIZE after the transfer;
    DMACH1bits.DAMODE = 1;      // DMADST is incremented by SIZE after the transfer
    DMACH1bits.TRMODE = 3;      // Repeated Continous operation
    DMACH1bits.SIZE = 0;        // Word size (16 bit)

    DMAINT1 = 0;                // clear the reg - getting halfif interrupt when shouldnt..
    DMAINT1bits.CHSEL = 0x3A;   // oc1 interrupt is the trigger
    DMAINT1bits.HALFEN = 0;     // no interrupt in the halfway through the transfer

    DMASRC1 = (unsigned int) &ADRES0;       // source address for dma (has to be adc)
    DMADST1 = (unsigned int) ADCdata_p0;    // destination address, has to be somewhere in ram - lets point to the first allocated page (see the header file)
    DMACNT1 = 16;                           // transaction counter TODO: maybe this should be 32? for now lets hope transfering 16 words takes less than filling them by adc
    //DMACNT1 = 32;                           // transaction counter TODO: maybe this should be 32? for now lets hope transfering 16 words takes less than filling them by adc

    DMACH1bits.CHEN = 1; // enable dma channel
    // enable and trigger source interrupt now

}

// initialize dma2 to transfer between ram and epmp
void dma2_init()
{

}

void  dma_init()
{
    DMACONbits.DMAEN = 1;   // enable dma module
    DMACONbits.PRSSEL = 0;  // fixed priority scheme
    DMAL = 0x17C0; // lower ram addres limit
    DMAH = 0x2800; // higher ram address limit
    // TODO: disable upper 4 channels
    dma0_init();
    //dma1_init();
    IFS0bits.DMA0IF = 0; // clear DMA0 interrupt flag
    IPC1bits.DMA0IP = 6; // interrupt priority
    IEC0bits.DMA0IE = 1; // enable dma0 interrupts
    IFS0bits.DMA1IF = 0;
    IPC3bits.DMA1IP = 6;
    IEC0bits.DMA1IE = 1;
}

void InitApp(void)
{
    // Setup analog functionality and port direction
    gpio_init();

    // Initialize peripherals
    i2c_init();
    dma_init();
    //oc1_init(); // output capture for interrupt generation
    
    ADC_Pipeline_Init(POSREF_AVDD | NEGREF_AVSS, ADCFORM_UINT);  // TODO: meybe better use fractional format and use high byte for 8 bit resolution
    ADC_Pipeline_configure_samplelist();

    lea6t_init();
    IC_init();
    initRtcc(&RtccTime, &RtccTime); // Initializes RTCC and sets time hhmmss to 000000
    register_commands();
    
    USBDeviceInit();
    write_index = 0;

}

