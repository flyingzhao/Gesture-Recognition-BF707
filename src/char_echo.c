/*********************************************************************************

Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/
/*!
* @file      char_echo.c
*
* @brief     This is the primary source file for UART character echo example.
*
*/

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"suppress all MISRA rules for test")
#endif /* _MISRA_RULES */


#include "char_echo.h"
#include <blackfin.h>
#include <stdio.h>
#include <services\int\adi_int.h>
#include <services\int\adi_sec.h>
#include <drivers\uart\adi_uart.h>
#include <services\pwr\adi_pwr.h>
#include "adi_initialize.h"

/* Number of Tx and Rx buffers */
#define NUM_BUFFERS     2

/* ADI initialization header */
//#include "system/adi_initialize.h"
/* UART Handle */
static ADI_UART_HANDLE  ghUART;

/* Rx and Tx buffers */
static uint8_t RxBuffer[NUM_BUFFERS];

unsigned char bu[10]="hjxkl";

/* Memory required for operating UART in interrupt mode */
static uint8_t  gUARTMemory[ADI_UART_BIDIR_DMA_MEMORY_SIZE];
/* UART return code */
	ADI_UART_RESULT    eResult;
	/* SEC return code */
	ADI_SEC_RESULT     eSECResult;
	/* Tx size */
	uint32_t           nTxSize = 1;
	/* Flag which indicates whether to stop the program */
    _Bool bStopFlag = false;

    //adi_initComponents(); /* auto-generated code */


int32_t opencom(void)
{
	/* UART return code */
	ADI_UART_RESULT    eResult;
	/* SEC return code */
	ADI_SEC_RESULT     eSECResult;
	/* Tx size */
	uint32_t           nTxSize = 1;
	/* Flag which indicates whether to stop the program */
    _Bool bStopFlag = false;

    //adi_initComponents(); /* auto-generated code */

    printf("UART Char echo blocking mode example \n");


    /* Initialize Power service */
#if defined (__ADSPBF707_FAMILY__)
    if(adi_pwr_Init(0, CLKIN) != ADI_PWR_SUCCESS)
    {
        REPORT_ERROR("Failed to initialize power service \n");
        return FAILED;
    }
#else
    if(adi_pwr_Init(CLKIN, CORE_MAX, SYSCLK_MAX, VCO_MIN) != ADI_PWR_SUCCESS)
    {
        REPORT_ERROR("Failed to initialize power service \n");
        return FAILED;
    }
#endif
    /*
     * Initialize UART
     */
    /* Open UART driver */
    if((eResult = adi_uart_Open(UART_DEVICE_NUM,
                                 ADI_UART_DIR_BIDIRECTION,
                                 gUARTMemory,
                                 ADI_UART_BIDIR_DMA_MEMORY_SIZE,
                                 &ghUART)) != ADI_UART_SUCCESS)
    {
    	REPORT_ERROR("Could not open UART Device 0x%08X \n", eResult);
    	return FAILED;
    }
    /* Set the UART Mode */
    if((eResult = adi_uart_SetMode(ghUART,
                                    ADI_UART_MODE_UART
                                   )) != ADI_UART_SUCCESS)
    {
    	REPORT_ERROR("Could not set the Mode 0x%08X \n", eResult);
    	return FAILED;
    }

    /* Set UART Baud Rate */
    if((eResult = adi_uart_SetBaudRate(ghUART,
    									BAUD_RATE
                                        )) != ADI_UART_SUCCESS)
    {
    	REPORT_ERROR("Could not set the Baud Rate 0x%08X \n", eResult);
    	return FAILED;
    }

    /* Set number of stop bits */
    if((eResult = adi_uart_SetNumStopBits(ghUART,
                                            ADI_UART_ONE_STOPBIT
                                         )) != ADI_UART_SUCCESS)
    {
    	REPORT_ERROR("Could not set the stop bits 0x%08X \n", eResult);
    	return FAILED;
    }

    /* Set number of stop bits */
    if((eResult = adi_uart_SetWordLen(ghUART,
                                          ADI_UART_WORDLEN_8BITS
                                         )) != ADI_UART_SUCCESS)
    {
    	REPORT_ERROR("Could not set word length 0x%08X \n", eResult);
    	return FAILED;
    }

    printf("Setup Hyperterminal as described in Readme file. \n");
    printf("Press any key on the key board and notice the character  being echoed to Hyperterminal. \n");
    printf("\n Press return key to stop the program.\n");
    return 0;
}
    /* UART processing loop */
   // while(bStopFlag == false)
int32_t serial(unsigned char *state){

        /* Read a character */
     /*  if((eResult = adi_uart_Read(ghUART,
                &RxBuffer[0],
                1
        )) != ADI_UART_SUCCESS)
        {
            REPORT_ERROR("Could not do a read 0x%08X \n", eResult);
            return FAILED;
        }


        /* If return character is pressed, write back \n character along with \r */
        if(RxBuffer[0] == '\r')
        {
            RxBuffer[1] = '\r';

            /* Transmit two characters in this case */
            nTxSize     = 2u;

            /* Stop the program upon receiving carriage return */
           // bStopFlag = true;
        }

        /* Write back the character */
        if((eResult = adi_uart_Write(ghUART,
                state,
                nTxSize
        )) != ADI_UART_SUCCESS)
        {
            REPORT_ERROR("Could not do a write 0x%08X \n", eResult);
            return FAILED;
        }
        return 0;
}


int32_t  closecom(void){
    /* Close the UART */
    if((eResult = adi_uart_Close(ghUART)) != ADI_UART_SUCCESS)
    {
        REPORT_ERROR("Could not close UART driver 0x%08X \n", eResult);
        return FAILED;
    }

    return PASSED;
}


#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */
