package decoder 
import chisel3._
import chisel3.util._
import scala.util.Random
/*
此模块用于对一个tile进行压缩
传入的数据应该是按照每4个Byte的bgra顺序存
一共有sizex*sizey个
比如8*8就是
8*8*4总的数据大小
int onesize = 8*8*4 / 4 ; 
	for (int i = 0 ;i < onesize ; i ++ ){
		b[i] = data[colorind];
		g[i] = data[colorind+1];
		r[i] = data[colorind+2];
		a[i] = data[colorind+3];
		colorind += 4;
	}
jpeglsencode
写压缩数据的地址从0x80000000开始
读图像像素的数据从0开始

在外部模块加地址偏移
*/
class compIO extends Bundle with COMMON {
  val inpixaddr  = Input(UInt(AXIADDRWIDTH.W)) 
  val incompaddr = Input(UInt(AXIADDRWIDTH.W))
  val len        = Output(UInt(AXIADDRWIDTH.W))
}


class tilecompress extends Module with COMMON {
  val io = IO(new Bundle {
    val tcontrol = new controlIO
    // read pix 
    // write bit 
    val axi = new AXIIO

    val compinf = new compIO
  })
  val jls = Module(new jpeglsencode)
  jls.io.econtrol.start := false.B 
  jls.io.flush := false.B


  io.axi <> jls.io.axi
  io.tcontrol.finish := false.B 

  val pixreadaddr = RegInit(0.U(AXIADDRWIDTH.W))
  val compaddr = RegInit(0.U(AXIADDRWIDTH.W))
  val len =RegInit(0.U(AXIADDRWIDTH.W))
  val coloraddr = jls.io.axi.araddr<<2.U
  io.compinf.len := len
  io.axi.awaddr := jls.io.axi.awaddr-"x80000000".U + compaddr + len  
  //jls.io.axi <> ram.io.axi 
  //jls.io.econtrol <> io.econtrol 
  //jls.io.flush <> io.flush
  //io.len := jls.io.len
  //io.econtrol.finish :=  jls.io.econtrol.finish
  val statecount = RegInit(0.U(2.W))

  val idle :: compressB :: compressG :: compressR :: compressA :: initial :: finish :: Nil = Enum(7)

  val tilestate = RegInit(idle)

  switch(tilestate){
    is(idle){
      when(io.tcontrol.start){
        tilestate := compressB
        pixreadaddr := io.compinf.inpixaddr
        compaddr := io.compinf.incompaddr
        statecount := 0.U 
        len := 0.U 
      }
    }
    is(compressB){
      jls.io.econtrol.start := true.B 
      io.axi.araddr := coloraddr + pixreadaddr
      when(jls.io.econtrol.finish){
        tilestate := initial
        jls.io.econtrol.start := false.B 
      }
    }
    is(compressG){
      jls.io.econtrol.start := true.B 
      io.axi.araddr := coloraddr + pixreadaddr + 1.U
      when(jls.io.econtrol.finish){
        tilestate := initial
        jls.io.econtrol.start := false.B 
      }
    }
    is(compressR){
      jls.io.econtrol.start := true.B 
      io.axi.araddr := coloraddr + pixreadaddr + 2.U
      when(jls.io.econtrol.finish){
        tilestate := initial
        jls.io.econtrol.start := false.B 
      }
    }
    is(compressA){
      jls.io.econtrol.start := true.B 
      io.axi.araddr := coloraddr + pixreadaddr + 3.U
      when(jls.io.econtrol.finish){
        tilestate := initial
        jls.io.econtrol.start := false.B 
      }
    }
    is(initial){
      jls.io.flush := true.B 
      statecount := statecount + 1.U 
      len := len + jls.io.len
      when(statecount === 0.U){
        tilestate := compressG
      }.elsewhen(statecount === 1.U){
        tilestate := compressR
      }.elsewhen(statecount === 2.U){
        tilestate := compressA
      }.elsewhen(statecount === 3.U){
        tilestate := finish
      }
    }
    is(finish){
      io.tcontrol.finish := true.B 
      tilestate := idle
    }

  }
  


}

class tilecompresssimtop extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val tcontrol = new controlIO
    //val flush = Input(Bool())
    val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val jls = Module(new tilecompress)
  val ram = Module(new  ramtop)
  jls.io.axi <> ram.io.axi 
  jls.io.tcontrol <> io.tcontrol 
  io.len := jls.io.compinf.len
  jls.io.compinf.inpixaddr := "x60000000".U 
  jls.io.compinf.incompaddr := "x70000000".U
  //io.econtrol.finish :=  jls.io.econtrol.finish
}