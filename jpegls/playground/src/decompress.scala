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
	for (int i = 0 ; i < onesize; i++  ){
		data[i*4]   = res[i];
		data[i*4+1] = res[onesize+i];
		data[i*4+2] = res[onesize*2+i];
		data[i*4+3] = res[onesize*3+i];
	}
jpeglsdecode

写图像数据的地址从0x00000000开始
读压缩数据从0开始0x80000000开始

在外部模块加地址偏移
*/
//class compIO extends Bundle with COMMON {
//  val inpixaddr  = Input(UInt(AXIADDRWIDTH.W)) 
//  val incompaddr = Input(UInt(AXIADDRWIDTH.W))
//  val len        = Output(UInt(AXIADDRWIDTH.W))
//}


class tiledecompress extends Module with COMMON {
  val io = IO(new Bundle {
    val tcontrol = new controlIO
    // read bit 
    val axi = new AXIIO
    // read Pix  write Pix 
    val pixaxi = new AXIIO
    val compinf = new compIO
  })
  val jls = Module(new jpeglsdecode)
  jls.io.dcontrol.start := false.B 
  jls.io.flush := false.B


  io.axi <> jls.io.axi
  io.pixaxi <> jls.io.pixaxi
  io.tcontrol.finish := false.B 

  val pixreadaddr = RegInit(0.U(AXIADDRWIDTH.W))
  val compaddr = RegInit(0.U(AXIADDRWIDTH.W))
  val len =RegInit(0.U(AXIADDRWIDTH.W))
  val coloraddr = jls.io.pixaxi.awaddr
  val rcoloraddr = jls.io.pixaxi.araddr
  io.compinf.len := len-1.U
  io.axi.araddr := jls.io.axi.araddr-"x80000000".U + compaddr   
  //jls.io.axi <> ram.io.axi 
  //jls.io.dcontrol <> io.dcontrol 
  //jls.io.flush <> io.flush
  //io.len := jls.io.len
  //io.dcontrol.finish :=  jls.io.dcontrol.finish
  val statecount = RegInit(0.U(2.W))

  val idle :: decompressB :: decompressG :: decompressR :: decompressA :: initial :: finish :: Nil = Enum(7)

  val tilestate = RegInit(idle)

  switch(tilestate){
    is(idle){
      when(io.tcontrol.start){
        tilestate := decompressB
        pixreadaddr := io.compinf.inpixaddr
        compaddr := io.compinf.incompaddr
        statecount := 0.U 
        len := 0.U 
      }
    }
    is(decompressB){
      jls.io.dcontrol.start := true.B 
      io.pixaxi.awaddr := ((coloraddr )<<2.U)+ pixreadaddr
      io.pixaxi.araddr := ((rcoloraddr)<<2.U)+ pixreadaddr
      when(jls.io.dcontrol.finish){
        tilestate := initial
        jls.io.dcontrol.start := false.B 
      }
    }
    is(decompressG){
      jls.io.dcontrol.start := true.B 
      io.pixaxi.awaddr := ((coloraddr)<<2.U ) + 1.U + pixreadaddr
      io.pixaxi.araddr := ((rcoloraddr)<<2.U) + 1.U + pixreadaddr
      when(jls.io.dcontrol.finish){
        tilestate := initial
        jls.io.dcontrol.start := false.B 
      }
    }
    is(decompressR){
      jls.io.dcontrol.start := true.B 
      io.pixaxi.awaddr := ((coloraddr)<<2.U ) + 2.U + pixreadaddr
      io.pixaxi.araddr := ((rcoloraddr)<<2.U) + 2.U + pixreadaddr
      when(jls.io.dcontrol.finish){
        tilestate := initial
        jls.io.dcontrol.start := false.B 
      }
    }
    is(decompressA){
      jls.io.dcontrol.start := true.B 
      io.pixaxi.awaddr := ((coloraddr)<<2.U ) + 3.U + pixreadaddr
      io.pixaxi.araddr := ((rcoloraddr)<<2.U) + 3.U + pixreadaddr
      when(jls.io.dcontrol.finish){
        tilestate := initial
        jls.io.dcontrol.start := false.B 
      }
    }
    is(initial){
      //jls.io.flush := true.B 
      statecount := statecount + 1.U 
      len := jls.io.len
      when(statecount === 0.U){
        tilestate := decompressG
      }.elsewhen(statecount === 1.U){
        tilestate := decompressR
      }.elsewhen(statecount === 2.U){
        tilestate := decompressA
      }.elsewhen(statecount === 3.U){
        tilestate := finish
      }
    }
    is(finish){
      jls.io.flush := true.B 
      io.tcontrol.finish := true.B 
      tilestate := idle
    }

  }
  


}

class tiledecompresssimtop extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val tcontrol = new controlIO
    //val flush = Input(Bool())
    val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val jls = Module(new tiledecompress)
  val bitram = Module(new  ramtop)
  val pixram = Module(new  ramtop)
  jls.io.axi <> bitram.io.axi 
  jls.io.pixaxi <> pixram.io.axi 
  jls.io.tcontrol <> io.tcontrol 
  io.len := jls.io.compinf.len
  jls.io.compinf.inpixaddr := "x60000000".U 
  jls.io.compinf.incompaddr := "x70000000".U
  //io.dcontrol.finish :=  jls.io.dcontrol.finish
}

class decompressARGB extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val tcontrol = new controlIO
    //val flush = Input(Bool())
    val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val jls = Module(new tiledecompress)
  val bitram = Module(new  ramtop)
  val pixram = Module(new  ramtop)
  jls.io.axi <> bitram.io.axi 
  jls.io.pixaxi <> pixram.io.axi 
  jls.io.tcontrol <> io.tcontrol 
  io.len := jls.io.compinf.len
  jls.io.compinf.inpixaddr := "x60000000".U 
  jls.io.compinf.incompaddr := "x70000000".U
}