/*****************************************************************************
 * image_catchandshow.c
 *****************************************************************************/
//#include "ov9653rgb.h"
#include "ov9653.h"
#include <stdio.h>
#include "adi_initialize.h"
#include "QDTFT_demo.h"
#include "cdefBF707.h"
#include "handrecog.h"
#include <time.h>

#include <ccblkfn.h>


#include "char_echo.h"

#define T 5		//7帧连续图片进行手势识别


unsigned char imskin[320*240];	//存放图片处理过程中的一系列中间结果
unsigned char imR[320*240];
unsigned char imG[320*240];
// 存放两种颜色手指在连续T帧中的位置
int R[3][T] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int G[3][T] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void main(void)
{

	int mode=0;
	int count = 0;
	int delays = 0;
	unsigned char handstate;
	unsigned char message = '0';
	/* make sure we call adi_sec_init() */
	adi_initComponents();


	Reset_TWI0();
	TWI0_MasterMode_Write( CMOSI2CADD>>1, MY_CMOSConfig, MY_SETTINGNUM, 2);
	InitPPI0DMA();
    InitEPPI0();
    opencom();

	while(1)
	{
//		clock_t start,finish;
//		double duration;
//		start = clock();

		delays = (delays>0)?(delays-1):0;

		EnableEPPI0_DMA();
		while((*pREG_DMA14_STAT & 0x700) != 0)
		{
			asm("nop;");
		}
		DisableEPPI0_DMA();		//关闭DMA，CPU恢复对总线和内存的控制权

		unsigned char *imAddrstart = *pREG_DMA14_ADDRSTART;
//		unsigned char mask[25] = {255,255,255,255,255,255,255,255,255,255,255,255,255
//								,255,255,255,255,255,255,255,255,255,255,255,255,};
		unsigned char mask1[9] = {255,255,255,255,255,255,255,255,255};
		unsigned char mask2[9] = {0,255,0,255,255,255,0,255,0};



//		finger_detect(imAddrstart,imR,0x55,0x80,0xA0,0xDF);
//		finger_detect(imAddrstart,imG,0x90,0xA5,0x50,0x6F);		//戴标记手指
		finger_detect(imAddrstart,imskin,0x65,0x95,0x85,0x9A);		//肤色检测
		//finger_detect(imAddrstart,imskin,0x65,0x90,0x85,0x9A);		//肤色检测
		imerode(imR,imskin,mask2);
		imdilate(imskin,imR,mask1);
		get_center(imskin,R);
//		get_center(imG,G);

		if(delays == 0)
		{
			//gesture_rec(R,G,&message);
			gesture_rec1(R,&message);
			if (message!='0')
			{
				serial(&message);
				delays = 10;
			}
		}

//		finish = clock();
//		duration = (double)(finish - start) / CLOCKS_PER_SEC;
//		printf( "%f seconds\n", duration );

//		if(message!='0')
//		{
//			while(1)
//			{
//				asm("nop;");
//			}
//		}


//		getgrayscale(imAddrstart);
//		yuv422torgb565(imAddrstart);
//		QDTFT_Test_Demo(imAddrstart);
	}

	closecom();
}

void getgrayscale(unsigned char *data)		//将采集到的yuv格式的帧中的y分量提取出来并降采样得到320*240的图片
{
	int i,j,p;
	for (int k = 1;k<=76800;k++)
	{
//		i = (k%320==0) ? (k/320) : (k/320+1);
//		j = k % 320;
		j = (k-1) % 320 + 1;
		i = (j==320) ? (k/320) : (k/320+1);
		i = i * 2 - 1;
		j = j * 2 - 1;

		p = 2*((i-1)*640 + j - 1);
		data[k-1] = data[p];

	}
}

void yuv422torgb565(unsigned char *imAddrstart)
{
	unsigned char y,cb,cr,r,g,b;
	int i,j,p,k;
	for (k = 1;k <= 76800; k++)
	{
//		i = (k%320==0) ? (k/320) : (k/320+1);
//		j = k % 320;
		j = (k-1) % 320 + 1;
		i = (j==320) ? (k/320) : (k/320+1);
		i = i * 2 - 1;
		j = j * 2 - 1;
		p = 2*((i-1)*640 + j - 1);

		y = imAddrstart[p];
		if (p%4 == 0)
		{
			cb = imAddrstart[p+1];
			cr = imAddrstart[p+3];
		}
		else
		{
			cb = imAddrstart[p-1];
			cr = imAddrstart[p+1];
		}


//		r = y + (351*(cr-128))>>8;
//		g = y - (179*(cr-128)+86*(cb-128))>>8;
//		b = y + (443*(cb-128))>>8;
		r = (unsigned char)(y + 1.371*(cr-128));
		g = (unsigned char)(y - 0.698*(cr-128) - 0.336*(cb-128));
		b = (unsigned char)(y + 1.732*(cb-128));


		imAddrstart[2*k-1] = ((r>>3)<<3) + (g>>5);	//高8位
		imAddrstart[2*(k-1)] = (b>>3) + (g<<5);
	}

}
