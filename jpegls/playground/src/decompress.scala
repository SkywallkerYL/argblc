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

/******
该模块用于进行完整解压流程，读取压缩文件的数据，
读取文件头获取图片的大小信息
1.支持自主配置图片的大小，
2.支持从文件读取图片的大小，//已实现
还要将文件的头，以及tile的大小和信息也写入
对应/home/yangli/argblc/jingjiawei/src/main.cpp
decompressARGB函数


******/
class decompressARGBfile extends Module with COMMON {
  val io = IO(new Bundle {
    //  
    // 
    //readbit
    val axi = new AXIIO
    // readpix writepix 
    val pixaxi = new AXIIO 
    val control = new controlIO
    val size = new sizeIO
    //val 
    //val flush = Input(Bool())
    val compinf = new compIO
    //val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val jls = Module(new tiledecompress)
  val jlsstart = Wire(Bool())
  jlsstart := false.B 
  jls.io.tcontrol.start := jlsstart
  io.axi <> jls.io.axi
  io.pixaxi <> jls.io.pixaxi
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



  val xr  = (jls.io.pixaxi.araddr >> 2.U) % X_SIZE.U 
  //jls.io.addr.x 
  val yr  = (jls.io.pixaxi.araddr >> 2.U) / X_SIZE.U 

  val xw  = (jls.io.pixaxi.awaddr >> 2.U) % X_SIZE.U 
  //jls.io.addr.x 
  val yw  = (jls.io.pixaxi.awaddr >> 2.U) / X_SIZE.U 

  // 4 bytes per pixel
  val rowStride = RegInit(0.U((WIDTH+2).W)) //RegNext(widthh<<2.U)


  //
  val tileRowIndex = RegInit(0.U(HEIGHT.W))
  val tileColumnIndex = RegInit(0.U(WIDTH.W))

  col := tileColumnIndex * X_SIZE.U 
  row := tileRowIndex * Y_SIZE.U 


  val tileSize = RegInit(0.U(AXIADDRWIDTH.W))

  val tileDataOffset = RegInit(0.U(AXIADDRWIDTH.W))
  val tilePosition = RegInit(0.U(AXIADDRWIDTH.W))
  val tileoneSize  = RegInit(0.U(AXIADDRWIDTH.W))

  io.compinf.len := tileSize + HeaderSize.U + CompressOffset
  io.control.finish := false.B
  //state control 

  val idle :: initial :: readHeader :: readdata :: updatesizeinf :: readTileInf :: decompresstile :: update :: updatetileind :: finish :: Nil = Enum(10)
  //  0    :: 1       :: 2          :: 3        :: 4             :: 5           :: 6              :: 7      :: 8             :: 9
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
      HeaderCount := 3.U 
      //row := 0.U 
      //col := 0.U 
      argbstate := updatesizeinf
    }
    is(readHeader){
     
      io.axi.arvalid := true.B 
      io.axi.arsize  := "b010".U 
      
      switch(HeaderCount){
        is(0.U){
          io.axi.araddr := 0.U + compaddr
        }
        is(1.U){
          io.axi.araddr := 4.U + compaddr
        }
        is(2.U){
          io.axi.araddr := 8.U + compaddr
        }
        is(3.U){
          io.axi.araddr := 20.U + compaddr
        }
      }
      when(io.axi.arready){
        argbstate := readdata
      }

    }
    is(readdata){
      io.axi.rready := true.B 
      
      switch(HeaderCount){
        is(0.U){
          io.axi.wdata := headerJLCD.U
          io.axi.wstrb := "b1111".U 
          io.axi.awsize  := "b010".U 
        }
        is(1.U){
          widthh := io.axi.rdata 
          io.axi.arsize  := "b010".U 
        }
        is(2.U){
          height := io.axi.rdata 
          io.axi.arsize  := "b010".U 
        }
        //tile的大小不可设置，因为是写死的
        // 读完height和widthh后，就可以计算tilecount = numRows * numcolumns
        is(3.U){
          //有大小端序的问题
          tileDataOffset := io.axi.rdata(31,0)
          //io.axi.wdata := tileoneSize##tilePosition
          io.axi.arsize  := "b011".U 
        }
      }
      when(io.axi.rvalid){
        when(HeaderCount < 2.U){
          argbstate := readHeader 
          HeaderCount := HeaderCount + 1.U
        }.elsewhen(HeaderCount === 2.U){   
          argbstate := updatesizeinf
          HeaderCount := HeaderCount + 1.U
        }.elsewhen(HeaderCount > 2.U){   
          argbstate := decompresstile
        }
      }
    }
    is(updatesizeinf){
      //tilecount = numRows * numColumns
      numRows := height/(Y_SIZE.U)
      numColumns := widthh/(X_SIZE.U)
      rowStride := widthh << 2.U
      tileIndex := 0.U 
      CompressOffset := height*widthh*8.U/(Y_SIZE.U)/(X_SIZE.U)
      argbstate := readTileInf 
    }
    is(readTileInf){
      //读取这个当前这个tile数据所在的偏移，和他的tile对应的数据的bytes数，
      //但是实际解压过程好像没有用到这个bytes
      io.axi.arvalid := true.B 
      io.axi.arsize  := "b011".U 
      io.axi.araddr :=  HeaderSize.U + compaddr +tileIndex*8.U
      when(io.axi.arready){
        argbstate := readdata
      }
    }
    is(decompresstile){
      jlsstart := true.B 
      when(jlsfinish){
        jlsstart := false.B 
        //tileSize := tileSize + jls.io.compinf.len 
        //tilePosition := tileSize 
        //tileoneSize  := jls.io.compinf.len 
        argbstate := update
        
      }
      //process addr 
      //给tilecompress的Pixaddr给0
      //给tilecompress的compaddr给0
      val realrow = row + Mux(jls.io.pixaxi.arvalid,yr,yw) 
      val realcol = col + Mux(jls.io.pixaxi.arvalid,xr,xw) 
      val pixoffset = rowStride*realrow + realcol * 4.U
      io.pixaxi.araddr := jls.io.pixaxi.araddr(1,0) + pixoffset + pixaddr
      io.pixaxi.awaddr := jls.io.pixaxi.awaddr(1,0) + pixoffset + pixaddr 

      io.axi.araddr := jls.io.axi.araddr + CompressOffset + HeaderSize.U + tileDataOffset

    }
    is(update){
      //tileIndex := tileRowIndex * numColumns + tileColumnIndex
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
        argbstate := updatetileind
      }
    }
    is(updatetileind){
      tileIndex := tileRowIndex * numColumns + tileColumnIndex
      argbstate := readTileInf
    }
    is(finish){
      io.control.finish := true.B 
      argbstate := idle
      
    }
  }
}

class decompressARGBfiletop extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val control = new controlIO
    //val flush = Input(Bool())
    val size = new sizeIO
    val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val decompress = Module(new decompressARGBfile)
  val ram = Module(new  ramtop)
  val pixram = Module(new  ramtop)
  decompress.io.axi <> ram.io.axi 
  decompress.io.pixaxi <> pixram.io.axi
  decompress.io.control <> io.control 
  decompress.io.size <> io.size
  decompress.io.compinf.incompaddr := 0.U 
  decompress.io.compinf.inpixaddr := 0.U 
  pixram.io.axi.araddr := decompress.io.pixaxi.araddr + "x50000000".U 
  pixram.io.axi.awaddr := decompress.io.pixaxi.awaddr + "x50000000".U 
  ram.io.axi.araddr := decompress.io.axi.araddr + "x40000000".U 
  io.len := 0.U 
  //io.econtrol.finish :=  jls.io.econtrol.finish
}