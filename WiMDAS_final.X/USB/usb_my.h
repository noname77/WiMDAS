
#ifndef USB_MY_H
#define	USB_MY_H

extern uint8_t readBuffer[CDC_DATA_OUT_EP_SIZE];
extern uint8_t writeBuffer[CDC_DATA_IN_EP_SIZE];
extern uint8_t write_index;

void CDC_Tasks();

#endif	/* USB_MY_H */

