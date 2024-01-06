package decoder 
import chisel3._
import chisel3.util._
import scala.util.Random
/****
bitread 
opcode = 1 readbit 
opcode = 2 readbits
opcode = 3 flushRbits

****/
class BitReaderIO extends Bundle with COMMON {
  val flag      = Input(Bool())
  val opcode    = Input(UInt(2.W))
  val number    = Input(UInt(SIGNWIDTH.W))
  val bit       = Output(UInt(1.W))
  val bits      = Output(UInt(SIGNWIDTH.W))
  val finish    = Output(Bool())
}
class BitReader extends Module with COMMON {
  val io = IO(new Bundle {
    val readaddr  = Input(UInt(AXIADDRWIDTH.W))    
    val reader    = new BitReaderIO
    val control   = new controlIO
    //val flag      = Input(Bool())
    //val opcode    = Input(UInt(2.W))
    //val bit       = Input(UInt(1.W))
    //val bits      = Input(UInt(SIGNWIDTH.W))
    //val number    = Input(UInt(SIGNWIDTH.W))


    val axi       = new AXIIO
    val outaddr   = Output(UInt(AXIADDRWIDTH.W))
  })
  val bitmask   = RegInit("h80".U(BYTEWIDTH.W))
  val byte      = RegInit(0.U(BYTEWIDTH.W))
  val localbyte = byte & bitmask
  val len       = RegInit(0.U(AXIADDRWIDTH.W))
  val readaddr  = RegInit(0.U(AXIADDRWIDTH.W))
  io.outaddr := len
  io.control.finish := false.B
  io.reader.finish := io.control.finish 
  //io.awready :=  
  io.axi.awvalid := false.B 
  io.axi.awid    := 0.U
  io.axi.awaddr  := 0.U
  io.axi.awlen   := 0.U 
  io.axi.awsize  := 0.U 
  io.axi.awburst := 1.U

  //io.wready  
  io.axi.wvalid  :=  false.B 
  io.axi.wdata   :=  0.U
  io.axi.wstrb   :=  1.U
  io.axi.wlast   :=  false.B

  io.axi.bready  :=  false.B

  io.axi.arvalid :=  false.B 
  io.axi.arid    :=  0.U 
  io.axi.araddr  :=  readaddr
  io.axi.arlen   :=  0.U 
  io.axi.arsize  :=  0.U 
  io.axi.arburst :=  1.U
  
  io.axi.rready  :=  false.B
  val idle :: reading :: readBit :: readBits :: flushBits :: arread :: rread :: finish :: Nil = Enum(8)
 //   0    :: 1       :: 2       :: 3        :: 4         :: 5      :: 6    :: 7      :: 8
  val readstate = RegInit(idle)

// value for writevalue
  //val value     = RegInit(0.U(SIGNWIDTH.W))
  //val bytecnt   = RegInit(0.U(SIGNWIDTH.W))
// value for writebit writebits
  val bit         = RegInit(0.U(1.W))
  val bits        = RegInit(0.U(SIGNWIDTH.W))
  val reversebits = RegInit(0.U(SIGNWIDTH.W))//Reverse(bits)
  val number      = RegInit(0.U(SIGNWIDTH.W))
  val shiftn      = number-1.U
  val shifval     = (bits >> shiftn)


  io.reader.bit  := bit 
  io.reader.bits := bits
  switch(readstate){
    is(idle){
      //首先读byte
      when(io.control.start){
        readstate := arread 
        readaddr  := io.readaddr
        len := 0.U 
      }
    }
    is(reading){
      //io.control.writing := true.B
      when(io.reader.flag){
        when(io.reader.opcode === 1.U){
          readstate := readBit
          //bit := io.reader.bit 
        }.elsewhen(io.reader.opcode === 2.U){
          readstate   := readBits
          bits    := 0.U
          reversebits :=  Reverse(io.reader.bits)
          number  := io.reader.number 
        }.elsewhen(io.reader.opcode === 3.U){
          readstate := flushBits 
        }.elsewhen(io.reader.opcode === 0.U){
          readstate := idle
        }
      }
    }
    is(readBit){
      when(localbyte =/= 0.U){
        bit := 1.U 
      }.otherwise{
        bit := 0.U 
      }

      when(number =/= 0.U) {
        number := number - 1.U 
        when(localbyte =/= 0.U){
          bits := bits(BYTEWIDTH-2,0)## 1.U 
        }.otherwise{
          bits := bits(BYTEWIDTH-2,0)## 0.U 
        }
      }

      bitmask := bitmask >> 1.U
      when(bitmask === 1.U){
        readstate := arread
        bitmask := "h80".U 
      }.elsewhen(number =/= 0.U){
        //number := number - 1.U
        readstate := readBits
      }.otherwise{
        readstate := finish
      }
    }
    is(readBits){
      when(number =/= 0.U){
        //number := number - 1.U
        //bits := bits >> 1.U 
        //reversebits := reversebits >> 1.U
        //bit  := shifval(0) //reversebits(0)
        readstate := readBit
      }.otherwise{
        readstate := finish
      }
    }
    is(flushBits){
      when(bitmask < "h80".U){
        //readaddr := readaddr + ADDRADD.U 
        bitmask := "h80".U
        readstate := arread
      }.otherwise{
        readstate := finish 
      }
    }
    is(arread){
      io.axi.arvalid := true.B 
      when(io.axi.arready){
        readstate := rread
      }
    }
    is(rread){
      io.axi.rready := true.B
      when(io.axi.rvalid){
        readaddr := readaddr + ADDRADD.U 
        len := len + 1.U 
        byte := io.axi.rdata
        when(number =/= 0.U){
          readstate := readBits
        }.otherwise{
          readstate := finish
        }
      }
    }
    is(finish){
      io.control.finish := true.B
      readstate := reading
    }
  }

}
/****
GolombdeCoding 
Sequatial
****/
class golombdeIO extends Bundle with COMMON{
  //val limit     = Input(UInt(SIGNWIDTH.W))
  val lim       = Input(UInt(SIGNWIDTH.W))
  val k         = Input(UInt(SIGNWIDTH.W))
  val value     = Output(UInt(SIGNWIDTH.W))
  val gcontrol  = new controlIO
}
class GolombdeCoding extends Module with COMMON {
  val io = IO(new Bundle {
    val golomb = new golombdeIO
    val reader = Flipped(new BitReaderIO)
  })

  val idle :: readBit :: getnumber :: compare :: readBits1 :: getfinal1 :: readBits2 :: getfinal2:: finish :: Nil = Enum(9) 
  //  0    :: 1       :: 2         :: 3       :: 4         :: 5         :: 6         :: 7        :: 8      ::
  
  val golombstate = RegInit(idle) 


  val localbool  = RegInit(0.U(1.W))
  val zeronumber = RegInit(0.U(SIGNWIDTH.W)) 

  val finalval   = RegInit(0.U(SIGNWIDTH.W))

  val lowkbits   = RegInit(0.U(SIGNWIDTH.W))

  val number  = Wire(UInt(SIGNWIDTH.W))
  number := qbpp.U 
  //val value  = Wire(UInt(SIGNWIDTH.W))
  val k      = RegInit(0.U(SIGNWIDTH.W))
  val lim    = RegInit(0.U(SIGNWIDTH.W))
  val flag    = RegInit(false.B)  
  val opcode  = RegInit(0.U(2.W))  



  io.golomb.gcontrol.finish :=  false.B 
  io.reader.flag   := flag    
  io.reader.opcode := opcode  
  io.golomb.value := finalval 
  io.reader.number := number  

  //val controlflag = RegInit(0.U.asTypeOf())  
  //val writing =   
  switch(golombstate){
    is(idle){
      when(io.golomb.gcontrol.start) {
        golombstate := readBit
        flag := true.B  
        k    := io.golomb.k
        lim  := io.golomb.lim
        zeronumber := 0.U  
        opcode     := 1.U 
      }    
    }
    is(readBit){
      when(io.reader.finish){
        localbool := io.reader.bit 
        when(io.reader.bit === 0.U){
          zeronumber := zeronumber + 1.U
          golombstate := readBit 
        }.elsewhen(io.reader.bit === 1.U){
          golombstate := compare
          flag := false.B 
          opcode := 2.U 
        }
      }  
    }
    is(compare){
      //when(!io.control.writing){
      //  flag := false.B
      //}
      when(zeronumber < (lim-qbpp.U-1.U) ){
        golombstate := readBits1 
        flag        := true.B 
        finalval    := zeronumber << k 
      }.otherwise{
        golombstate := readBits2 
        flag        := true.B 
      }
    }
    is(readBits1){
      number := k 
      when(io.reader.finish){
        lowkbits := io.reader.bits 
        flag := false.B
        golombstate := getfinal1 
      }
    }
    is(getfinal1){
      golombstate := finish 
      finalval := finalval + lowkbits
    }
    is(readBits2){
      when(io.reader.finish){
        flag  := false.B 
        finalval := io.reader.bits 
        golombstate := getfinal2
      }
    }
    is(getfinal2){
      golombstate := finish
      finalval := finalval + 1.U 
    }
    is(finish){
      io.golomb.gcontrol.finish := true.B 
      //io.finish := true.B
      golombstate := idle 
    }
  }

}
/*************
pix write

此模块用于向指定的地址写入像素值
执行的功能类似于

GET2D(localimg,xsize,*y,*x) = RUNval ;


**************/

class updateSample extends Module with COMMON {
  val io = IO(new Bundle {

    //input
    val inaddr    = Flipped((new addrIO))
    //val update    = Input(Bool())
    val inpix     = Input(UInt(COLORWIDTH.W))
    //val pixvalid  = Input(Bool())
    ////output
    ////val pix = (new pixIO)
    ////val EOLine = Output(Bool())
    //val addrvalid = Output(Bool())
    //val getvalid = Output(Bool())
    val control = new controlIO
    val axi     = new AXIIO 
    //val outaddr = (new addrIO)
  })

  val x = io.inaddr.x 
  val y = io.inaddr.y 
  io.control.finish := false.B 

  io.axi.awvalid := false.B 
  io.axi.awid    := 0.U
  io.axi.awaddr  := x+y*X_SIZE.U
  io.axi.awlen   := 0.U 
  io.axi.awsize  := 0.U 
  io.axi.awburst := 1.U

  //io.wready  
  io.axi.wvalid  :=  false.B 
  io.axi.wdata   :=  io.inpix
  io.axi.wstrb   :=  1.U
  io.axi.wlast   :=  true.B

  io.axi.bready  :=  false.B

  io.axi.arvalid :=  false.B 
  io.axi.arid    :=  0.U 
  io.axi.araddr  :=  0.U 
  io.axi.arlen   :=  0.U 
  io.axi.arsize  :=  0.U 
  io.axi.arburst :=  1.U
  
  io.axi.rready  :=  false.B

  val idle :: awwrite :: wwrite ::bwrite ::Nil = Enum(4)

  val updatestate = RegInit(idle) 
  switch(updatestate){
    is(idle ){
      when(io.control.start){
        updatestate := awwrite
      }
    }
    is(awwrite){
      io.axi.awvalid := true.B 
      when(io.axi.awready){
        updatestate := wwrite
      }
    }
    is(wwrite){
      io.axi.wvalid := true.B 
      when(io.axi.wready){
        updatestate := bwrite 
      }
    }
    is(bwrite){
      io.axi.bready := true.B 
      when(io.axi.bvalid ){
        updatestate := idle
        io.control.finish := true.B 
      }
    }
  }


}