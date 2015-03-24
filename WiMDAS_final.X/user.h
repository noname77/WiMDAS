/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

#define ADC_TOGGLE 1
#define DMA_TOGGLE 1

// TODO: shoul i move it somewhere else?
extern unsigned int IC_enable;
extern unsigned int timer4_enable;
extern unsigned int oc1_enable;

void InitApp(void);         /* I/O and Peripheral Initialization */
