/* --------------------------Includes ---------------------------------------------*/
#include "define.h"
#include "Lcd_Driver.h"
#include "delay.h"
//#include "Picture.h"
#include "QDTFT_demo.h"

void QDTFT_Test_Demo(unsigned char *data)
{
	Lcd_Init();
	Lcd_WriteBuffer(data);
	delay_ms(1500);
}
