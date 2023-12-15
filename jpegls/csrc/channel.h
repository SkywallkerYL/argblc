#ifndef CHANNEL_H
#define CHANNEL_H  
/*************************************************************************
	> File Name: channel.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月21日 星期日 14时26分51秒
 ************************************************************************/
#include "common.h"
#include<iostream> 
using namespace std;

static default_random_engine channel(static_cast<unsigned>(time(NULL)));
double RandomGen(double sigma)
{
  normal_distribution<double> nd(0, sigma);
  return nd(channel);
}
/************Yn Initial Table*************/
int LLrInitial(double sigma)
{
  //all zero code
  double llr_init = 1.0 + RandomGen(sigma);
#if LLR_INIT_TABLE == 1
  if (llr_init < 0)
    return -1;
  else
    return 1;
#elif LLR_INIT_TABLE == 2
  if(llr_init < -0.3144384671){
    return -4;
  }else if (llr_init < 0){
    return -1;
  }else if (llr_init < 0.3144384671){
    return 1;
  }else{
    return 4;
  }
#elif LLR_INIT_TABLE == 3
		if (llr_init < -boundy7)		return -LLR_INIT4bit7;
		else if (llr_init < -boundy6) 	return -LLR_INIT4bit6;
		else if (llr_init < -boundy5) 	return -LLR_INIT4bit55;
		else if (llr_init < -boundy4) 	return -LLR_INIT4bit5;
		else if (llr_init < -boundy3) 	return -LLR_INIT4bit4;
		else if (llr_init < -boundy2) 	return -LLR_INIT4bit3;
		else if (llr_init < -boundy1) 	return -LLR_INIT4bit2;
		else if (llr_init < 0)        	return -LLR_INIT4bit1;
		else if (llr_init < boundy1)  	return LLR_INIT4bit1;
		else if (llr_init < boundy2)  	return LLR_INIT4bit2;
		else if (llr_init < boundy3)  	return LLR_INIT4bit3;
		else if (llr_init < boundy4)  	return LLR_INIT4bit4;
		else if (llr_init < boundy5)  	return LLR_INIT4bit5;
		else if (llr_init < boundy6)  	return LLR_INIT4bit55;
		else if (llr_init < boundy7)  	return LLR_INIT4bit6;
		else                          	return LLR_INIT4bit7;
#elif LLR_INIT_TABLE == 4
  if(llr_init < -0.496752656){
    return -7;
  }else if(llr_init < -0.13206355){
    return -4;
  }else if (llr_init < 0){
    return -1;
  }else if (llr_init < 0.13206355){
    return 1;
  }else if (llr_init < 0.496752656){
    return 4;
  }else{
    return 7;
  }
#elif LLR_INIT_TABLE == 5 
	double bound = sigma*sigma/2.0;

		/*******均匀量化*******/
	if 		(llr_init < -bound*6.5)		return -7	;
	else if (llr_init < -bound*5.5) 	return -6	;
	else if (llr_init < -bound*4.5) 	return -5	;
	else if (llr_init < -bound*3.5) 	return -4	;
	else if (llr_init < -bound*2.5) 	return -3	;
	else if (llr_init < -bound*1.5) 	return -2	;
	else if (llr_init < -bound*0.5) 	return -1	;
	else if (llr_init < 0)        		return -0	;
	else if (llr_init < bound*0.5)  	return 0	;
	else if (llr_init < bound*1.5)  	return 1	;
	else if (llr_init < bound*2.5)  	return 2	;
	else if (llr_init < bound*3.5)  	return 3	;
	else if (llr_init < bound*4.5)  	return 4	;
	else if (llr_init < bound*5.5)  	return 5	;
	else if (llr_init < bound*6.5)  	return 6	;
	else                          		return 7	;
#endif
}


#endif 
