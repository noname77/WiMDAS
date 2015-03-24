#ifndef _ADC_PIPELINE_H_
#define _ADC_PIPELINE_H_

extern unsigned int offset;
extern unsigned int ADC_enable;

// buffer pages for storing adc readings before the transfer to flash
extern unsigned int ADCdata_p0[1024];
extern unsigned int ADCdata_p1[1024];
extern unsigned int ADC_finished;

void ADC_Pipeline_Init(unsigned int ADCReferences, unsigned int ADCFormat);
void ADC_Pipeline_Disable(void);
void ADC_Pipeline_configure_samplelist(void);
void ADC_Tasks(void);

//------------------------------------------------------------------------------
//Public defintions
//------------------------------------------------------------------------------
#define LIST_SIZE 31
//#define ADC_Pipeline_Start() {offset = 0; ADL0CONLbits.SLEN = 1;}
#define ADC_Pipeline_Start() { ADL0CONLbits.SAMP=1; ADL0CONLbits.SAMP=0; offset = 0; ADL0CONLbits.SLEN = 1;}
#define ADC_Pipeline_Stop() { ADL0CONLbits.SLEN = 0; ADC_finished = 1; }

//Definitions for the ADCReferences parameter
#define POSREF_AVDD 0x0000
#define POSREF_EXTREF 0x4000
#define POSREF_BGBUF 0x8000

#define NEGREF_AVSS 0x0000
#define NEGREF_EXTREF 0x1000

//Definitions for the ADCFormat parameter
#define ADCFORM_UINT 0x0000
#define ADCFORM_SINT 0x0100
#define ADCFORM_SFRACTIONAL 0x0300
#define ADCFORM_UFRACTIONAL 0x0200

#endif // _ADC_PIPELINE_H_