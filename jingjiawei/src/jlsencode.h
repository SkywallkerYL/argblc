/*************************************************************************
	> File Name: jlsencode.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年07月05日 星期三 09时36分09秒
 ************************************************************************/
#ifndef JLSENCODE 
#define JLSENCODE 
#include<iostream>
using namespace std;
#define    ABS(x)               ( ((x) < 0) ? (-(x)) : (x) )                         // get absolute value
#define    MAX(x, y)            ( ((x)<(y)) ? (y) : (x) )                            // get the minimum value of x, y
#define    MIN(x, y)            ( ((x)<(y)) ? (x) : (y) )                            // get the maximum value of x, y
#define    CLIP(x, min, max)    ( MIN(MAX((x), (min)), (max)) )                      // clip x between min~max

#define    GET2D(ptr,xsz,y,x)   (*( (ptr) + (xsz)*(y) + (x) ))


#define		NEAR				0

#define    RESET            64

#define		CONFIG_MAXVAL       255

#define    X_SIZE			8
#define    Y_SIZE           8 
#define    MIN_C       -128
#define    MAX_C       127 
int J [] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,5,5,6,6,7,7,8,9,10,11,12,13,14,15};

// 获取一个数值的2的对数低，向上去整
int getceillog(int val) {
	int ans = 1; 
	while((1 << ans) < val) {
		ans ++;
	}
	return ans ;
}




void setParameters (int MAXVAL, int near, int *RANGE, int *t1, int *t2, int *t3, int *quant, int *bpp, int *qbpp, int *limit, int *a_init) {
    int tmp;
    //*alpha = 1 << bpp;
    tmp = (MIN(MAXVAL, 4096) + 127) / 256;
    *t1 =     tmp + 2 + 3 * near;
	//printf("t1:%d\n",);
    *t2 = 4 * tmp + 3 + 5 * near;
    *t3 = 17* tmp + 4 + 7 * near;
    *quant = 2 * near + 1;
    *RANGE = (MAXVAL + 2*near) / *quant+1;
	*bpp   = MAX(2,getceillog(MAXVAL+1));
    *qbpp  = getceillog(*RANGE);
    *limit = 2*(*bpp+MAX(8,*bpp));//4 * bpp - *qbpp - 1;
    *a_init = MAX((*RANGE+32)/64, 2);
	//printf("t1:%d t2:%d t3:%d quant:%d range:%d bpp:%d qbpp:%d limit:%d a_ini:%d\n",
	//*t1,*t2,*t3,*quant,*RANGE,*bpp,*qbpp,*limit,*a_init
	//);
}


int isNear (int near, int v1, int v2) {
    return (v1-v2 <= near) && (v2-v1 <= near);
}
void GetNextSample(unsigned char *img, int xsize, int i, int j, int *Ra , int *Rb , int * Rc , int* Rd , bool *EOLine) {
	*Ra = 0;
	*Rb = 0;
	*Rc = 0 ;
	*Rd = 0; 
	*EOLine = 0;
	if ( i > 0 ) {
		*Rb = GET2D(img,xsize,i-1,j);
		*Rd = *Rb ;
		if (j+1 < xsize) {
			*Rd =GET2D(img,xsize,i-1,j+1);
		}
		//else {
		//	printf("y %d x %d",i,j);
		//	*EOLine = 1; 
		//}
	}
	if ( j <= 0) {
		*Ra = *Rb ;
		if( i > 1) {
			*Rc = GET2D(img,xsize,i-2,j);
		}
	}else {
		*Ra = GET2D(img , xsize, i , j -1 );
		if(i > 0) {
			*Rc = GET2D(img , xsize, i-1,j-1);
		}
	}
	if(j == xsize - 1) {
		*EOLine = 1;
	}
}



int gradientQuantize (int val, int near, int t1, int t2, int t3) {
    int absval = ABS(val);
    int sign   = (val < 0);
    if (absval >=  t3) return sign ? -4 : 4;
    if (absval >=  t2) return sign ? -3 : 3;
    if (absval >=  t1) return sign ? -2 : 2;
    if (absval > near) return sign ? -1 : 1;
    return 0;
}


int getQ (int near, int t1, int t2, int t3, int a, int b, int c, int d ,int *sign) {
    int Q1 = gradientQuantize(d-b, near, t1, t2, t3);
    int Q2 = gradientQuantize(b-c, near, t1, t2, t3);
    int Q3 = gradientQuantize(c-a, near, t1, t2, t3);
	if((Q1 < 0 )||((Q1 ==0 )&& (Q2 < 0)) || ((Q1==0)&&(Q2==0) &&(Q3<0)))
		*sign = -1;
	else *sign = 1;
	Q1 = Q1 * (*sign);
	Q2 = Q2 * (*sign) ;
	Q3 = Q3 * (*sign) ;
    return 81*Q1 + 9*Q2 + Q3;
}


int predict (int a, int b, int c) {
    if      (c >= MAX(a, b))
        return MIN(a, b);
    else if (c <= MIN(a, b))
        return MAX(a, b);
    else
        return a + b - c;
}


void quantize (int near, int* errval) {
	if(*errval > 0) {
		*errval = (*errval + near)/ (2*near + 1) ;
	}else {
		*errval = -((near - *errval)/(2*near + 1)) ;
	}
	
}
void computeRx(int near ,int *Rx , int sign ,int Px, int * errval) {
	*Rx = Px + sign * (*errval) *(2*near + 1) ;
	if(*Rx < 0) 
		*Rx = 0; 
	else if (*Rx > CONFIG_MAXVAL) {
		*Rx = CONFIG_MAXVAL ;
	}
}


int ModRange (int RANGE,  int val) {
    if      (val < 0)
        val += RANGE;
    if (val >= (RANGE+1 ) / 2)
        val -= RANGE;
    return val;
}

int LG(int Q,int * N, int * A) {
	int k = 0;
	//printf("Q:%d N:%x A:%x\n",Q,N[Q],A[Q]);
	for (k = 0;((N[Q] << k) < A[Q]) ; k++);
	return k ;
}

int getK (int At, int Nt, int ritype) {
    int k = 0;
    if (ritype)
        At += (Nt >> 1);
    while ((Nt << k) < At)
        k ++;
    return k;
}




typedef struct {
    unsigned char  bitmask;
    unsigned char  byte;
    unsigned char *pbuf;
    unsigned char *pbuf_base;
} BitWriter_t;


BitWriter_t initBitWriter (unsigned char *pbuf) {
    BitWriter_t bw = {0x80, 0x00, pbuf, pbuf};
    return bw;
}


int getBitWriterLength (BitWriter_t *pbw) {
    return pbw->pbuf - pbw->pbuf_base;
}


void writeValue (BitWriter_t *pbw, int value, int byte_cnt) {
    int i = byte_cnt * 8;
    for (i-=8; i>=0; i-=8) {
		//printf("byte_cnt:%d i: %d\n",byte_cnt,i);
        pbw->pbuf[0] = (value >> i) & 0xFF;
        pbw->pbuf ++;
    }
}


void writeBit (BitWriter_t *pbw, int bit) {
	//printf("bit : %d\t ind:%x\n",bit,pbw->bitmask);
    if (bit)
        pbw->byte |= pbw->bitmask;
    pbw->bitmask >>= 1;
    if (pbw->bitmask == 0) {
        pbw->pbuf[0] = pbw->byte;
        pbw->pbuf ++;
		//printf("%x\n",pbw->byte);
        pbw->bitmask = 0x80;
        //if (pbw->byte == 0xFF)
        //    pbw->bitmask >>= 1;
        pbw->byte = 0x00;
    } 
}


void writeBits (BitWriter_t *pbw, int bits, int n) {
    //printf("bits:%x n: %d\n",bits,n);
	for (n--; n>=0; n--){
	//	printf("byte_cnt:%d i: %d\n",n,n);
		writeBit(pbw, (bits>>n)&1);
	}
        
}


void flushBits (BitWriter_t *pbw) {
    if (pbw->bitmask < 0x80) {
        pbw->pbuf[0] = pbw->byte;
        pbw->pbuf ++;
	//	printf("%x\n",pbw->byte);
        pbw->bitmask = 0x80;
        pbw->byte = 0x00;
    }
}


void GolombCoding (BitWriter_t *pbw, int qbpp, int limit, int val, int k) {

	if ((val>>k) < (limit-qbpp-1)) {
        writeBits(pbw, 0, val>>k);
        writeBit (pbw, 1);
        writeBits(pbw, val, k);
    } else {
        writeBits(pbw, 0, (limit-qbpp-1));
        writeBit (pbw, 1);
        writeBits(pbw, val-1, qbpp);
    }
}

void RegularMODEProcessing(int near,int xsize,int ysize,int RANGE,int limit,int qbpp,int t1,int t2,int t3,int D1,int D2,int D3,BitWriter_t *pbw,unsigned char* localimg,int *RUNindex,int *x,int *y,int *Ra ,int *Rb, int *Rc, int *Rd, int *Ix, int *A, int *B,int *C, int *N,int * Nn, bool * EOLine) {
	int SIGN ,Rx,Px; 
	int Q = getQ(near,t1,t2,t3,*Ra,*Rb,*Rc,*Rd,&SIGN); 
	//if(Q > 366) printf("something wrong\n");
	Px = predict(*Ra,*Rb,*Rc) + SIGN * C[Q];
	if(Px > CONFIG_MAXVAL) {
		Px = CONFIG_MAXVAL ;
	}else if (Px < 0) {
		Px = 0 ;
	}
	//printf("pre :%d Cq:%d\n",predict(*Ra,*Rb,*Rc) ,C[Q]);
	int Errval = *Ix - Px ;
	if(SIGN == -1) Errval = -Errval ;
	//Error quantization for near-lossless coding 
	quantize(near,&Errval) ;
	/***************************/
	//computeRx(near ,&Rx ,SIGN , Px, &Errval);
	Rx = *Ix;
	//GET2D(localimg,xsize,*y,*x) = Rx; 
	//printf("Errval %d \n",Errval);
	Errval = ModRange(RANGE,Errval);

	//Prediction error encoding 
	//Golomb coding variable compution  
	int k;
	int MErrval ;
	k = LG(Q,N,A);
	if((near==0)&&(k==0)&&(2*B[Q]<=-N[Q]))
	{
		if(Errval>=0)
			MErrval=2*Errval+1;
		else
			MErrval=-2*(Errval+1);
	}
	else{
		if(Errval>=0)
			MErrval=2*Errval;
		else
			MErrval=-2*Errval-1;
	}	
	//printf("Ra:%d Rb:%d Rc:%d Rd:%d\n",*Ra,*Rb,*Rc,*Rd);
	//printf("Q:%d Ix:%d Px:%d sign:%d\n",Q,*Ix,Px,SIGN);
	//printf("error Q:%d N:%d A:%d\n",Q,N[Q],A[Q]);
	//printf("Jout :%d k : %d Errval %d \n",J[*RUNindex],k,Errval);
	
	GolombCoding(pbw,qbpp,limit,MErrval,k); 


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
void RUNMODEProcessing(int near,int xsize,int ysize,int RANGE,int limit,int qbpp,BitWriter_t *pbw,unsigned char* localimg,int *RUNindex,int *x,int *y,int *Ra ,int *Rb, int *Rc, int *Rd, int *Ix, int *A, int *B,int *C, int *N,int * Nn, bool * EOLine) {
	int Rx , Px ; 
	int RItype ;
	int Errval ;
	int SIGN ;
	// Run-length determination for run mode      
	int RUNval = *Ra ;
	int RUNcnt = 0  ;
	//printf("runcnt%d runindex%d\n",RUNcnt,*RUNindex);
	while(ABS(*Ix - RUNval) <= near) {
		RUNcnt = RUNcnt + 1 ;
		Rx = RUNval ;
		//GET2D(localimg,xsize,*y,*x) = Rx ;
		//printf("%d %d %d\n",*x,*y,Rx);
		if(*EOLine == 1) {
			break;
		}else {	
			*x = *x + 1;
		//	printf("%d\n",*x);
		//	if(*x >= xsize) {
		//		printf("something wrong %d\n",*x);
		//		exit(0);
		//	}
			*Ix = GET2D(localimg,xsize,*y,*x);
			GetNextSample(localimg,xsize, *y, *x, Ra, Rb, Rc, Rd , EOLine) ;
			//printf("hhh %d %d\n",*Ix,RUNval);
		}
	//	printf("inside runcnt:%d runindex:%d\n",RUNcnt,*RUNindex);
	}
	//	printf("inside runcnt:%d runindex:%d\n",RUNcnt,*RUNindex);
	// Run-length coding  
	//printf("runcnt%d runindex%d\n",runcnt,runindex);
	//printf("binside runcnt:%d runindex:%d\n",RUNcnt,*RUNindex);
	while(RUNcnt >= (1 << J[*RUNindex])){
		writeBit(pbw,1);
		RUNcnt = RUNcnt - (1 << J[*RUNindex]) ;
		//printf("inside runcnt:%d runindex:%d shift:%d\n",RUNcnt,*RUNindex,(1 << J[*RUNindex]));
		if(*RUNindex < 31) {
			*RUNindex = *RUNindex + 1;
		}
	}		
	//printf("inside runcnt:%d runindex:%d\n",RUNcnt,*RUNindex);
	
	// IF the run was interrupted by the end of a line 
	//if(*EOLine == 1 &&(ABS(*Ix-RUNval)<=near)) {
	//	//*x = *x -1;
	//	if(RUNcnt > 0) {
	//		writeBit(pbw,1) ;
	//		//return ;
	//	}
	//	return;
	//}
	//printf("hjhjhj\n");
	int originrunindex = *RUNindex ;
	if(ABS(*Ix-RUNval) > near) {
		writeBit(pbw,0);
		writeBits(pbw,RUNcnt,J[*RUNindex]);
		//printf("hhhh\n");
		if(*RUNindex > 0) {
			*RUNindex = *RUNindex - 1;
		}
	
	// Run interruption sample encoding 
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
	Errval = *Ix - Px ; 
	if((RItype == 0) && (*Ra > *Rb)) {
		Errval = - Errval ;
		SIGN  = -1; 
	}else {
		SIGN = 1; 
	}

	if(near > 0) { 
		quantize(near,&Errval) ;
		computeRx(near ,&Rx ,SIGN , Px, &Errval); 
	}else {
		Rx = *Ix ;
	}
	//printf("decoding x:%d y:%d Rx:%d\n",*x,*y,Rx);
	//GET2D(localimg,xsize,*y,*x) = Rx ;
	Errval = ModRange(RANGE ,Errval) ; 
	
	//printf("error Q:%d N:%d A:%d\n",366,N[366],A[366]);
	int AQ ,TEMP,k, map,EMErrval; 
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
	if((k==0)&&(Errval>0)&&(2*Nn[Q-365]<N[Q]))
		map=1;
	else if((Errval<0)&&(2*Nn[Q-365]>=N[Q]))
		map=1;
	else if((Errval<0)&&(k!=0))
		map=1;
	else 
		map=0;
    EMErrval = 2*ABS(Errval) - RItype - map ;
	//printf("k : %d Errval %d EMErrval %d\n",k,Errval,EMErrval);
	int glimit = limit - J[*RUNindex] -1;
	//printf("x : %d y :%d",*x,*y);
	//printf("Ra:%d Rb:%d Rc:%d Rd:%d\n",*Ra,*Rb,*Rc,*Rd);
	//printf("Q:%d Ix:%d Px:%d sign:%d\n",Q,*Ix,Px,SIGN);
	//printf("error Q:%d N:%d A:%d\n",Q,N[Q],A[Q]);
	//printf("Jout :%d glimit:%d  k : %d Errval %d EMErrval %d\n",J[*RUNindex],glimit,k,Errval,EMErrval);
	GolombCoding(pbw,qbpp,glimit,EMErrval,k); 
	
	// Update  
	if(Errval < 0) 
		Nn[Q-365] = Nn [Q-365] + 1;
	A[Q] = A[Q] + ((EMErrval + 1 + RItype) >> 1) ;
	if(N[Q] == RESET) {
		A[Q] = A[Q] >> 1; 
		N[Q] = N[Q] >> 1; 
		Nn[Q-365] = Nn[Q-365] >> 1;
	}
	//if(N[Q] == 64) {
	//	printf("error Q:%d N:%d A:%d\n",Q,N[Q],A[Q]);
	//}
	/************************************/
	//N[Q] = N[Q] + 1;
	}else if(RUNcnt > 0){
		writeBit(pbw,1);
	}
	//printf("error Q:%d N:%d A:%d\n",Q,N[Q],A[Q]);
}

 

int jlsencode(int near , int xsize, int ysize, unsigned char * img ,unsigned char * outstream){
	bool EOLine = 0; 
	
	int Ra, Rb,Rc, Rd ;
	int RUNindex = 0 ;
	int Ix ,D1 ,D2 , D3 ; 
	int MAXVAL = CONFIG_MAXVAL ;
	int RANGE, t1,t2,t3,quant,bpp,qbpp,limit,a_init ;
	setParameters (MAXVAL,near,&RANGE, &t1, &t2, &t3,&quant, &bpp, &qbpp, &limit, &a_init);
	BitWriter_t pbw = initBitWriter(outstream);
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
	unsigned char * localimg = img ;
	for (int y = 0 ; y < ysize ; y++) {
		for (int x = 0;x < xsize; x ++ ) {
			GetNextSample(localimg,xsize, y, x, &Ra , &Rb , &Rc , &Rd , &EOLine) ;
			int D1 = Ra - Rb ; 
			int D2 = Rb - Rc ;
			int D3 = Rc - Ra ; 
			Ix = GET2D(localimg,xsize,y,x );
			//printf("x : %d y:%d\n",x,y);
			//printf("Ra:%d Rc:%d Rb:%d Rd:%d Ix:%d\n", Ra,Rc,Rb,Rd,Ix);
			//printf("runindex %d\n",RUNindex);
			if( (ABS(D1) <= near) && ( ABS(D2) <= near) && (ABS(D3) <= near)) {
				// RUNMODEProcessing 
			//	printf("1\n");
				RUNMODEProcessing(near,xsize,ysize, RANGE,limit,qbpp,&pbw,localimg,&RUNindex,&x,&y,&Ra ,&Rb, &Rc, &Rd, &Ix, A,B,C, N,Nn, &EOLine); 
				//printf("jjjj\n");
			}
			else {
			//	printf("2\n");
				// RegularModeProcessing 
				RegularMODEProcessing(near,xsize,ysize,RANGE,limit,qbpp,t1,t2, t3, D1,D2,D3, &pbw,localimg,&RUNindex,&x,&y,&Ra ,&Rb, &Rc, &Rd, &Ix, A, B,C, N,Nn, &EOLine);
				//printf("hhhh\n");
			
			}
			//if(N[366]>64 || A[366]>1000) {
			//	exit(0);
			//}
		}
	}
	flushBits(&pbw);
	int len = getBitWriterLength(&pbw);
	return len ;//pbw->pbuf - pbw->pbuf_base;
} 
int jpeglsencode(const unsigned char* data, int n , unsigned char * code ,int xsize = 8,int ysize = 8) {
	int near = 0;
	int ysz  = ysize;
	int xsz  = xsize;
	unsigned char img [n]; 
	for (int i = 0; i < n; i ++){
		img[i] = data[i];
	}
	int length = jlsencode(near,xsz,ysz,img,code);
	//printf("len %d\n",length);
	return length;
}
int jpeglscompress (const unsigned char* data, int *size, unsigned char* compressed ,bool flag,int xsize = 8,int ysize = 8){
	//分离出 b g r a
	//*size = 
	int onesize = *size / 4 ; 	
//	cout << onesize << endl ;
	//printf("onesize : %d\n",*size );
	unsigned char b[onesize],g[onesize],r[onesize],a[onesize];
	int colorind = 0;
	for (int i = 0 ;i < onesize ; i ++ ){
		b[i] = data[colorind];
		//printf("%d ",b[i]);
		g[i] = data[colorind+1];
		r[i] = data[colorind+2];
		a[i] = data[colorind+3];
		colorind += 4;
	}
//	printf("\n");
	unsigned char bcode[10*onesize],gcode[10*onesize],rcode[10*onesize],acode[10*onesize];
	int bsize = jpeglsencode(b,onesize,bcode,xsize,ysize);
	int gsize = jpeglsencode(g,onesize,gcode,xsize,ysize);
	int rsize = jpeglsencode(r,onesize,rcode,xsize,ysize);
	int asize = jpeglsencode(a,onesize,acode,xsize,ysize);

//	char * bptr = (char*) malloc(bsize * sizeof(char));	
//	char * gptr = (char*) malloc(gsize * sizeof(char));	
//	char * rptr = (char*) malloc(rsize * sizeof(char));	
//	char * aptr = (char*) malloc(asize * sizeof(char));
//	
//	memcpy(bptr,bcode,bsize);
//	memcpy(gptr,gcode,gsize);
//	memcpy(rptr,rcode,rsize);
//	memcpy(aptr,acode,asize);
	// 将压缩完成的字符按顺序存入compressed ;
	unsigned char * compr = compressed;
	if((bsize >= onesize || gsize >= onesize || rsize >= onesize || asize>=onesize)&&flag){
		//cout << onesize <<endl;
		//printf("%d %d %d %d \n",bsize,gsize,rsize,asize);
		memcpy(compr,b,onesize); 
		compr +=onesize;

		memcpy(compr,g,onesize);
		compr +=onesize;

		memcpy(compr,r,onesize);
		compr += onesize;

		memcpy(compr,a,onesize);
		compr += onesize ;

		*size = 4*onesize;
	} else {
		memcpy(compr,bcode,bsize); 
		compr +=bsize;
	
		memcpy(compr,gcode,gsize);
		compr +=gsize;
	
		memcpy(compr,rcode,rsize);
		compr += rsize ;
	
		memcpy(compr,acode,asize);
		compr += asize ;
	
		*size = bsize+gsize+rsize+asize;
	}
//	printf("%d %d %d %d \n",bsize,gsize,rsize,asize);
	return 0;
//	free(bptr);free(gptr);free(rptr);free(aptr);
}










#endif 
