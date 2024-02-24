#Jpeg-loss less压缩解压单元(通过chisel实现)
该项目是一个基于Chisel的JPEG无损压缩解压硬件设计。
##概述
该单元通过AXI-FULL接口与外部单元相连。压缩单元读取外部图像的像素数据(bmp格式)，并且将压缩数据写入ROM。解压单元读取压缩后的数据，并且将图像像素数据写入ROM。




```bash
make test
```

To generate Verilog:
```bash
make verilog
```

## Change FIRRTL Compiler

You can change the FIRRTL compiler between SFC (Scala-based FIRRTL compiler) and
MFC (MLIR-based FIRRTL compiler) by modifying the `useMFC` variable in `playground/src/Elaborate.scala`.
The latter one requires `firtool`, which is included under `utils/`.
