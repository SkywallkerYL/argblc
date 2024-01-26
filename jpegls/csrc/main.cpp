#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include "common.h"
#include "testfunc.h"
#include "simualte.h"
#include "argbtilesim.h"
#include "addr.h"
int main(int argc , char* argv[]) {
	sim_init();

	reset(5);
#if TESTMODULE == 1
	encodetest();
#elif TESTMODULE == 2 
	//printf("jjjjj\n");
	decodetest();
#elif TESTMODULE == 3 
    char const* inFileName = NULL;
    inFileName = argv[2];
    encodetest(inFileName );
	//encodetest();
#elif TESTMODULE == 4
    char const* compressedFileName = NULL;
    compressedFileName = argv[2];
    char const* outFileName = NULL;
    outFileName = argv[3];
    decodetest(compressedFileName,outFileName );
	//decodetest();
#elif TESTMODULE == 5
	//compresstest();
	char const* inFileName = NULL;
	inFileName = argv[2];
	compressfiletest(inFileName);
#elif TESTMODULE == 6 || TESTMODULE == 7 || TESTMODULE == 8 
	 int     func = 0;
	char const* inFileName = NULL;
    char const* outFileName = NULL;
    int     IsNewBuff = 0;
    int     ret = ERROR_OK;

#define USAGE   "USAGE: fblcd.out [--version] [-{en,de,cp} infile outfile]"

    if (argc < 2) {
        std::cout << USAGE << std::endl;
        std::cout << "parameters: " << std::endl;
        std::cout << "  --version              show author's info" << std::endl;
        std::cout << "  -en infile outfile     encode(compress) `infile` to `outfile`" << std::endl;
        std::cout << "                           `infile` should be BMP file" << std::endl;
        std::cout << "                           `outfile` is TILE file" << std::endl;
        std::cout << "                           if `outfile` parameter is not specified, it will assigned to `infile` and add JLCD suffix" << std::endl;
        std::cout << "  -de infile outfile     decode(decompress) `infile` to `outfile`" << std::endl;
        std::cout << "                           `infile` should be TILE file" << std::endl;
        std::cout << "                           `outfile` is BMP file" << std::endl;
        std::cout << "                           if `outfile` parameter is not specified, it will assigned to `infile` and add BMP suffix" << std::endl;
        std::cout << "  -cp infile outfile     compare `infile` and `outfile`, pixel by pixel" << std::endl;
        return ERROR_PARAM_NOT_ENOUGH;
    }

    if (strcmp(argv[1], "--version") == 0) {
        std::cout << USAGE << std::endl;
        // !!! REPLACE THE NAME TO YOUR TEAM'S
        std::cout << "made by: ChatSOC  "  << std::endl;
        std::cout << "version: " << 1 << std::endl;
        return ERROR_PARAM_NOT_ENOUGH;
    }
    else if (strcmp(argv[1], "-en") == 0) {
        func = 1;
    }
    else if (strcmp(argv[1], "-de") == 0) {
        func = 2;
    }
    else {
        return ERROR_INVALID_PARAM;
    }

    if (argc < 3) {
        std::cout << "ERROR: parameter is not enough!" << std::endl;
        return ERROR_PARAM_NOT_ENOUGH;
    }
    inFileName = argv[2];
    if (argc >= 4) {
        outFileName = argv[3];
    }
    else {
        IsNewBuff = 1;
        outFileName = new char[strlen(inFileName) + 6];
        sprintf((char*)outFileName, "%s.%s", inFileName, (1==func) ? "jlcd" : "bmp");
        std::cout << "output file is not assigned, we assign it to: " << outFileName << std::endl;
    }

    if (func == 1) {
        // compress
		//encodetest(inFileName, outFileName);
		//compressinC(inFileName, outFileName);
        //writeCoeFile(inFileName,outFileName);
        fpgaSimulate(inFileName,outFileName);
        //unsigned char * comp = compressARGBfile(inFileName, outFileName);
    } else if (func == 2) {
        // decompress
        //decodetest(inFileName,outFileName);
        //decompressinC(inFileName,outFileName);
        //ret = decompressARGB(inFileName, outFileName);
    }
    std::cout << "result = " << ret << std::endl;
    if (IsNewBuff) {
        delete[] outFileName;
        outFileName = NULL;
    }
    return ret;
#endif 
	sim_exit();
	return 0;
}
