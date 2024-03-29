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
    val addr = new addrIO
    val compinf = new compIO
  })
  val jls = Module(new jpeglsencode)
  jls.io.econtrol.start := false.B 
  jls.io.flush := false.B
  io.addr <> jls.io.addr

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
/****
此模块用于从RAM或者ROM里直接读取图片文件数据，并且进行分tile的压缩
图片的数据地址定在0x50000000
写压缩数据的地址定在0x40000000

****/
class sizeIO extends Bundle with COMMON{
  val height  = Input(UInt(HEIGHT.W))
  val widthh  = Input(UInt(WIDTH.W))
}
class compressARGB extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val axi = new AXIIO
    val control = new controlIO
    val size = new sizeIO
    //val 
    //val flush = Input(Bool())
    val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val jls = Module(new tilecompress)
  val jlsstart = Wire(Bool())
  jlsstart := false.B 
  jls.io.tcontrol.start := jlsstart
  io.axi <> jls.io.axi
  jls.io.compinf.inpixaddr := 0.U 
  jls.io.compinf.incompaddr := 0.U
  val jlsfinish = jls.io.tcontrol.finish

  //
  val height = RegInit(0.U(HEIGHT.W))
  val widthh  = RegInit(0.U(WIDTH.W))

  val numRows    = RegInit(0.U(HEIGHT.W))//RegNext(height/(X_SIZE.U))//RegInit(0.U(HEIGHT.W))
  val numColumns = RegInit(0.U(WIDTH.W)) //RegNext(widthh /(Y_SIZE.U))//RegInit(0.U(WIDTH.W))
  
  val row    = Wire(UInt(HEIGHT.W))
  val col    = Wire(UInt(WIDTH.W)) 



  val x  = (jls.io.axi.araddr >> 2.U) % X_SIZE.U 
  //jls.io.addr.x 
  val y  = (jls.io.axi.araddr >> 2.U) / X_SIZE.U 


  // 4 bytes per pixel
  val rowStride = RegInit(0.U((WIDTH+2).W)) //RegNext(widthh<<2.U)


  //
  val tileRowIndex = RegInit(0.U(HEIGHT.W))
  val tileColumnIndex = RegInit(0.U(WIDTH.W))

  col := tileColumnIndex * X_SIZE.U 
  row := tileRowIndex * Y_SIZE.U 


  val tileSize = RegInit(0.U(AXIADDRWIDTH.W))
  io.len := tileSize
  io.control.finish := false.B
  //state control 

  val idle :: initial :: compresstile :: update :: finish :: Nil = Enum(5)

  val argbstate = RegInit(idle)

  switch(argbstate){
    is(idle){
      when(io.control.start){
        height := io.size.height
        widthh  := io.size.widthh
        argbstate := initial
      }
    }
    is(initial){
      numRows := height/(Y_SIZE.U)
      numColumns := widthh/(X_SIZE.U)
      tileRowIndex := 0.U 
      tileColumnIndex := 0.U
      tileSize := 0.U 
      rowStride := widthh << 2.U
      //row := 0.U 
      //col := 0.U 
      argbstate := compresstile
    }
    is(compresstile){
      jlsstart := true.B 
      when(jlsfinish){
        jlsstart := false.B 
        tileSize := tileSize + jls.io.compinf.len 
        argbstate := update
      }
      //process addr 
      //给tilecompress的Pixaddr给0
      //给tilecompress的compaddr给0
      val realrow = row + y 
      val realcol = col + x 
      val pixoffset = rowStride*realrow + realcol * 4.U
      io.axi.araddr := jls.io.axi.araddr(1,0) + pixoffset
      io.axi.awaddr := jls.io.axi.awaddr + tileSize

    }
    is(update){
      when(tileColumnIndex < (numColumns-1.U)){
        tileColumnIndex := tileColumnIndex + 1.U 
        //col := (tileColumnIndex + 1.U ) * X_SIZE.U
      }.otherwise{
        tileColumnIndex := 0.U 
        //col := 0.U
        when(tileRowIndex < (numRows-1.U)){
          tileRowIndex := tileRowIndex + 1.U 
          //row := (tileRowIndex + 1.U ) * Y_SIZE.U
        }.otherwise{
          tileRowIndex := 0.U 
        }
      }
      when((tileColumnIndex === (numColumns-1.U)) && (tileRowIndex === (numRows-1.U))){
        argbstate := finish
      }.otherwise{
        argbstate := compresstile
      }
    }
    is(finish){
      io.control.finish := true.B 
      argbstate := idle
    }
  }
}
class compressARGBtop extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val control = new controlIO
    //val flush = Input(Bool())
    val size = new sizeIO
    val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val compress = Module(new compressARGB)
  val ram = Module(new  ramtop)
  compress.io.axi <> ram.io.axi 
  compress.io.control <> io.control 
  io.len := compress.io.len
  compress.io.size <> io.size

  ram.io.axi.araddr := compress.io.axi.araddr + "x50000000".U 
  ram.io.axi.awaddr := compress.io.axi.awaddr + "x40000000".U 
  //io.econtrol.finish :=  jls.io.econtrol.finish
}
/******
该模块用于进行完整的压缩流程，除了对压缩数据写入文件，
还要将文件的头，以及tile的大小和信息也写入
对应/home/yangli/argblc/jingjiawei/src/main.cpp
compressARGB函数


******/
class compressARGBfile extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val axi = new AXIIO
    val control = new controlIO
    val size = new sizeIO
    //val 
    //val flush = Input(Bool())
    val compinf = new compIO
    //val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val jls = Module(new tilecompress)
  val jlsstart = Wire(Bool())
  jlsstart := false.B 
  jls.io.tcontrol.start := jlsstart
  io.axi <> jls.io.axi
  jls.io.compinf.inpixaddr := 0.U 
  jls.io.compinf.incompaddr := 0.U
  val jlsfinish = jls.io.tcontrol.finish

  //
  val height = RegInit(0.U(HEIGHT.W))
  val widthh  = RegInit(0.U(WIDTH.W))

  val numRows    = RegInit(0.U(HEIGHT.W))//RegNext(height/(X_SIZE.U))//RegInit(0.U(HEIGHT.W))
  val numColumns = RegInit(0.U(WIDTH.W)) //RegNext(widthh /(Y_SIZE.U))//RegInit(0.U(WIDTH.W))

  val CompressOffset = RegInit(0.U(AXIADDRWIDTH.W))
  val tileIndex = RegInit(0.U(AXIADDRWIDTH.W))
  val compaddr = RegInit(0.U(AXIADDRWIDTH.W))
  val pixaddr  = RegInit(0.U(AXIADDRWIDTH.W))
  //val tileIndex = RegInit(0.U(AXIADDRWIDTH.W))
  val HeaderCount = RegInit(0.U(3.W))
  //CompressOffset = numRows*numColumns*8


  val row    = Wire(UInt(HEIGHT.W))
  val col    = Wire(UInt(WIDTH.W)) 



  val x  = (jls.io.axi.araddr >> 2.U) % X_SIZE.U 
  //jls.io.addr.x 
  val y  = (jls.io.axi.araddr >> 2.U) / X_SIZE.U 


  // 4 bytes per pixel
  val rowStride = RegInit(0.U((WIDTH+2).W)) //RegNext(widthh<<2.U)


  //
  val tileRowIndex = RegInit(0.U(HEIGHT.W))
  val tileColumnIndex = RegInit(0.U(WIDTH.W))

  col := tileColumnIndex * X_SIZE.U 
  row := tileRowIndex * Y_SIZE.U 


  val tileSize = RegInit(0.U(AXIADDRWIDTH.W))

  val tilePosition = RegInit(0.U(AXIADDRWIDTH.W))
  val tileoneSize  = RegInit(0.U(AXIADDRWIDTH.W))

  io.compinf.len := tileSize + HeaderSize.U + CompressOffset
  io.control.finish := false.B
  //state control 

  val idle :: initial :: writeHeader ::writedata :: writeb :: compresstile:: writeTileInf :: update :: finish :: Nil = Enum(9)

  val argbstate = RegInit(idle)

  switch(argbstate){
    is(idle){
      when(io.control.start){
        height := io.size.height
        widthh  := io.size.widthh
        compaddr := io.compinf.incompaddr
        pixaddr  := io.compinf.inpixaddr
        argbstate := initial
      }
    }
    is(initial){
      numRows := height/(Y_SIZE.U)
      numColumns := widthh/(X_SIZE.U)
      tileRowIndex := 0.U 
      tileColumnIndex := 0.U
      tileSize := 0.U 
      rowStride := widthh << 2.U
      tilePosition := 0.U  
      tileoneSize  := 0.U 
      HeaderCount := 0.U 
      //row := 0.U 
      //col := 0.U 
      argbstate := writeHeader
    }
    is(writeHeader){
      CompressOffset := numRows*numColumns*8.U
      io.axi.awvalid := true.B 
      io.axi.awsize  := "b010".U 
      
      switch(HeaderCount){
        is(0.U){
          io.axi.awaddr := 0.U + compaddr
        }
        is(1.U){
          io.axi.awaddr := 4.U + compaddr
        }
        is(2.U){
          io.axi.awaddr := 8.U + compaddr
        }
        is(3.U){
          io.axi.awaddr := 12.U + compaddr
        }
        is(4.U){
          io.axi.awaddr := 16.U + compaddr
        }
        is(5.U){
          io.axi.awaddr := 20.U + compaddr
        }
      }
      when(io.axi.awready){
        argbstate := writedata
      }

    }
    is(writedata){
      io.axi.wvalid := true.B 
      
      switch(HeaderCount){
        is(0.U){
          io.axi.wdata := headerJLCD.U
          io.axi.wstrb := "b1111".U 
          io.axi.awsize  := "b010".U 
        }
        is(1.U){
          io.axi.wdata := widthh
          io.axi.wstrb := "b1111".U 
          io.axi.awsize  := "b010".U 
        }
        is(2.U){
          io.axi.wdata := height
          io.axi.wstrb := "b1111".U 
          io.axi.awsize  := "b010".U 
        }
        is(3.U){
          io.axi.wdata := X_SIZE.U
          io.axi.wstrb := "b1111".U 
          io.axi.awsize  := "b010".U 
        }
        is(4.U){
          io.axi.wdata := Y_SIZE.U 
          io.axi.wstrb := "b1111".U 
          io.axi.awsize  := "b010".U 
        }
        is(5.U){
          io.axi.wdata := CompressOffset>>3.U
          io.axi.wstrb := "b1111".U 
          io.axi.awsize  := "b010".U 
        }
        is(6.U){
          //有大小端序的问题
          io.axi.wdata := tileoneSize##tilePosition
          io.axi.wstrb := "xff".U
          io.axi.awsize  := "b011".U  
        }
      }
      when(io.axi.wready){
        argbstate := writeb
      }
    }
    is(writeb){
      io.axi.bready := true.B 
      when(io.axi.bvalid){
        
        when(HeaderCount < 5.U){
          argbstate := writeHeader 
          HeaderCount := HeaderCount + 1.U
        }.elsewhen(HeaderCount === 5.U){   
          argbstate := compresstile
          HeaderCount := HeaderCount + 1.U
        }.elsewhen(HeaderCount > 5.U){   
          argbstate := update
        }
      }
    }
    is(compresstile){
      jlsstart := true.B 
      when(jlsfinish){
        jlsstart := false.B 
        tileSize := tileSize + jls.io.compinf.len 
        tilePosition := tileSize 
        tileoneSize  := jls.io.compinf.len 
        argbstate := writeTileInf
        tileIndex := tileRowIndex * numColumns + tileColumnIndex
      }
      //process addr 
      //给tilecompress的Pixaddr给0
      //给tilecompress的compaddr给0
      val realrow = row + y 
      val realcol = col + x 
      val pixoffset = rowStride*realrow + realcol * 4.U
      io.axi.araddr := jls.io.axi.araddr(1,0) + pixoffset + pixaddr
      io.axi.awaddr := jls.io.axi.awaddr + tileSize + compaddr + CompressOffset+HeaderSize.U 

    }
    is(writeTileInf){
      io.axi.awvalid := true.B 
      io.axi.awsize  := "b011".U
      io.axi.awaddr :=  HeaderSize.U + compaddr +tileIndex*8.U
      when(io.axi.awready){
        argbstate := writedata
      }
    }
    is(update){
      when(tileColumnIndex < (numColumns-1.U)){
        tileColumnIndex := tileColumnIndex + 1.U 
        //col := (tileColumnIndex + 1.U ) * X_SIZE.U
      }.otherwise{
        tileColumnIndex := 0.U 
        //col := 0.U
        when(tileRowIndex < (numRows-1.U)){
          tileRowIndex := tileRowIndex + 1.U 
          //row := (tileRowIndex + 1.U ) * Y_SIZE.U
        }.otherwise{
          tileRowIndex := 0.U 
        }
      }
      when((tileColumnIndex === (numColumns-1.U)) && (tileRowIndex === (numRows-1.U))){
        argbstate := finish
      }.otherwise{
        argbstate := compresstile
      }
    }
    is(finish){
      io.control.finish := true.B 
      argbstate := idle
      
    }
  }
}

class compressARGBfiletop extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val control = new controlIO
    //val flush = Input(Bool())
    val size = new sizeIO
    val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val compress = Module(new compressARGBfile)
  val ram = Module(new  ramtop)
  compress.io.axi <> ram.io.axi 
  compress.io.control <> io.control 
  io.len := compress.io.compinf.len
  compress.io.size <> io.size
  compress.io.compinf.incompaddr := 0.U 
  compress.io.compinf.inpixaddr := 0.U 
  ram.io.axi.araddr := compress.io.axi.araddr + "x50000000".U 
  ram.io.axi.awaddr := compress.io.axi.awaddr + "x40000000".U 
  //io.econtrol.finish :=  jls.io.econtrol.finish
}