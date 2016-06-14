#include "3052.h"
main(){
	P1.DDR=0xff;
	short x;
	long i;
	x=1;
	while(1){
		while(x<128){
			P1.DR.BYTE=x;
			for(i=0;i<=250000;i++);
			x=x*2;
		}
		while(x>1){
			P1.DR.BYTE=x;
			for(i=0;i<=250000;i++);
			x=x/2;
		}
	}
}
