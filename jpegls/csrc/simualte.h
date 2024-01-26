#ifndef SIMULATE_H
#define SIMULATE_H

/*************************************************************************
	> File Name: simualte.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月21日 星期日 14时32分21秒
 ************************************************************************/
#include "common.h"
#include<iostream>
using namespace std;


VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
//static VRiscvCpu* top;
void step_and_dump_wave(){
  top->eval();
  if(top->clock == 1)wavecount++;
#ifdef WAVE
  if(!TRACE_CONDITION(wavecount,WAVE_BEGIN,WAVE_END)) {return;}
  //if(top->clock == 1)wavecount++;
  //printf("pc:%x wavecount:%d\n",Pc_Fetch(),wavecount);
  contextp->timeInc(1);
  tfp->dump(contextp->time());
#endif
}
//初始化
void sim_init(){
#ifdef WAVE
  contextp = new VerilatedContext;
  //top = new VRiscvCpu;
  tfp = new VerilatedVcdC;
#endif
#if TESTMODULE == 1
  top = new Vjpeglsencodesimtop;
#elif TESTMODULE == 2
  top = new Vjpeglsdecodesimtop;
#elif TESTMODULE == 3 
  top = new Vtilecompresssimtop;
#elif TESTMODULE == 4 
  top = new Vtiledecompresssimtop; 
#elif TESTMODULE == 5
  top = new VcompressARGBtop; 
#elif TESTMODULE == 6 
  top = new VcompressARGBfiletop;
#elif TESTMODULE == 7 
  top = new VdecompressARGBfiletop; 
#elif TESTMODULE == 8
  top = new VJpegIP;
#elif TESTMODULE == 9
  top = new Vrsdecodertop ;
#elif TESTMODULE == 10 
  top = new VErrorbits;
#elif TESTMODULE == 11 
  top = new VrsdecodertopGauss;
#elif TESTMODULE == 12
  top = new Vrsdecoder2colGauss;
#endif 
#ifdef WAVE
  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("wave.vcd");
#endif 
}

void clockntimes(int n ){
	
	int temp = n;
	while (temp >= 1)
	{
		top->clock = 0;
		step_and_dump_wave();
		top->clock = 1;
		step_and_dump_wave();
		temp --;
	}
}

void reset(int n) {
	top->reset = 0b1;
	clockntimes(n);
	top->reset = 0b0;

}

void sim_exit(){
  step_and_dump_wave();
#ifdef WAVE
  tfp->close();
#endif
  delete top;
  delete contextp;
}


#endif
