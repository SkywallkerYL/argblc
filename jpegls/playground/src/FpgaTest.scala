package decoder 
import chisel3._
import chisel3.util._
import scala.util.Random
class RisingEdgeDetector extends Module {
  val io = IO(new Bundle {
    val input = Input(Bool())
    val risingEdgeDetected = Output(Bool())
  })

  // 状态寄存器，表示上一时刻的输入状态
  val prevState = RegNext(io.input)

  // 当输入信号发生上升沿时，输出 risingEdgeDetected 为 true
  io.risingEdgeDetected := RegNext(io.input && !prevState)

  // 更新状态寄存器
 //prevState := io.input
}
object RisingEdge {
  def apply(in:Bool): Bool = {
    val inst = Module(new RisingEdgeDetector)
    inst.io.input := in
    inst.io.risingEdgeDetected
  }
}

/*****
这个顶层用于进行FPGA的上版验证
从外部的ROM里面直接读入图片的数据，

然后压缩模块把压缩后的数据再写入外部的BRAM或者sram 

然后解压再从外部的BRAM或者SRAM读出压缩数据
将解压得到的图片数据写入外部的BRAM或者SRAM 


然后再来一个compare  的操作
依次从外部的ROM和解压得到的数据读取图像像素文件
并且进行一个比较


需要手动配置图片的大小 x y 

*****/
class JpegIP extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val control = new controlIO
    //val flush = Input(Bool())
    val size = new sizeIO
    val state = Output(UInt(3.W))
    val allreset = Input(Bool())
    val success = Output(Bool())
    //Master 

    //val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val compress = Module(new compressARGBfile)
  //对于COMPRESS 模块  他读图片只会用到读通道 
  //写数据指挥用到写通道 就一个AXI口
  //所以这里直接一个连接就行 只是在顶层用的时候改一下
  val RomAxi = Module(new ramtop)
  //if(Fpga) Module(new  RomwithIP) else Module(new ramtop)
  val CompressRamAxi = Module(new ramtop)
  //if(FPGATEST) Module(new  RamwithIP) else Module(new ramtop)
  val DecompressRamAxi = Module(new ramtop)
  //if(FPGATEST) Module(new  RamwithIP) else Module(new ramtop)


  RomAxi.io.axi <> compress.io.axi 
  
  //pix Rom的写数据端口要悬空
  RomAxi.io.axi.awvalid := false.B 
  RomAxi.io.axi.wvalid := false.B 


  //这里写的时候没有把 读端口和写端口分开，所以有点不方便了。。。
  //compress.io.axi.araddr :=
  //CompressRamAxi.io.axi.awaddr <> compress.io.axi 
  CompressRamAxi.io.axi.awaddr  <> compress.io.axi.awaddr
  CompressRamAxi.io.axi.awvalid <> compress.io.axi.awvalid
  CompressRamAxi.io.axi.awid    <> compress.io.axi.awid
  CompressRamAxi.io.axi.awready <> compress.io.axi.awready
  CompressRamAxi.io.axi.awlen   <> compress.io.axi.awlen
  CompressRamAxi.io.axi.awsize  <> compress.io.axi.awsize
  CompressRamAxi.io.axi.awburst <> compress.io.axi.awburst
  CompressRamAxi.io.axi.wvalid  <> compress.io.axi.wvalid
  CompressRamAxi.io.axi.wready  <> compress.io.axi.wready
  CompressRamAxi.io.axi.wdata   <> compress.io.axi.wdata
  CompressRamAxi.io.axi.wstrb   <> compress.io.axi.wstrb
  CompressRamAxi.io.axi.wlast   <> compress.io.axi.wlast
  CompressRamAxi.io.axi.bready  <> compress.io.axi.bready
  CompressRamAxi.io.axi.bvalid  <> compress.io.axi.bvalid
  CompressRamAxi.io.axi.bid     <> compress.io.axi.bid
  CompressRamAxi.io.axi.bresp   <> compress.io.axi.bresp
  
  compress.io.control.start := RisingEdge (io.control.start)//false.B // <> io.control 
  compress.io.size <> io.size
  //地址偏移，都默认为0
  compress.io.compinf.incompaddr := 0.U 
  compress.io.compinf.inpixaddr := 0.U 
  val decompress = Module(new decompressARGBfile)
  //顶层的CompressRamAxi读数据端口要重新连接给decompress模块
  //CompressRamAxi.io.axi.bready  <> decompress.io.axi.bready  
  //CompressRamAxi.io.axi.bvalid  <> decompress.io.axi.bvalid  
  //CompressRamAxi.io.axi.bid     <> decompress.io.axi.bid     
  //CompressRamAxi.io.axi.bresp   <> decompress.io.axi.bresp   
  CompressRamAxi.io.axi.arready <> decompress.io.axi.arready 
  CompressRamAxi.io.axi.arvalid <> decompress.io.axi.arvalid 
  CompressRamAxi.io.axi.arid    <> decompress.io.axi.arid    
  CompressRamAxi.io.axi.araddr  <> decompress.io.axi.araddr  
  CompressRamAxi.io.axi.arlen   <> decompress.io.axi.arlen   
  CompressRamAxi.io.axi.arsize  <> decompress.io.axi.arsize  
  CompressRamAxi.io.axi.arburst <> decompress.io.axi.arburst 
  CompressRamAxi.io.axi.rready  <> decompress.io.axi.rready  
  CompressRamAxi.io.axi.rvalid  <> decompress.io.axi.rvalid  
  CompressRamAxi.io.axi.rid     <> decompress.io.axi.rid     
  CompressRamAxi.io.axi.rresp   <> decompress.io.axi.rresp   
  CompressRamAxi.io.axi.rdata   <> decompress.io.axi.rdata   
  CompressRamAxi.io.axi.rlast   <> decompress.io.axi.rlast   
  decompress.io.axi.awready := false.B 
  decompress.io.axi.wready := false.B 
  decompress.io.axi.bvalid := false.B 
  decompress.io.axi.bid := 0.U 
  decompress.io.axi.bresp := 0.U 


  DecompressRamAxi.io.axi <> decompress.io.pixaxi

  //C仿真的Picdata在50000000 压缩数据在40000000  解压出来的数据放在3000 0000
  if(!Fpga){
    RomAxi.io.axi.araddr := compress.io.axi.araddr + "x50000000".U
    CompressRamAxi.io.axi.awaddr := compress.io.axi.awaddr + "x40000000".U 
    CompressRamAxi.io.axi.araddr  <> decompress.io.axi.araddr + "x40000000".U 
    DecompressRamAxi.io.axi.awaddr := decompress.io.pixaxi.awaddr + "x30000000".U
    DecompressRamAxi.io.axi.araddr := decompress.io.pixaxi.araddr + "x30000000".U
  }

  decompress.io.control.start := RisingEdge (compress.io.control.finish)//io.control 
  decompress.io.size <> io.size
  decompress.io.compinf.incompaddr := 0.U 
  decompress.io.compinf.inpixaddr := 0.U 

  val success = RegInit(false.B)
  io.success := success
  io.control.finish := false.B 
  //状态机

  val idle :: process :: readpix:: getpix :: compare :: finish :: Nil =  Enum(6)
  val MainState = RegInit(idle) 
  io.state := MainState
  val pixRom = RegInit(0.U (8.W)) 
  val pixRam = RegInit(0.U (8.W)) 
  val ReadSize = RegInit(0.U (32.W))
  val ReadAddr = RegInit(0.U (32.W))
  val getRomReady = RegInit(false.B)
  val getRom = RegInit(false.B)
  val getRamReady = RegInit(false.B)
  val getRam = RegInit(false.B)
  switch(MainState){
    is(idle){
      when(io.control.start){
        MainState := process
        ReadSize := (io.size.height*io.size.widthh)<<2.U 
        ReadAddr := 0.U 
        success := true.B 
      }
    }
    is(process){
      when(decompress.io.control.finish){
        MainState := readpix
      }
    }
    is(readpix){
      RomAxi.io.axi.araddr := ReadAddr
      DecompressRamAxi.io.axi.araddr := ReadAddr
      RomAxi.io.axi.arvalid := true.B 
      RomAxi.io.axi.rready := false.B 
      DecompressRamAxi.io.axi.rready := false.B 
      DecompressRamAxi.io.axi.arvalid := true.B
      RomAxi.io.axi.arsize := 4.U 
      DecompressRamAxi.io.axi.arsize := 4.U 

      when(RomAxi.io.axi.arready){
        getRomReady := true.B 
      }
      when(DecompressRamAxi.io.axi.arready){
        getRamReady := true.B 
      }
      when(getRomReady && getRamReady){
        MainState := getpix
        getRomReady := false.B 
        getRamReady := false.B 
      }
      //分别从ROM 和RAM里读数据 并且进行比较
    }
    is(getpix){
      RomAxi.io.axi.rready := true.B 
      DecompressRamAxi.io.axi.rready := true.B 
      when(RomAxi.io.axi.rvalid){
        getRom := true.B 
        pixRom := RomAxi.io.axi.rdata
      }
      when(DecompressRamAxi.io.axi.rvalid){
        getRam := true.B 
        pixRam := DecompressRamAxi.io.axi.rdata
      }
      when(getRom && getRam){
        MainState := compare
        getRom := false.B 
        getRam := false.B 
      }
    }
    is(compare){
      when(getRom === getRam){
        when(ReadAddr === ReadSize-1.U){
          MainState := finish
          success := true.B 
        }.otherwise{
          MainState := readpix
          ReadAddr := ReadAddr + 1.U 
        }
      }.otherwise{
        success := false.B 
        MainState := finish
      }
    }
    is(finish){
      when(io.allreset){
        MainState := idle 
        ReadAddr := 0.U 
      }
      io.control.finish := true.B 
    }
  }

  //io.econtrol.finish :=  jls.io.econtrol.finish
}