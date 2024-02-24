# Jpeg-loss less压缩解压单元(通过chisel实现)
该项目是一个基于Chisel的JPEG无损压缩解压硬件设计。
## 概述
该单元通过AXI-FULL接口与外部单元相连。压缩单元读取外部图像的像素数据(bmp格式)，并且将压缩数据写入ROM。解压单元读取压缩后的数据，并且将图像像素数据写入ROM。
目前该单元性能不佳，进行一个tile的解压缩时，需要对同一个像素数据进行反复读取，导致周期较长，且并未流水化，仅仅简单实现了功能。

## 主要模块说明

### compressARGBfile
该模块位于 jpegls/playground/src/compress.scala
其用于进行完整的压缩流程，写入压缩数据的文件头，每个tile的大小和信息，以及压缩的数据。其实现的功能类似argblc/jingjiawei/src/main.cpp中的compressARGB函数
AXI端口的读端口读取图像数据
AXI端口的写端口向外写入压缩后的数据
compinf用于配置读取像素的起始地址和写入数据的其实地址
size用于配置图像的大小
controlIO的start拉高时，开始压缩流程，结束时finish信号拉高一个周期

### decompressARGBfile
该模块位于 jpegls/playground/src/decompress.scala
其用于进行完整的解压流程，写入压缩数据的文件头，每个tile的大小和信息，以及压缩的数据。其实现的功能类似argblc/jingjiawei/src/main.cpp中的decompressARGB函数
AXI端口的读端口读取图像的压缩数据
PIXAXI端口的读写端口读出和写入图像数据
compinf用于配置读取像素的起始地址和写入数据的其实地址
size用于配置图像的大小
controlIO的start拉高时，开始解压流程，结束时finish信号拉高一个周期

### tilecompress
该模块位于 jpegls/playground/src/compress.scala
该模块用于进行一个完整的tile的压缩按顺序对B G R A四个通道的数据进行压缩
他的内部例化了一个jpeglsencode模块其是最基本的jpegls压缩单元
基本单元的
AXI端口的读端口读取图像数据
AXI端口的写端口向外写入压缩后的数据
flush信号用于重置Bitwrite的状态，结束当前比特流的写入

### tiledecompress
该模块位于 jpegls/playground/src/decompress.scala
该模块用于进行一个完整的tile的解压，按顺序对B G R A四个通道的数据进行解压
他的内部例化了一个jpeglsdecode模块其是最基本的jpegls解压单元
基本单元的
AXI端口的读端口读取图像数据
AXI端口的写端口向外写入压缩后的数据
flush信号用于重置Bitread的状态，结束当前比特流的读入

## 参数配置说明

参数位于 jpegls/playground/src/common.scala
NEAR是jpeg解压缩的参数，为0表示无损解压缩
目前只检验了无损解压缩情况，可以正常对图像进行解压缩。

## 软件测试代码说明
本项目通过Verilator进行仿真测试
包括软件上实现的JPEG解压缩函数，对图片数据进行压缩解压的回环验证。具体流程见jingjiawei/readme.MD
对于硬件上的验证包括对基本单元的验证，对Tile解压缩模块的验证，以及对完整模块的验证，还有完整解压缩模块的验证，以及上板的验证。具体见/home/yangli/argblc/jpegls/csrc/common.h中的**TESTMODULE**函参数。
修改common.scala中的Fpga参数，当其为true时，可以生成用于上板验证的Verilog代码
只是还要修改jpegls/playground/src/FpgaTest.scala中65 67 69行的rom ram的例化代码。进行不同的测试时，需要修改jpegls/Makefile中的TOP变量

## FPGA资源消耗(包括一个解压模块和压缩模块以及三个调用IP的ROM)
|   项目   | 使用的 LUT 数量 | 使用的 FF 数量 | 使用的 DSP 数量 |
|:--------:|:----------------:|:--------------:|:----------------:|
|   总计   |       28184        |      41470       |        21         |

时钟频率100MHz
To run Verilator Test:
```bash
make run
```

To generate Verilog:
```bash
make verilog
```

## Change FIRRTL Compiler

You can change the FIRRTL compiler between SFC (Scala-based FIRRTL compiler) and
MFC (MLIR-based FIRRTL compiler) by modifying the `useMFC` variable in `playground/src/Elaborate.scala`.
The latter one requires `firtool`, which is included under `utils/`.
