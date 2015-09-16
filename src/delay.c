#include "define.h"
#include "delay.h" 
#include <time.h>
#include <sysreg.h>

void delay_ms(u16 ms)
{
	   uint32_t delay = (int32_t) ((long long) ms * (CLOCKS_PER_SEC / 5000));
	   uint32_t start = __builtin_sysreg_read(reg_CYCLES);

	   /* Spin until the delay expires. */
	   while ((__builtin_sysreg_read(reg_CYCLES) - start) < delay)
	   {
	   	/* do nothing */
	   }
}   





































