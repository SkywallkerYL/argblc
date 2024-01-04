package decoder 
import chisel3._
import chisel3.util._
import scala.io.{BufferedSource,Source}
import java.io._
trait COMMON{
  def ABS(x: Int): Int = if (x < 0) -x else x
  def MAX(x: Int, y: Int): Int = if (x < y) y else x
  def MIN(x: Int, y: Int): Int = if (x < y) x else y

  def ABSH(x: SInt): SInt = Mux ((x < 0.S) ,-x , x)
  def MAXH(x: SInt, y: SInt): SInt = Mux ( (x < y) ,y, x)
  def MINH(x: SInt, y: SInt): SInt = Mux ( (x < y) ,x, y)
  def CLIP(x: SInt, min: SInt, max: SInt): SInt = MINH(MAXH(x, min), max)

  // 定义常量
  val NEAR = 0
  val RESET = 64
  val MAXVAL = 255
  val X_SIZE = 4
  val Y_SIZE = 4
  val MIN_C = -128
  val MAX_C = 127

  // 定义整数数组
  val J = Seq(0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15)

  // 定义函数
  def getceillog(value: Int): Int = {
    var ans = 1
    while (1 << ans < value) {
      ans += 1
    }
    ans
  }

  val tmp = (MIN(MAXVAL, 4096) + 127) / 256
  val t1  = tmp + 2 + 3 * NEAR
  val t2  = 4 * tmp + 3 + 5 * NEAR
  val t3  = 17 * tmp + 4 + 7 * NEAR
  val quant = 2 * NEAR + 1
  val RANGE = (MAXVAL + 2 * NEAR) / quant + 1
  val bpp   = MAX(2, getceillog(MAXVAL + 1))
  val qbpp  = getceillog(RANGE)
  val limit = 2 * (bpp + MAX(8, bpp))
  val a_init = MAX((RANGE + 32) / 64, 2)

  //数据位宽
  val COLORWIDTH      = 8
  val SIGNWIDTH       = 10
  val SIGNWIDTH1      = 11
  val TEXTWIDTH       = 13

  //地址位宽
  val XADDRWIDTH = log2Ceil(X_SIZE)
  val YADDRWIDTH = log2Ceil(Y_SIZE)
  //  量化表  
  
  //ITER 的宽度 
  val ITERWITH = 5  
  val FRAMEWITH = 64
//这个相对路径是根据makefile所在的文件夹决定的
  val IOTablePath     : String    = "./build/Table.h"
  val FilePath        : String    = "../matrix/qc_peg_5_32.txt"
  val CodePath          : String    = "../matrix/codeword.txt"
  val UncodePath        : String    = "../matrix/uncodedbits.txt"
//RAM的类型 ，是用syncreadmem 或者 mem  
//并行路数
  val PARRELNUM  = 1
// 最多记录的错误比特的个数   
  val MAXERRORNUM = 10
  // 后处理参数  
  val strongMessage = 6
  val weakMessage   = 2
  val strongMessage0 = 7
  val weakMessage0   = 2
  val postIterInput      = 8
  val maxpostnum  = 2
  
  
  val PWITH = 16

  val BYTEWIDTH = 8
  //每8个bit占用的地址数 即一个地址存8bit
  val ADDRADD   = 1
  //AXI PARAMETER
  val AXIDATAWIDTH = 8 
  val AXISTRBWIDTH = 8
  val AXIADDRWIDTH = 32
  val AXILENWIDTH  = 8
  val AXISIZEWIDTH = 3
  val AXIBURSTWIDTH= 2 
  val AXIIDWIDTH   = 4
  val AXIRESPWIDTH = 2
 
}
object COMMON extends COMMON {}



// 以master 视角  
//
// slave flipp即可  
class AXIIO extends Bundle with COMMON {
  val awready = Input(Bool()) 
  val awvalid = Output(Bool())
  val awid    = Output(UInt(AXIIDWIDTH.W))
  val awaddr  = Output(UInt(AXIADDRWIDTH.W))
  val awlen   = Output(UInt(AXILENWIDTH.W))
  val awsize  = Output(UInt(AXISIZEWIDTH.W))
  val awburst = Output(UInt(AXIBURSTWIDTH.W))

  val wready  = Input(Bool())
  val wvalid  = Output(Bool())
  val wdata   = Output(UInt(AXIDATAWIDTH.W))
  val wstrb   = Output(UInt(AXISTRBWIDTH.W))
  val wlast   = Output(Bool())
  
  val bready  = Output(Bool())
  val bvalid  = Input(Bool())
  val bid     = Input(UInt(AXIIDWIDTH.W))
  val bresp   = Input(UInt(AXIRESPWIDTH.W))

  val arready = Input(Bool())
  val arvalid = Output(Bool())
  val arid    = Output(UInt(AXIIDWIDTH.W))
  val araddr  = Output(UInt(AXIADDRWIDTH.W))
  val arlen   = Output(UInt(AXILENWIDTH.W))
  val arsize  = Output(UInt(AXISIZEWIDTH.W))
  val arburst = Output(UInt(AXIBURSTWIDTH.W))

  val rready  = Output(Bool())
  val rvalid  = Input(Bool())
  val rid     = Input(UInt(AXIIDWIDTH.W))
  val rresp   = Input(UInt(AXIRESPWIDTH.W))
  val rdata   = Input(UInt(AXIDATAWIDTH.W))
  val rlast   = Input(Bool())



}