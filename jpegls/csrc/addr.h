#ifndef ADDRH
#define ADDRH 
#include "common.h"
#include "testfunc.h"


//unsigned char data[16];
//unsigned char coded[160];


unsigned char data[] = {
		(unsigned char)103	,(unsigned char)81    ,(unsigned char)41    ,(unsigned char)242     ,
		(unsigned char)124	,(unsigned char)27    ,(unsigned char)118   ,(unsigned char)51      ,
		(unsigned char)102	,(unsigned char)49    ,(unsigned char)37    ,(unsigned char)88     ,
		(unsigned char)171	,(unsigned char)155   ,(unsigned char)14    ,(unsigned char)33 
	};
unsigned char data1[16];
unsigned char tiledata[16*4];
unsigned char tilecode[640];
unsigned char tiledata1[16*4];
unsigned char coded[160];

extern "C" void pmem_write(long long waddr, long long wdata,char wmask){
    //printf("addr:0x%016x data:0x%016x mask:0b%02b \n",waddr,wdata,wmask);
    //char c = wdata&0xff;
    //uint64_t write_data = wdata;
    if(waddr >= 0x80000000){
        coded[waddr-0x80000000] = (unsigned char)(wdata & 0xffull);
    }else if (waddr < 0x10000000) {
        data1[waddr] =  (unsigned char)(wdata & 0xffull);
    }else if (waddr >= 0x70000000 && waddr < 0x80000000){
        tilecode[waddr-0x70000000] = (unsigned char)(wdata & 0xffull);
    }else if (waddr >= 0x60000000 && waddr < 0x70000000){
        tiledata1[waddr-0x60000000] = (unsigned char)(wdata & 0xffull);
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
    //printf("addr:0x%016x data:0x%016x\n",raddr,*rdata);
    if(raddr >= 0x80000000){
        *rdata = (long long) (unsigned char)coded[raddr-0x80000000];
    }else if (raddr < 0x10000000) {
        *rdata = (long long) (unsigned char)data[raddr];
    }else if (raddr >= 0x60000000 && raddr < 0x70000000){
        *rdata = (long long) (unsigned char)tiledata[raddr-0x60000000];
    }
    else if (raddr >= 0x70000000 && raddr < 0x80000000){
        *rdata = (long long) (unsigned char)tilecode[raddr-0x70000000];
    }
    
    //printf("addr:0x%016x data:%d read:%d\n",raddr,*rdata,data[raddr]);
}


#endif