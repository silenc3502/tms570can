#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_can.h"
#include "HL_esm.h"
#include "HL_sci.h"
#include "HL_etpwm.h"
#include "HL_sys_core.h"

#include <stdio.h>
#include <string.h>

#define UART        sciREG1

#define D_COUNT     8
#define D_SIZE      8

int data;

uint32 cnt = 0;
uint32 error = 0;
uint32 tx_done = 0;

uint8_t tx_data[D_COUNT] = {1, 2, 3, 4, 4, 3, 2, 1};
uint8_t rx_data[D_COUNT] = {0};

uint32_t checkPackets(uint8_t *src_packet, uint8_t *dst_packet, uint32_t psize);

void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 len);
void delay(uint32 time);
void disp_set(char *);

void delay(uint32 time)
{
    int i;

    for(i = 0; i < time; i++)
        ;
}

int main(void)
{
    //sciInit();
    //disp_set("UART Init Success!!!\n\r\0");

    etpwmInit();
    canInit();
    _enable_interrupt_();

    //printf("start\n");
    etpwmStartTBCLK();
    canEnableErrorNotification(canREG1);

    etpwmREG1->CMPA = 1500 * 1.25;

#if 1
    while(1)
    {
        //printf("CAN TX\n");
        //delay(10000000);
        //canTransmit(canREG1, canMESSAGE_BOX1, (const uint8 *)&tx_data[0]);
    }
#endif

    return 0;
}

void canMessageNotification(canBASE_t *node, uint32_t messageBox)
{
#if 0
    if(node == canREG1)
    {
        printf("CAN RX\n");
        while(!canIsRxMessageArrived(canREG1, canMESSAGE_BOX2))
            ;
        canGetData(canREG1, canMESSAGE_BOX2, (uint8 *)&rx_data[0]);
        printf("rx_data : %x\n", *rx_data);
    }
#endif

    while(!canIsRxMessageArrived(canREG1, canMESSAGE_BOX2))
        ;

    canGetData(canREG1, canMESSAGE_BOX2, (uint8 *)&rx_data[0]);

    switch(rx_data[0])
    {
        case 13:
            data = (rx_data[1] - 0x30) * 1000 + (rx_data[2] - 0x30) * 100 + (rx_data[3] -0x30) * 10 + rx_data[4] - 0x30;
            data *= 1.25;
            break;
        default:
            break;
    }

    etpwmREG1->CMPA = data;

    memset(rx_data, 0, sizeof(rx_data));
    data = 0;
}
