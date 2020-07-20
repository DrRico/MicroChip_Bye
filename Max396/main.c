#include "reg52.h"

void DelayMS(int n)
{
	int k;
	while(n--)
	{
		for (k = 0; k < 75; k++);
	}
}


void mian(){
	int i;
	code int table[10] = {0x01,0x02,0x03,0x04,0x05,0x06};
	P0=0x61;
	while(1){
		for(i=0;i<16;i++){
			DelayMS(100);
			P0 = table[i];
		}
	}
}
