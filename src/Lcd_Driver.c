#include "define.h"
#include "Lcd_Driver.h"
#include "delay.h"
#include <cdefbf707.h>
#include <stdio.h>
#include <services/int/adi_sec.h>
#include <services/int/adi_int.h>
#include <drivers/spi/adi_spi.h>



//初始化SPI设备
static ADI_SPI_HANDLE phSPI;

uint8_t SpiMemory[ADI_SPI_DMA_MEMORY_SIZE];
void SPI1_Init()
{
#if 0
	*pREG_PORTD_DIR |= BITM_PORT_DATA_PX2;	//配置DCX的方向

    *pREG_SPI1_SLVSEL  = 0xFE02;
	*pREG_SPI1_CLK = 0x1;			/*  BAUD_RATE_DIVISOR; */
	*pREG_SPI1_TXCTL=0x0000005; 	/*  initiate transfer on write */
	*pREG_SPI1_CTL |= BITM_SPI_CTL_MSTR|BITM_SPI_CTL_CPHA |BITM_SPI_CTL_CPOL;	/*  spi_setting */
#else
	adi_initpinmux();

	*pREG_PORTA_DIR |= BITM_PORT_DATA_PX1;

	adi_spi_Open(1u, &SpiMemory, (uint32_t)ADI_SPI_DMA_MEMORY_SIZE, &phSPI);
	/* device in master of the SPI interface */
	adi_spi_SetMaster(phSPI, true);
	/* SPI slave select in controlled by software not hardware */
	//adi_spi_SetHwSlaveSelect(phSPI, true);
	adi_spi_SetClockPolarity(phSPI, true);
	//adi_spi_SetClockPhase(phSPI, true);
	*pREG_SPI1_CTL |= BITM_SPI_CTL_CPOL;
	/* SPI clock is SCLK divided by 1000 + 1 */
	adi_spi_SetClock(phSPI, 2u);

	*pREG_SPI1_TXCTL=0x0000005;
	/* SPI slave select is on SPI slave select 1 pin */
	adi_spi_SetSlaveSelect(phSPI, ADI_SPI_SSEL_ENABLE2);

#endif
}

int write_SPI(unsigned char cBuf)
{
	uint32_t stat;
	int i;
	int times, was_true;

	while (*pREG_SPI1_STAT & BITM_SPI_STAT_TFF)
		;
	*pREG_SPI1_TFIFO = cBuf;


	/*
	* It's possible when we check REG_SPI0_STAT, the data has just been
	* moved from TFIFO to shift register.  At that time TFIFO is empty,
	* and SPIF is still set (from last transfer and not cleared yet since
	* the first bit has not been shifted out from the shift register).
	*
	*  So we can't simply use (TFIFO_EMPTY && SPIF) as the indication of
	*  transfer finish.  We need to check for more times until
	*
	*  -- it holds continuously in checking 100 times, (100 is a somewhat
	*  arbitrarily chosen number).
	*
	*  -- or, it goes through TRUE, FALSE, TRUE again.
	*/
	//==
	times = 0;
	was_true = 0;
	do
	{
		stat = *pREG_SPI1_STAT;

		if ((stat & BITM_SPI_STAT_TFS) == ENUM_SPI_STAT_TFIFO_EMPTY
			&& (stat & BITM_SPI_STAT_SPIF) == ENUM_SPI_STAT_SPIF_HI)
		{
			if (was_true)
				break;

			times++;
		}
		else if (times != 0)
		{
			was_true = 1;
		}
	}while (times < 100);

	/*  ok */
	return 0;
}
//写命令
void Lcd_WriteIndex(u8 Data)
{
	*pREG_PORTA_DATA &= ~BITM_PORT_DATA_PX1;	//命令，DCX 拉低

	*pREG_SPI1_SLVSEL  = 0xFA04;		//片选
	*pREG_SPI1_CTL |= ENUM_SPI_CTL_EN;	//使能

	write_SPI(Data);

	*pREG_SPI1_CTL &= ~ENUM_SPI_CTL_EN; //关闭
	*pREG_SPI1_SLVSEL  = 0xFE04;		//关闭片选

}

void Lcd_WriteBuffer(unsigned char *Data)
{
	unsigned char picH,picL;
	*pREG_PORTA_DATA |= BITM_PORT_DATA_PX1;	//数据，DCX 拉高
	*pREG_SPI1_SLVSEL  = 0xFA04;	//片选
	*pREG_SPI1_CTL |= ENUM_SPI_CTL_EN;	//使能
	Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	for(int i=0;i<320*240;i++)
	 {
		picL=*(Data+i*2);	//数据低位在前
		picH=*(Data+i*2+1);
		Lcd_WriteData_16Bit(picH<<8|picL);
	 }

	*pREG_SPI1_CTL &= ~ENUM_SPI_CTL_EN; //关闭
	*pREG_SPI1_SLVSEL  = 0xFE04;		//关闭片选
}

/****************************************************************************
* 名    称：Lcd_WriteData(u8 Data)
* 功    能：向液晶屏写一个8位数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，内部函数
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
	*pREG_PORTA_DATA |= BITM_PORT_DATA_PX1;	//数据，DCX 拉高

	*pREG_SPI1_SLVSEL  = 0xFA04;		//片选
	*pREG_SPI1_CTL |= ENUM_SPI_CTL_EN;	//使能

	write_SPI(Data);

	*pREG_SPI1_CTL &= ~ENUM_SPI_CTL_EN; //关闭
	*pREG_SPI1_SLVSEL  = 0xFE04;		//关闭片选

}


/****************************************************************************
* 名    称：void Lcd_WriteData_16Bit(u16 Data)
* 功    能：向液晶屏写一个16位数据
* 入口参数：Data
* 出口参数：无
* 说    明：向控制器指定地址写入一个16位数据
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{	
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);	
}


/****************************************************************************
* 名    称：void Lcd_Init(void)
* 功    能：液晶初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化_ILI9225_176X220
****************************************************************************/
void Lcd_Init(void)
{	
#if USE_HARDWARE_SPI //使用硬件SPI
	SPI1_Init();
#else	
	LCD_GPIO_Init();//使用模拟SPI
#endif

	Lcd_WriteIndex(0xCB);
	Lcd_WriteData(0x39);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x34);
	Lcd_WriteData(0x02);

	Lcd_WriteIndex(0xCF);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0XC1);
	Lcd_WriteData(0X30);

	Lcd_WriteIndex(0xE8);
	Lcd_WriteData(0x85);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x78);

	Lcd_WriteIndex(0xEA);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0xED);
	Lcd_WriteData(0x64);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0X12);
	Lcd_WriteData(0X81);

	Lcd_WriteIndex(0xF7);
	Lcd_WriteData(0x20);

	Lcd_WriteIndex(0xC0);    //Power control
	Lcd_WriteData(0x23);   //VRH[5:0]

	Lcd_WriteIndex(0xC1);    //Power control
	Lcd_WriteData(0x10);   //SAP[2:0];BT[3:0]

	Lcd_WriteIndex(0xC5);    //VCM control
	Lcd_WriteData(0x3e); //对比度调节
	Lcd_WriteData(0x28);

	Lcd_WriteIndex(0xC7);    //VCM control2
	Lcd_WriteData(0x86);  //--

	Lcd_WriteIndex(0x36);    // Memory Access Control
#ifdef USE_HORIZONTAL
	Lcd_WriteData(0xE8); //C8	   //48 68竖屏//28 E8 横屏
#else
	Lcd_WriteData(0x48);
#endif

	Lcd_WriteIndex(0x3A);
	Lcd_WriteData(0x55);

	Lcd_WriteIndex(0xB1);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x18);

	Lcd_WriteIndex(0xB6);    // Display Function Control
	Lcd_WriteData(0x08);
	Lcd_WriteData(0x82);
	Lcd_WriteData(0x27);

	Lcd_WriteIndex(0xF2);    // 3Gamma Function Disable
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0x26);    //Gamma curve selected
	Lcd_WriteData(0x01);

	Lcd_WriteIndex(0xE0);    //Set Gamma
	Lcd_WriteData(0x0F);
	Lcd_WriteData(0x31);
	Lcd_WriteData(0x2B);
	Lcd_WriteData(0x0C);
	Lcd_WriteData(0x0E);
	Lcd_WriteData(0x08);
	Lcd_WriteData(0x4E);
	Lcd_WriteData(0xF1);
	Lcd_WriteData(0x37);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x10);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x0E);
	Lcd_WriteData(0x09);
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0XE1);    //Set Gamma
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x0E);
	Lcd_WriteData(0x14);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x11);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x31);
	Lcd_WriteData(0xC1);
	Lcd_WriteData(0x48);
	Lcd_WriteData(0x08);
	Lcd_WriteData(0x0F);
	Lcd_WriteData(0x0C);
	Lcd_WriteData(0x31);
	Lcd_WriteData(0x36);
	Lcd_WriteData(0x0F);

	Lcd_WriteIndex(0x11);    //Exit Sleep
	delay_ms(120);

	Lcd_WriteIndex(0x29);    //Display on
	Lcd_WriteIndex(0x2c);

}



/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 Xpos, u16 Ypos)
{	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData_16Bit(Xpos);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData_16Bit(Ypos);
	Lcd_WriteIndex(0x2c);	
} 
/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
//设置显示窗口
void Lcd_SetRegion(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData_16Bit(xStar);
	Lcd_WriteData_16Bit(xEnd);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData_16Bit(yStar);
	Lcd_WriteData_16Bit(yEnd);
	Lcd_WriteIndex(0x2c);
}

	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：xy坐标和颜色数据
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetXY(x,y);
	Lcd_WriteData_16Bit(Data);

}    

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	Lcd_WriteData_16Bit(Color);
    }   
}

