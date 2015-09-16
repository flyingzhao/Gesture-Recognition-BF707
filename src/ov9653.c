#include <cdefBF707.h>
#include <ccblkfn.h>
#include <sys\exception.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <drivers\twi\adi_twi.h>

#define PRESCALE_VALUE 10			
#define RESET_TWI 0					
#define CLKDIV_HI 90				
#define CLKDIV_LO 45

#pragma align(4)
unsigned char cmos_data[640*480*2];

void InitPPI0DMA(void)
{	
	*pREG_DMA14_ADDRSTART = cmos_data;
	*pREG_DMA14_XCNT = 640*2/4;
	*pREG_DMA14_XMOD = 0x4;
	*pREG_DMA14_YCNT = 480;
	*pREG_DMA14_YMOD = 0x4;

	*pREG_DMA14_CFG = ENUM_DMA_CFG_WRITE |
					  ENUM_DMA_CFG_MSIZE04 |
					  ENUM_DMA_CFG_PSIZE04 |
					  ENUM_DMA_CFG_ADDR2D |
					  ENUM_DMA_CFG_SYNC |
					  ENUM_DMA_CFG_STOP;
					  //ENUM_DMA_CFG_AUTO;
	ssync();
}


unsigned int  tmp ;


void InitEPPI0()
{	
	//==
	*pREG_EPPI0_CTL=0;
	*pREG_EPPI0_CTL = ENUM_EPPI_CTL_RXMODE |
					  ENUM_EPPI_CTL_FS1HI_FS2HI |
					  ENUM_EPPI_CTL_PACK_EN|
					  ENUM_EPPI_CTL_POLC11 |
					  ENUM_EPPI_CTL_DLEN08 |
					  ENUM_EPPI_CTL_NON656 |
					  ENUM_EPPI_CTL_SYNC2
					  ;

	ssync();
   	*pREG_EPPI0_LINE = 1280;
    ssync();
	*pREG_EPPI0_FRAME = 480;
	ssync();

}

void DisableEPPI0_DMA(void)
{
	*pREG_DMA14_CFG &= ~ENUM_DMA_CFG_EN;
	ssync();
	*pREG_EPPI0_CTL &= ~ENUM_EPPI_CTL_EN;
	ssync();
}

void EnableEPPI0_DMA(void)
{
    *pREG_DMA14_CFG |= ENUM_DMA_CFG_EN;
    ssync();
    *pREG_EPPI0_CTL |= ENUM_EPPI_CTL_EN;
    ssync();
}

void Reset_TWI0(void)
{
     //RESET_TWI CONTROLLER
	*pREG_TWI0_CTL = RESET_TWI;
	ssync();

	//CLEAR ALL ERRONOUS CONDITIONS BEFORE ENABLING TWI
	*pREG_TWI0_MSTRSTAT = BITM_TWI_MSTRSTAT_BUFWRERR | BITM_TWI_MSTRSTAT_BUFRDERR |
						  BITM_TWI_MSTRSTAT_LOSTARB |BITM_TWI_MSTRSTAT_ANAK | BITM_TWI_MSTRSTAT_DNAK;

	ssync();
	//CLEAR ALL INTERRUPTS BEFORE ENABLING TWI
	*pREG_TWI0_ISTAT = BITM_TWI_ISTAT_SINIT | BITM_TWI_ISTAT_SCOMP | BITM_TWI_ISTAT_SERR |
			           BITM_TWI_ISTAT_SOVF | BITM_TWI_ISTAT_MCOMP | BITM_TWI_ISTAT_MERR | BITM_TWI_ISTAT_TXSERV |
			           BITM_TWI_ISTAT_RXSERV;
	ssync();
	//FLUSH THE FIFOs - BOTH TX AND RX.
	*pREG_TWI0_FIFOCTL = BITM_TWI_FIFOCTL_TXFLUSH | BITM_TWI_FIFOCTL_RXFLUSH;
	ssync();
}

void delay(unsigned int tem)
{
	int i;
	for(i=0;i<tem;i++)
		asm("nop;");
}

void TWI0_MasterMode_Write(unsigned short DeviceAddr, unsigned short *TWI_Data_Pointer, unsigned short TX_Count, unsigned short TWI_TX_Length)
{
	int i, j;
	

	*pREG_TWI0_FIFOCTL = BITM_TWI_FIFOCTL_TXFLUSH | BITM_TWI_FIFOCTL_RXFLUSH;
	ssync();

	*pREG_TWI0_MSTRSTAT = BITM_TWI_MSTRSTAT_BUFWRERR | BITM_TWI_MSTRSTAT_BUFRDERR |
							  BITM_TWI_MSTRSTAT_LOSTARB |BITM_TWI_MSTRSTAT_ANAK | BITM_TWI_MSTRSTAT_DNAK;
	ssync();	


	*pREG_TWI0_FIFOCTL = 0;


	*pREG_TWI0_CTL = BITM_TWI_CTL_EN | PRESCALE_VALUE;


	*pREG_TWI0_CLKDIV = ((CLKDIV_HI) << 8) | (CLKDIV_LO);


	*pREG_TWI0_MSTRADDR = DeviceAddr;

	for (i = 0; i < TX_Count; i++)
	{	
		delay(10000);

		*pREG_TWI0_TXDATA8 = *TWI_Data_Pointer++;


		*pREG_TWI0_MSTRCTL = (TWI_TX_Length<<6) | BITM_TWI_MSTRCTL_EN;

		for (j = 0; j < (TWI_TX_Length-1); j++)
		{

			while (*pREG_TWI0_FIFOSTAT == BITM_TWI_FIFOSTAT_TXSTAT)
				ssync();

			*pREG_TWI0_TXDATA8 = *TWI_Data_Pointer++;

			ssync();
		}			

		while ((*pREG_TWI0_ISTAT & BITM_TWI_ISTAT_MCOMP) == 0)	//如果未完成transfer就等一等，同步一下
		ssync();	
		*pREG_TWI0_ISTAT = BITM_TWI_ISTAT_TXSERV | BITM_TWI_ISTAT_MCOMP;
	}
	asm("nop;");
	asm("nop;");
	asm("nop;");
}

void TWI_MasterMode_Read(unsigned short DeviceAddr, unsigned char *TWI_Data_Pointer, unsigned short Count)
{
	int i, j;

    /* clear the bit manually */
	*pREG_TWI0_FIFOCTL = 0;

	/* PRESCALE = fsclk/10MHz */
	*pREG_TWI0_CTL = BITM_TWI_CTL_EN | PRESCALE_VALUE;

	/* CLKDIV = (1/SCL)/(1/10MHz) */
	*pREG_TWI0_CLKDIV = ((CLKDIV_HI) << 8) | (CLKDIV_LO);

	/* target address (7-bits plus the read/write bit) */
	*pREG_TWI0_MSTRADDR = DeviceAddr;

	/* start transmission */
	*pREG_TWI0_MSTRCTL = (Count<<6) | BITM_TWI_MSTRCTL_EN | BITM_TWI_MSTRCTL_DIR;

	/* for each item */
	for (i = 0; i < Count; i++)
	{
		/* wait for data to be in FIFO */
		while (*pREG_TWI0_FIFOSTAT == 0)
			ssync();

		*TWI_Data_Pointer++ = *pREG_TWI0_RXDATA8;		/* read the data */
		ssync();
	}

	/* wait until transmission complete and MCOMP is set */
	while ((*pREG_TWI0_ISTAT & BITM_TWI_ISTAT_MCOMP) == 0)
			ssync();

	/* service TWI for next transmission */
	*pREG_TWI0_ISTAT = BITM_TWI_ISTAT_RXSERV | BITM_TWI_ISTAT_MCOMP;

	asm("nop;");
	asm("nop;");
	asm("nop;");
}


