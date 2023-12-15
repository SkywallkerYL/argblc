#ifndef ADDRH
#define ADDRH 
#include "common.h"
#include "testfunc.h"


//unsigned char data[16];
//unsigned char coded[160];


unsigned char data[] = {
		(unsigned char)131	,(unsigned char)0     ,(unsigned char)67    ,(unsigned char)198     ,
		(unsigned char)62	,(unsigned char)112   ,(unsigned char)148   ,(unsigned char)101     ,
		(unsigned char)8	,(unsigned char)52    ,(unsigned char)125   ,(unsigned char)93      ,
		(unsigned char)29	,(unsigned char)90    ,(unsigned char)96    ,(unsigned char)237 
	};
unsigned char coded[1600];

extern "C" void pmem_write(long long waddr, long long wdata,char wmask){
    //printf("addr:0x%016x data:0x%016x mask:0b%02b \n",waddr,wdata,wmask);
    //char c = wdata&0xff;
    //uint64_t write_data = wdata;
    coded[waddr] = (unsigned char)(wdata & 0xffull);
    //for (char i = 0; i < 8; i++)
    //{
        //判断mask的i位是否为1,从地到高。
        //if (wmask & (0x1<<i))
        //{
            ////默认对齐低位
            //coded[]
           //*(uint8_t *)((uint8_t *)vmem + waddr - FB_ADDR+i)= write_data&0xffull;
       // }
    //}
    
}
extern "C" void pmem_read(long long raddr, long long *rdata){
    //printf("addr:0x%016x data:0x%016x\n",waddr,wdata);
    *rdata = (long long) (unsigned char)data[raddr];
    //printf("addr:0x%016x data:%d read:%d\n",raddr,*rdata,data[raddr]);
}


#endif