/*
 * sub_functions.c
 *
 *  Created on: 2015-4-25
 *      Author: Zou L.G
 */
#include "stdio.h"
#include "delay.h"

#define T 5
// 肤色对应的下面四个值为77 127 133 173
//#define cbmin 0x50
//#define cbmax 0x80
//#define crmin 0xA0
//#define crmax 0xDF


void finger_detect(unsigned char *imAddrstart,unsigned char *imresult,unsigned char cbmin
					,unsigned char cbmax,unsigned char crmin,unsigned char crmax)
{
	int i,j,p;
	for (int k = 1;k <= 160*120;k++)
	{
		j = (k-1) % 160 + 1;
		i = (j==160) ? (k/160) : (k/160+1);
		i = i * 4 - 3;
		j = j * 4 - 3;
		p = 2*((i-1)*640 + j - 1);

		if (p%4 == 0)
			imresult[k-1] = 255*(((imAddrstart[p+1]<=cbmax)&&(imAddrstart[p+1]>=cbmin)
							   && (imAddrstart[p+3]<=crmax)&&(imAddrstart[p+3]>=crmin)));
		else
			imresult[k-1] = 255*(((imAddrstart[p-1]<=cbmax)&&(imAddrstart[p-1]>=cbmin)
							   && (imAddrstart[p+1]<=crmax)&&(imAddrstart[p+1]>=crmin)));

	}
}

void imdilate(unsigned char *imresult,unsigned char *imAddrstart,unsigned char *mask)
{
	short imwidth = 320;
	short imheight = 240;
	short maskwidth= 3;
	short maskheight = 3;
	signed short x,y,xx,yy,xrange,yrange,dx,dy;
	xrange = maskwidth/2;
	yrange = maskheight/2;
	int p,q;
	for (int k = 1;k <= imwidth*imheight;k++)
	{
		imresult[k-1] = 0;			//清空图像中间结果以免上次的结果对这次采集造成影响
		if (imAddrstart[k-1]>128)
		{
			y = (k-1) % imwidth + 1;
			x = (y==320) ? (k/imwidth) : (k/imwidth+1);
			//imAddrstart[k] = mask[maskwidth*maskheight/2+1];
			for (signed short i = -xrange;i <= xrange;i++)
				for (signed short j = -yrange;j <= yrange;j++)
				{
					xx = x + i;
					yy = y + j;
					if ((xx>0)&&((yy>0)))
					{
						p = (xx-1)*imwidth+yy-1;
						q = (i+xrange)*maskwidth+j+yrange;
						imresult[p] = (imresult[p]>128) ? imresult[p] : mask[q];
					}
				}

		}
	}
}

void imerode(unsigned char *imresult,unsigned char *imAddrstart,unsigned char *mask)
{
	short imwidth = 320;
	short imheight = 240;
	short maskwidth= 3;
	short maskheight = 3;
	signed short x,y,xx,yy,xrange,yrange,dx,dy;
	xrange = maskwidth/2;
	yrange = maskheight/2;
	int p,q;
	bool flag;
	for (int k = 1;k <= imwidth*imheight;k++)
	{
		imresult[k-1] = 0;
		if (imAddrstart[k-1]>128)
		{
			y = (k-1) % imwidth + 1;
			x = (y==imwidth) ? (k/imwidth) : (k/imwidth+1);
			//imAddrstart[k] = mask[maskwidth*maskheight/2+1];
			flag = true;
			for (signed short i = -xrange;i <= xrange;i++)
				for (signed short j = -yrange;j <= yrange;j++)
				{
					xx = x + i;
					yy = y + j;
					if ((xx>0)&&((yy>0)))
					{
						q = (i+xrange)*maskwidth+j+yrange;
						p = (xx-1)*imwidth+yy-1;
						if (mask[q]>128 & imAddrstart[p]<128)
						{
							flag = false;
							break;
						}
					}
				}
			imresult[k-1] = flag ? 255 : 0;

		}
	}
}

void get_center(unsigned char *imbinary,int position[][T])
{
	short x,y,i,j;
	int k,count = 0,xsum = 0,ysum = 0;
	for (k = 1;k <= 160*120;k++)
	{
		if (imbinary[k] > 128)
		{
			j = (k-1) % 160 + 1;
			i = (y==160) ? (k/160) : (k/160+1);
			count++;
			xsum+=i;
			ysum+=j;
		}
	}
	if (count >= 500)
	{
		x = xsum / count;
		y = ysum / count;
	}
	else
	{
		x = 0;
		y = 0;
		count = 0;
	}

	for (k = 2;k <= T;k++)		//存放中心坐标的buffer全部左移，以便存放新的中心坐标
	{
		position[0][k-2] = position[0][k-1];
		position[1][k-2] = position[1][k-1];
		position[2][k-2] = position[2][k-1];
	};

	position[0][T-1] = x;
	position[1][T-1] = y;
	position[2][T-1] = count;

}

short isincreasing(int position[T])
{
	// 递增（右，下） = 1，递减（左，上） = 2，否则返回 0

	unsigned char c1=0,c2=0;
	for (short i = 1;i <= T-1;i++)
	{
		if (position[i]>position[i-1])
			c1++;
		else if (position[i]<position[i-1])
			c2++;
		else
			return 0;
	}

	if (c1==(T-1))
		return 1;
	else if(c2==(T-1))
		return 2;
	else
		return 0;
}

short isamplify(int position1[][T],int position2[][T])
{
	// 放大 = 1，缩小 = 2，否则为0
	int d[T];
	for (short i = 1;i<=T;i++)		//计算两个指尖距离存放在d[T]中
	{
		if (position1[0][i-1]!=0 && position2[0][i-1]!=0 && position1[1][i-1]!=0 && position2[1][i-1]!=0)
			d[i-1] = (position1[0][i-1]-position2[0][i-1])*(position1[0][i-1]-position2[0][i-1])
					+(position1[1][i-1]-position2[1][i-1])*(position1[1][i-1]-position2[1][i-1]);
		else
			return 0;
	}

	switch(isincreasing(d))
	{
		case 1:
			if ((d[T-1]-d[0])>100)	//设置一个放大操作手指的最小动作幅度
				return 1;
			else
				return 0;
			break;
		case 2:
			if ((d[0]-d[T-1])>100)
				return 2;
			else
				return 0;
			break;
		default:
			return 0;
	}
}

void gesture_rec1(int position[][T],unsigned char *message)
{
	// message[0]中存放的，'0'表示什么不是手势，'1'表示左划，'2'表示右划，'3'表示放大，'4'表示缩小
	short f1,f2,f3;
	bool fr;
	int temp1[T];
	int temp2[T];

	fr = checkvalid(position);			//检查位置坐标中是否存在缺失的帧

	for (short i = 1;i <= T;i++)
	{
		temp1[i-1] = position[1][i-1];
		temp2[i-1] = position[2][i-1];
	}

	if (fr == false)
		*message = '0';				//存在缺失帧，视为无效操作
	else
	{
		if ((temp1[0]-temp1[T-1])>50 || (temp1[T-1]-temp1[0])>50)
		{
			f1 = isincreasing(temp1);
			switch(f1)
			{
				case 1: *message = '2';break;
				case 2: *message = '1';break;
				default: *message = '0';break;
			}
		}
		else if ((temp2[0]-temp2[T-1])>400 || (temp2[T-1]-temp2[0])>400)
		{
			f2 = isincreasing(temp2);
			switch(f2)
			{
				case 1: *message = '3';break;
				case 2: *message = '4';break;
				default: *message = '0';break;
			}
		}
		else
			*message = '0';
	}


}

void gesture_rec(int position1[][T],int position2[][T],unsigned char *message)
{
	// message[0]中存放的，'0'表示什么不是手势，'1'表示左划，'2'表示右划，'3'表示放大，'4'表示缩小
	short f1,f2,f3;
	bool fr,fg;
	int temp1[T];
	int temp2[T];

	fr = checkvalid(position1);			//检查位置坐标中是否存在缺失的帧
	fg = checkvalid(position2);

	for (short i = 1;i <= T;i++)
	{
		temp1[i-1] = position1[1][i-1];
		temp2[i-1] = position2[1][i-1];
	}

	if (fr == false && fg == false)
		*message = '0';				//两个手指都存在缺失帧，视为无效操作
	else if(fr == true && fg == false)/* && ((position1[1][0]>position1[1][T-1]>150)?(position1[1][0]-position1[1][T-1]>150)
											:(position1[1][T-1]-position1[1][0]>150)))*/
	{
		f1 = isincreasing(temp1);
		if (f1 == 1)
			*message = '2';
		else if (f1 == 2)
			*message = '1';
		else
			*message = '0';
	}
	else if (fr == false && fg == true)/* && ((position2[1][0]>position2[1][T-1]>150)?(position2[1][0]-position2[1][T-1]>150)
											:(position2[1][T-1]-position2[1][0]>150)))*/
	{
		f2 = isincreasing(temp2);
		if (f2 == 1)
			*message = '2';
		else if (f2 == 2)
			*message = '1';
		else
			*message = '0';
	}
	else
	{
		f3 = isamplify(position1,position2);
		if (f3 == 0)
		{
			f1 = isincreasing(temp1);
			f2 = isincreasing(temp2);
			if (f1 == 1 && f2 == 1)
				*message = '2';
			else if(f1 == 2 && f2 == 2)
				*message = '1';
			else
				*message = '0';
		}
		else
			*message = (f3 == 1)?'3':'4';
	}

}

bool checkvalid(unsigned int RG[][T])
{
	// 返回1表示R,G中都不为0，是合法的
	bool flag = true;
	for (short i = 1;i <= T;i++)
	{
		flag = flag && (RG[0][i-1]!=0 && RG[1][i-1]!=0);
	}
	return flag;
}
