/*************************************************************************
	> File Name: jlsdecode.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年07月05日 星期三 09时36分09秒
 ************************************************************************/
#ifndef JLSDECODE 
#define JLSDECODE 
#include<iostream>
using namespace std;
#include "jlsencode.h"


typedef struct {
    unsigned char  bitmask;
    unsigned char  byte;
    unsigned char *pbuf;
    unsigned char *pbuf_base;
} BitReader_t;
//
//
BitReader_t initBitReader (unsigned char *pbuf) {
    BitReader_t br = {0x80, pbuf[0], pbuf, pbuf};
    return br;
}
//
//
int getBitReaderLength (BitReader_t *pbr) {
   return pbr->pbuf - pbr->pbuf_base;
}
//
//
//void writeValue (BitWriter_t *pbw, int value, int byte_cnt) {
//    int i = byte_cnt * 8;
//    for (i-=8; i>=0; i-=8) {
//        pbw->pbuf[0] = (value >> i) & 0xFF;
//        pbw->pbuf ++;
//    }
//}
//
//
void readBit (BitReader_t *pbr, bool * bit ) {
	unsigned char localbyte = pbr->byte & pbr->bitmask;
    pbr->bitmask >>= 1;
	if(localbyte != 0 ) {
		*bit = 1;
	}else *bit = 0;
	//printf("%x %x %d\n",pbr->byte,pbr->bitmask,*bit);
	//pbr->byte = pbr->pbuf[0];
    if (pbr->bitmask == 0) {
        //pbw->pbuf[0] = pbw->byte;
        pbr->pbuf ++;
        pbr->bitmask = 0x80;
		pbr->byte = pbr->pbuf[0];
        //if (pbw->byte == 0xFF)
        //    pbw->bitmask >>= 1;
	//	printf("len:%d\n",pbr->pbuf-pbr->pbuf_base);
	}   
	

}
//
//
void readBits (BitReader_t *pbr, int *bits, int n) {
	*bits = 0;
	for (n--; n>=0; n--){
		bool bit= 0 ;
		readBit(pbr, &bit);
		*bits = ((*bits) << 1) + bit ; 
	}
}
//
//
void flushRBits (BitReader_t *pbw) {
    if (pbw->bitmask < 0x80) {
        //pbw->pbuf[0] = pbw->byte;
        pbw->pbuf ++;
        pbw->bitmask = 0x80;
        pbw->byte = pbw->pbuf[0];
    }
}
//
//
void GolombDecoding (BitReader_t *pbr, int qbpp, int limit, int *val, int k) {
	int zeronumber = 0; 
	bool localbool; 
	readBit(pbr,&localbool);
	while (localbool ==0){
		zeronumber ++ ;
		readBit(pbr,&localbool);
	}
		//readBit(pbr,&localbool);
	//printf("number:%d  limit:%d k:%d\n",zeronumber,limit-qbpp-1,k);
	if(zeronumber < (limit - qbpp -1)) {
		int finalval = zeronumber ; 
		finalval = finalval << k ;
		int lowkbits = 0;
		readBits(pbr, &lowkbits,k);
		//printf("lowbits:%d\n",lowkbits);
		finalval = finalval + lowkbits;
		*val = finalval; 
	}else {
		int finalval = 0;
		readBits(pbr, &finalval,qbpp );
		//val -1 ;
		*val = finalval+1 ;
	}
}
//
#define PRINTLOG 0

void RegularMODEDecoding(int near,int xsize,int ysize,int RANGE,int limit,int qbpp,int t1,int t2,int t3,int D1,int D2,int D3,BitReader_t *pbr,unsigned char* localimg,int *RUNindex,int *x,int *y,int *Ra ,int *Rb, int *Rc, int *Rd, int *Ix, int *A, int *B,int *C, int *N,int * Nn, bool * EOLine) {
	int SIGN ,Rx,Px; 
	int Q = getQ(near,t1,t2,t3,*Ra,*Rb,*Rc,*Rd,&SIGN); 
	//if(Q > 366) printf("something wrong\n");
	Px = predict(*Ra,*Rb,*Rc) + SIGN * C[Q];
	if(Px > CONFIG_MAXVAL) {
		Px = CONFIG_MAXVAL ;
	}else if (Px < 0) {
		Px = 0 ;
	}


	int k;
	int MErrval ;
	k = LG(Q,N,A);
	GolombDecoding (pbr, qbpp, limit,&MErrval, k);
	//MErrval = MErrval + 1;
	int Errval ;
	if((near==0)&&(k==0)&&(2*B[Q]<=-N[Q]))
	{
		if((MErrval&1)==1)
			Errval=(MErrval-1)>>1;
		else
			Errval=(-(MErrval>>1))-1;
	}
	else{
		if((MErrval&1)==0)
			Errval=((MErrval)>>1);
		else
			Errval=-((MErrval+1)>>1);
	}	
#if PRINTLOG
	printf("E:%d M:%d Px:%d\n",Errval,MErrval,Px);
#endif
	//updating 
	B[Q]=B[Q]+Errval*(2*near+1);
	A[Q]=A[Q]+ABS(Errval);
	if(N[Q]==RESET)
	{
		A[Q]=A[Q]>>1;
		if(B[Q] >= 0) {
			B[Q] = B[Q] >> 1;
		}else {
			B[Q] = -((1-B[Q])>>1);
		}
		N[Q]=N[Q]>>1;
	}
	N[Q]=N[Q]+1;/*N[Q]指此种上下文出现的次数，最多为64次*/
//	if(N[Q] == 64) {
//		printf("error Q:%d N:%d A:%d\n",Q,N[Q],A[Q]);
//	}
//	Nt[Q]=Nt[Q]+1;

	Errval = Errval * (2*near+1);
	if(SIGN == -1)  Errval = -Errval;
	//Rx = Errval + Px;
	Rx=(Errval+Px)%(RANGE*(2*near+1));
#if PRINTLOG
	printf("Rx:%d Errval:%d Px:%d range:%d\n",Rx,Errval,Px,RANGE);
#endif
	if(Rx<(-near)) {
		Rx = Rx + RANGE*(2*near+1);
	}else if(Rx > (CONFIG_MAXVAL + near)) {
		Rx = Rx - RANGE*(2*near+1);
	}
	if(Rx < 0) {
		Rx = 0; 
	}else if(Rx > CONFIG_MAXVAL) {
		Rx =CONFIG_MAXVAL ;
	}
	//printf("E:%d M:%d Px:%d sign:%d\n",Errval,MErrval,Px,SIGN);

	GET2D(localimg,xsize,*y,*x) = Rx ;
	//printf("x:%d y:%d Rx%d\n",*x,*y,Rx);
	if(B[Q]<=-N[Q]){
		B[Q]=B[Q]+N[Q];
		if(C[Q]>MIN_C)
			C[Q]=C[Q]-1;
		if(B[Q]<=-N[Q])
			B[Q]=-N[Q]+1;
	}
	else if(B[Q]>0){
		B[Q]=B[Q]-N[Q];
		if(C[Q]<MAX_C)
			C[Q]=C[Q]+1;
		if(B[Q]>0)
			B[Q]=0;
	}

}
void RUNMODEDecoding(int near,int xsize,int ysize,int RANGE,int limit,int qbpp,BitReader_t *pbr,unsigned char* localimg,int *RUNindex,int *x,int *y,int *Ra ,int *Rb, int *Rc, int *Rd, int *Ix, int *A, int *B,int *C, int *N,int * Nn, bool * EOLine) {
	int Rx , Px ; 
	int RItype ;
	int Errval ;
	int SIGN ;
	int RUNval = *Ra ;
	int RUNcnt = 0  ;
	//printf("Ra:%d\n",*Ra);
	//Read a bit from the bit stream 
	bool localbit = 0; 
	readBit(pbr,&localbit); 
	// bit == 1 
	//if( localbit == 1) {
	while( localbit == 1) {
		int count = 0; 
		while (count < (1<<J[*RUNindex])) {
			count++;
			GET2D(localimg,xsize,*y,*x) = RUNval ;
			//printf("%d %d %d %d %dh\n",*x,*y,RUNval,*EOLine,(1<<J[*RUNindex]));
			//printf("count:%d j:%d Ra:%d x:%d y:%d\n",count,(1<<J[*RUNindex]),RUNval,*x,*y);
			//if(*EOLine == 1) {
			////	printf("aaa\n");
			//	break;
//
			//}
			if(*EOLine == 1) 
			{
				//printf("%d %d %d %d\n",*x,*y,RUNval,*EOLine);
				//printf("bbbb\n");
				break;
			}
			if(count < 1<<J[*RUNindex])
			*x = *x + 1;
			GetNextSample(localimg,xsize, *y, *x, Ra, Rb, Rc, Rd , EOLine) ;
			//if(*EOLine == 1) {
			//	//printf("%d %d %d %d\n",*x,*y,RUNval,*EOLine);
			//	break;
			//}
			
		}
		//printf("count:%d\n",count);
		if( (count == (1<< J[*RUNindex]) ) && (*RUNindex < 31)) {
			*RUNindex = *RUNindex +1; 
		}
		if (*EOLine == 0) {
			//printf("%d %d %d %d\n",*x,*y,RUNval,*EOLine);
			readBit(pbr,&localbit);
			//printf("%d %d %d %d %d\n",*x,*y,RUNval,*EOLine,localbit);
			//if(localbit == 1) {
				*x = *x + 1;
				GetNextSample(localimg,xsize, *y, *x, Ra, Rb, Rc, Rd , EOLine) ;
			
			//}
		}else {
			//*x = *x -1 ;
			return ;
		}
	}
	int originrunindex = *RUNindex ; 
	//可能要用减之前的，现保存
	if (localbit == 0) {
		int number = 0; 
		readBits(pbr,&number,J[*RUNindex]); 
		//printf("number %d\n",number);
		int count = 0;
		while(count < number){
			count ++ ;
			GET2D(localimg,xsize,*y,*x) = RUNval; 
		//	printf("x:%d y:%d Ra%d\n",*x,*y,RUNval);
			//*x = *x + 1;
			//printf("x:%d y:%d\n",*x,*y);
			//if(count == number) break;
			if(*x == xsize-1) {
				//if(count != number) {
				//	//printf("x:%d y:%d count:%d number:%d\n",*x,*y,count,number);
				////	exit(0);
				//}
				*x = 0;
				*y = *y + 1;
			} else {
				*x = *x + 1 ;
			}
			GetNextSample(localimg,xsize,*y,*x,Ra,Rb,Rc,Rd,EOLine);  
		}
		if (*RUNindex > 0 ) {
			*RUNindex = *RUNindex - 1; 
		}
	
	//GetNextSample(localimg,xsize,*y,*x,Ra,Rb,Rc,Rd,EOLine);  
	int EMErrval = 0 ;
	int glimit = limit- J[*RUNindex] -1;
	if(ABS(*Ra - *Rb) <= near) {
		RItype = 1;
	} else {
		RItype = 0; 
	}

	if(RItype == 1) {
		Px = *Ra ;
	}else {
		Px = *Rb ; 
	}
	if((RItype == 0) &&(*Ra > *Rb)) {
		SIGN  = -1; 
	}else {
		SIGN = 1; 
	}
	//printf("error Q:%d N:%d A:%d\n",366,N[366],A[366]);
	int AQ ,TEMP,k, map; 
	if(RItype == 0) {
		TEMP = A[365];
	}else {
		TEMP = A[366] + (N[366] >> 1) ;
	}
	int Q = RItype + 365;
	AQ=A[Q];
	A[Q] = TEMP;
	//printf("before count k \n");
	k = LG(Q,N,A);
	GolombDecoding (pbr, qbpp, glimit,&EMErrval, k);
	//EMErrval = EMErrval + 1;

	 if(((EMErrval&1)==0)&&(RItype==1))
		 map=1;
	 else if(((EMErrval&1)==1)&&(RItype==1))
		 map=0;
	 else if(((EMErrval&1)==1)&&(RItype==0))
		 map=1;
	 else if(((EMErrval&1)==0)&&(RItype==0))
		 map=0;
	//(map==0&&k!=0)||
	 if((map==0&&!(k==0&&2*Nn[Q-365]<N[Q]))||(map==1&&k==0&&2*Nn[Q-365]<N[Q]))
         Errval=(EMErrval+RItype+map)>>1;
	 else
         Errval=-((EMErrval+RItype+map)>>1); 
	 //printf("Errval:%d \n",Errval);
	 int flag=Errval;

	 Errval=Errval*(2*NEAR+1);
	 if(SIGN==-1)
		 Errval=-Errval;
		 
	 Rx=(Errval+Px)%(RANGE*(2*near+1));
	 
	 if(Rx<(-NEAR))
		 Rx=Rx+RANGE*(2*NEAR+1);
	 if(Rx>(CONFIG_MAXVAL+NEAR))
		 Rx=Rx-RANGE*(2*NEAR+1);
	 if(Rx<0)
		 Rx=0;
	 else if(Rx>CONFIG_MAXVAL)
		 Rx=CONFIG_MAXVAL;
	 //printf("x:%d y:%d Rx%d Px:%d\n",*x,*y,Rx,Px);
	 GET2D(localimg,xsize,*y,*x)= Rx;
//   **********************************
//   *           更新其变量           *
//   **********************************         
	 if(flag<0)
		 Nn[Q-365]=Nn[Q-365]+1;
         A[Q]=AQ;
         A[Q]=A[Q]+((EMErrval+1+RItype)>>1);
     if(N[Q]==RESET)
	 {
	 A[Q]=A[Q]>>1;
	 N[Q]=N[Q]>>1;
	 Nn[Q-365]=Nn[Q-365]>>1;
	 }
      //N[Q]=N[Q]+1;
    
	}
		//printf("error Q:%d N:%d A:%d\n",Q,N[Q],A[Q]);
}

 

int jlsdecode(int near,int xsize,int ysize, unsigned char * inputstream,unsigned char * img,int num = 1){
	int MAXVAL = CONFIG_MAXVAL ;
	int RANGE, t1,t2,t3,quant,bpp,qbpp,limit,a_init ;
	setParameters (MAXVAL,near,&RANGE, &t1, &t2, &t3,&quant, &bpp, &qbpp, &limit, &a_init);
	BitReader_t pbw = initBitReader(inputstream);
	
	//int len = 0;
	for(int ind = 0; ind < num ; ind++){
	int Ra, Rb,Rc, Rd ;
	//int RUNindex = 0 ;
	int Ix ,D1 ,D2 , D3 ; 
	bool EOLine = 0; 
	int RUNindex = 0 ;
	int A[367];
	int B[365];
	int C[365];
	int N[367];
	int Nn[2]; 
	for (int i = 0 ; i < 365; i ++){
		A[i] = a_init ;
		N[i] = 1 ;
		B[i] = 0;
		C[i] = 0; 
	}
	A[365] = a_init ; A[366] = a_init ;
	N[365] = 1		; N[366] = 1	  ;
	Nn[0]  = 0		; Nn[1]  = 0	 ;
	unsigned char * localimg = &img[ind*xsize*ysize];
	for (int y = 0 ; y < ysize ; y++) {
		for (int x = 0;x < xsize; x ++ ) {
			GetNextSample(localimg,xsize, y, x, &Ra , &Rb , &Rc , &Rd , &EOLine) ;
			int D1 = Ra - Rb ; 
			int D2 = Rb - Rc ;
			int D3 = Rc - Ra ; 
			//Ix = GET2D(localimg,xsize,y,x );
			//printf("x : %d y:%d\n",x,y);
			//printf("runindex %d\n",RUNindex);
			if( (ABS(D1) <= near) && ( ABS(D2) <= near) && (ABS(D3) <= near)) {
				// RUNMODEProcessing 
				//printf("x : %d y:%d\n",x,y);
				//printf("Ra:%d Rc:%d Rb:%d Rd:%d Ix:%d\n", Ra,Rc,Rb,Rd,Ix);
				RUNMODEDecoding(near,xsize,ysize, RANGE,limit,qbpp,&pbw,localimg,&RUNindex,&x,&y,&Ra ,&Rb, &Rc, &Rd, &Ix, A,B,C, N,Nn, &EOLine); 
				//printf("jjjj\n");
			}
			else {
				//printf("2\n");
				//printf("x : %d y:%d\n",x,y);
				//RegularModeProcessing 
				RegularMODEDecoding(near,xsize,ysize,RANGE,limit,qbpp,t1,t2, t3, D1,D2,D3, &pbw,localimg,&RUNindex,&x,&y,&Ra ,&Rb, &Rc, &Rd, &Ix, A, B,C, N,Nn, &EOLine);
				//printf("hhhh\n");
			
			}
			//if(N[366]>64 || A[366]>1000) {
			//	exit(0);
			//}
		}
	}
	flushRBits(&pbw);
	//int len0  =   getBitReaderLength(&pbw);
	//printf("len : %d num:%d \n",len0,len0);
	}
	//flushBits(&pbw);
	int len = getBitReaderLength(&pbw);
	return len ;//pbw->pbuf - pbw->pbuf_base;
} 
int jpeglsdecompress(const unsigned char * code , unsigned char *data , int size1){
	unsigned char res [ 1024];
	unsigned char *input = (unsigned char *) code;
	int originsize = 0 ;
	//printf("hhhh\n");
	if(size1 == 8 * 8 * 4){
		//printf("originsize : %d\n", size1);
		//cout << W << " "<<H << endl;
		memcpy(res,code,size1);
		originsize = size1 ;
	} else {
		originsize=256;	
		jlsdecode(0,8,8, input,res,4);
		//originsize=256;
		//printf("originsize : %d\n", originsize);
	}
	//printf("originsize : %d\n", originsize);
	//int originsize  = rleDecode(code,sizei,res);	
	//按照 b g r a 恢复 
	//根据压缩过程可以知道 code是按顺序存放bgra压缩后的数据
	//解压后也是按顺序存放bgra 压缩前的数据，只要按照顺序恢复即可 。
	int onesize = originsize / 4; 
	for (int i = 0 ; i < onesize; i++  ){
		data[i*4]   = res[i];
		data[i*4+1] = res[onesize+i];
		data[i*4+2] = res[onesize*2+i];
		data[i*4+3] = res[onesize*3+i];
	}
	return 0 ;

}



#endif 
