#ifndef ADDRH
#define ADDRH 
#include "common.h"
#include "testfunc.h"


//unsigned char data[16];
//unsigned char coded[160];


unsigned char data[] = {
		(unsigned char)0	,(unsigned char)52    ,(unsigned char)199   ,(unsigned char)114     ,
		(unsigned char)11	,(unsigned char)239   ,(unsigned char)11    ,(unsigned char)134     ,
		(unsigned char)229	,(unsigned char)7     ,(unsigned char)116   ,(unsigned char)137     ,
		(unsigned char)148	,(unsigned char)68    ,(unsigned char)68    ,(unsigned char)115 
	};
unsigned char data1[16];
unsigned char coded[1600];

extern "C" void pmem_write(long long waddr, long long wdata,char wmask){
    //printf("addr:0x%016x data:0x%016x mask:0b%02b \n",waddr,wdata,wmask);
    //char c = wdata&0xff;
    //uint64_t write_data = wdata;
    if(waddr >= 0x80000000){
        coded[waddr-0x80000000] = (unsigned char)(wdata & 0xffull);
    }else {
        data1[waddr] =  (unsigned char)(wdata & 0xffull);
    }
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
    if(raddr >= 0x80000000){
        *rdata = (long long) (unsigned char)coded[raddr-0x80000000];
    }else{
        *rdata = (long long) (unsigned char)data[raddr];
    }
    
    //printf("addr:0x%016x data:%d read:%d\n",raddr,*rdata,data[raddr]);
}


#endif