#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include "common.h"
#include "testfunc.h"
#include "simualte.h"
int main(int argc , char* argv[]) {
	sim_init();

	reset(5);
#if TESTMODULE == 1
	encodetest();
#elif TESTMODULE == 2 
	//printf("jjjjj\n");
	decodetest();
#endif 
	sim_exit();
	return 0;
}