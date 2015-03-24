# WiMDAS
MPLAB X project for PIC24FJ128GC006 uC. may contain useful ADC, DMA, I2C, SPI, SD card, USB, IC ,NRF24L01+, U-BLOX LEA6-T, NAND Flash configs and code. Free to use for personal and hobby projects. Contact if you want to profit from it ;)

TODO: what follows is just a draft and probably not fully complete. still worth giving it a go

## Start development
In order to start developing for the WiMDAS you need the following:
 - Download a copy of this repo
 - Download and install MPLAB X IDE (v2.30 as of 24.02.2015) from Downloads section at http://www.microchip.com/pagehandler/en-us/family/mplabx/ 
 - Download and install the MPLAB XC16 Compiler (v1.24 at the time of writing) from Downloads at http://www.microchip.com/pagehandler/en_us/devtools/mplabxc/ 
 - Launch mplab x ide and open the WiMDAS_final.X project
  
if you cant connect to pickit 3 from mplab, download PICkit 3 v3.01 (PICkit 3 Programmer) from <link>.
Tools -> Check communication
Tools -> Downloadc PICkit operating system
Tools -> revert to MPLAB mode

mplab should then see the pickit and download whatever software it wants.

if you seem to have connection to the programmer, but you still cant program the chip, go to project properties and make sure PICkit 3 is selected and not the particular device with serial number.
