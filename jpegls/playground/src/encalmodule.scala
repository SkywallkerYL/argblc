package decoder 
import chisel3._
import chisel3.util._
import scala.util.Random
class isNear extends Module with COMMON {
  val io = IO(new Bundle {
    val near        = Input(SInt(SIGNWIDTH.W))
    val v1          = Input(SInt(SIGNWIDTH.W))
    val v2          = Input(SInt(SIGNWIDTH.W))
    val isNear      = Output(Bool())
  })
  val flag1 = (io.v2-io.v1) <= io.near 
  val flag2 = (io.v1-io.v2) <= io.near 
  io.isNear := flag1 & flag2  
}
object isNear {
  def apply(near : SInt,v1 : SInt ,v2: SInt) : Bool ={
    val inst = Module(new isNear)
    inst.io.near := near 
    inst.io.v1   := v1 
    inst.io.v2   := v2 
    inst.io.isNear
  }
}
class pixIO extends Bundle with COMMON{
  val Ra = Output(SInt(SIGNWIDTH.W))
  val Rb = Output(SInt(SIGNWIDTH.W))
  val Rc = Output(SInt(SIGNWIDTH.W))
  val Rd = Output(SInt(SIGNWIDTH.W))
}
class addrIO extends Bundle with COMMON{
  val x = Output(UInt(XADDRWIDTH.W))
  val y = Output(UInt(YADDRWIDTH.W))
}
class SampleIO extends Bundle with COMMON{
  //input
  //val inaddr    = Flipped((new addrIO))
  val update    = Input(Bool())
  //val inpix     = Input(UInt(COLORWIDTH.W))
  //val pixvalid  = Input(Bool())
  //output
  val pix = (new pixIO)
  val EOLine = Output(Bool())
  //val addrvalid = Output(Bool())
  val getvalid = Output(Bool())
  //val outaddr = (new addrIO)
}
class GetpixIO extends Bundle with COMMON{
  val inpix     = Input(UInt(COLORWIDTH.W))
  val pixvalid  = Input(Bool())
  val addrvalid = Output(Bool())
}
/***
该模块用于获取像素 
***/
/***

该模块用于坐标的更新以及对像素进行填充

该模块每次读数据时addrvalid 拉高
pixvalid表示该数据得到

Sequantial logic
***/
class GetNextSample extends Module with COMMON {
  val io = IO(new Bundle {
    val sample = new SampleIO
    val getpix = new GetpixIO
    //input
    val inaddr    = Flipped((new addrIO))
    //val update    = Input(Bool())
    //val inpix     = Input(UInt(COLORWIDTH.W))
    //val pixvalid  = Input(Bool())
    ////output
    ////val pix = (new pixIO)
    ////val EOLine = Output(Bool())
    //val addrvalid = Output(Bool())
    //val getvalid = Output(Bool())
    val outaddr = (new addrIO)
  })
  
  val Ra = RegInit(0.U(COLORWIDTH.W))
  val Rb = RegInit(0.U(COLORWIDTH.W))
  val Rc = RegInit(0.U(COLORWIDTH.W))
  val Rd = RegInit(0.U(COLORWIDTH.W))
  val EOLine = RegInit(0.U(1.W))

  io.sample.pix.Ra := (0.U(1.W) ##Ra).asSInt
  io.sample.pix.Rb := (0.U(1.W) ##Rb).asSInt
  io.sample.pix.Rc := (0.U(1.W) ##Rc).asSInt
  io.sample.pix.Rd := (0.U(1.W) ##Rd).asSInt
  io.sample.EOLine := EOLine

  io.getpix.addrvalid := false.B
  io.sample.getvalid  := false.B

  val x = io.inaddr.x
  val y = io.inaddr.y
  val datain = io.getpix.inpix
  val datavalid = io.getpix.pixvalid
  io.outaddr.x := io.inaddr.x 
  io.outaddr.y := io.inaddr.y

  /***
  更新各个像素点，在对应的状态更新要读取的地址

  ***/
  val idle :: getRb :: getRd :: getRc1 :: getRa :: getRc2 :: finish :: Nil = Enum(7)

  val SampleState = RegInit(idle)

  switch(SampleState){
    is(idle){
      when(io.sample.update){
        when(y > 0.U){
          SampleState := getRb 
        }.elsewhen(x === 0.U){
          SampleState := getRc1
        }.otherwise{
          SampleState := getRa
        }
        Ra := 0.U
        Rb := 0.U
        Rc := 0.U
        Rd := 0.U
        EOLine := (x === (X_SIZE-1).U).asUInt
      }
    }
    is(getRb){
      //读数据 Rb
      io.outaddr.x := x 
      io.outaddr.y := y - 1.U  
      io.getpix.addrvalid := true.B

      //得到数据
      when(datavalid){
        Rb := datain
        when((x < (X_SIZE-1).U)){
          SampleState := getRd
        }.elsewhen((x === 0.U)){
          SampleState := getRc1
        }.otherwise{
          SampleState := getRa
        }
        Rd := datain
      }
    }
    is(getRd){
      io.outaddr.x := x + 1.U
      io.outaddr.y := y - 1.U 
      io.getpix.addrvalid := true.B

      //得到数据
      when(datavalid){
        Rd := datain
        when(x === 0.U){
          SampleState := getRc1
        }.otherwise{
          SampleState := getRa
        }
      }
    }
    is(getRc1){
      Ra := Rb 
      when(y > 1.U){
        io.outaddr.x := x 
        io.outaddr.y := y - 2.U  
        io.getpix.addrvalid := true.B
      }
      when(!io.getpix.addrvalid){
        SampleState := finish
      }.elsewhen(datavalid){
        Rc := datain
        SampleState := finish
      }
    }
    is(getRa){
      io.outaddr.x := x - 1.U  
      io.outaddr.y := y
      io.getpix.addrvalid := true.B

      //得到数据
      when(datavalid){
        Ra := datain
        when( y > 0.U){
          SampleState := getRc2
        }.otherwise{
          SampleState := finish
        }
      }
    }
    is(getRc2){
      io.outaddr.x := x - 1.U
      io.outaddr.y := y - 1.U 
      io.getpix.addrvalid := true.B

      //得到数据
      when(datavalid){
        Rc := datain
        SampleState := finish
      }
    }
    is(finish){
      io.sample.getvalid := true.B 
      SampleState := idle
    }
  }

}
/****
gradient QUANTI

combinational logic
****/
class gradientQuantize extends Module with COMMON {
  val io = IO(new Bundle {
    //val near        = Input(SInt(SIGNWIDTH.W))
    val value       = Input(SInt(SIGNWIDTH.W))
    //val v2          = Input(SInt(SIGNWIDTH.W))
    val Q      = Output(SInt(SIGNWIDTH.W))
  })
  val sign = io.value(SIGNWIDTH-1)
  val absvalue = Mux(sign===1.U,~io.value + 1.S,io.value)
  io.Q := 0.S
  when(absvalue >= t3.S){
    io.Q := Mux(sign === 1.U, -4.S(SIGNWIDTH.W),4.S(SIGNWIDTH.W))
  }.elsewhen(absvalue >= t2.S){
    io.Q := Mux(sign === 1.U, -3.S(SIGNWIDTH.W),3.S(SIGNWIDTH.W))
  }.elsewhen(absvalue >= t1.S){
    io.Q := Mux(sign === 1.U, -2.S(SIGNWIDTH.W),2.S(SIGNWIDTH.W))
  }.elsewhen(absvalue >  NEAR.S){
    io.Q := Mux(sign === 1.U, -1.S(SIGNWIDTH.W),1.S(SIGNWIDTH.W))
  }.otherwise{
    io.Q := 0.S
  }  
}

/****
getQ

combinational logic
****/
class getQ extends Module with COMMON {
  val io = IO(new Bundle {
    val pix = Flipped(new pixIO)
    val Q   = Output((SInt((SIGNWIDTH+1).W)))
    val sign= Output(UInt(1.W))
  })
  val gradientQuanti = Seq.fill(3)(Module(new gradientQuantize))  
  
  val inQ  = VecInit(Seq.fill(3)(0.S(SIGNWIDTH.W))) 
  val outQ = VecInit(Seq.fill(3)(0.S((SIGNWIDTH+1).W))) 
  val inR  = VecInit(Seq.fill(3)(0.S(SIGNWIDTH.W))) 

  inR(0) := io.pix.Rd-io.pix.Rb
  inR(1) := io.pix.Rb-io.pix.Rc 
  inR(2) := io.pix.Rc-io.pix.Ra 

  for(i <- 0 until 3){
    gradientQuanti(i).io.value := inR(i)
    inQ(i) := gradientQuanti(i).io.Q 
  }

  when(inQ(0) < 0.S || (inQ(0)=== 0.S && (inQ(1) < 0.S)) || ((inQ(0) === 0.S) &&(inQ(1) === 0.S)&&(inQ(2) < 0.S))){
    io.sign := 1.U 
  }.otherwise{
    io.sign := 0.U 
  }
  for(i <- 0 until 3) {
    outQ(i) := Mux(io.sign===1.U,0.S-inQ(i),inQ(i)) 
  }
  io.Q := 81.S*outQ(0) + 9.S*outQ(1) + outQ(2)
}

/****
predict
combinational logic
****/
class predict extends Module with COMMON {
  val io = IO(new Bundle {
    val a = Input(SInt(SIGNWIDTH.W))
    val b = Input(SInt(SIGNWIDTH.W))
    val c = Input(SInt(SIGNWIDTH.W))
    val Px = Output(SInt((SIGNWIDTH+1).W))
  })
  when(io.c >= MAXH(io.a,io.b)){
    io.Px := MINH(io.a,io.b)
  }.elsewhen(io.c <= MINH(io.a,io.b)){
    io.Px := MAXH(io.a,io.b)
  }.otherwise{
    io.Px := io.a+io.b-io.c
  }
}

class logicIO extends Bundle with COMMON{
  val in = Input(SInt(SIGNWIDTH.W))
  val out= Output(SInt(SIGNWIDTH.W))
}
/****
quantize errval
combinational logic
****/
class errvalquanti extends Module with COMMON {
  val io = IO(new Bundle {
    val quanti = new logicIO
  })
  when(io.quanti.in > 0.S){
    io.quanti.out := (io.quanti.in + NEAR.S)/(2.S*NEAR.S+1.S)
  }.otherwise{
    io.quanti.out := -(-io.quanti.in + NEAR.S)/(2.S*NEAR.S+1.S)
  }
}
/****
Compute Rx
combinational logic
****/
class predictIO extends Bundle with COMMON{
  val Px     = Input(SInt(SIGNWIDTH.W))
  val errval = Input(SInt(SIGNWIDTH.W))
  val sign   = Input(UInt(1.W))
  val Rx     = Output(SInt(SIGNWIDTH.W))
}
class computeRx extends Module with COMMON {
  val io = IO(new Bundle {
    val predict = new predictIO
    //val Px     = Input(SInt(SIGNWIDTH.W))
    //val errval = Input(SInt(SIGNWIDTH.W))
    //val sign   = Input(UInt(1.W))
    //val Rx     = Output(SInt(SIGNWIDTH.W))
  })
  val Rx = Wire(SInt((SIGNWIDTH1).W))
  val extPx = Wire(SInt((SIGNWIDTH1).W))
  extPx :=  io.predict.Px
  val extmulti = Wire(SInt((SIGNWIDTH1).W))
  extmulti := io.predict.errval
  val multi = extmulti*(2.S*NEAR.S + 1.S)
  Rx := extPx + Mux(io.predict.sign === 1.U,-multi,multi)
  when(Rx < 0.S){
    io.predict.Rx := 0.S
  }.elsewhen(Rx > MAXVAL.S){
    io.predict.Rx := MAXVAL.S 
  }.otherwise{
    io.predict.Rx := Rx 
  }
}
class computePx extends Module with COMMON {
  val io = IO(new Bundle {
    val predict = new predictIO
    //val Px     = Input(SInt(SIGNWIDTH.W))
    //val errval = Input(SInt(SIGNWIDTH.W))
    //val sign   = Input(UInt(1.W))
    //val Rx     = Output(SInt(SIGNWIDTH.W))
  })
  val Rx = Wire(SInt((SIGNWIDTH1).W))
  val extPx = Wire(SInt((SIGNWIDTH1).W))
  extPx :=  io.predict.Px
  val extmulti = Wire(SInt((SIGNWIDTH1).W))
  extmulti := io.predict.errval
  //val multi = io.predict.errval
  Rx := extPx + Mux(io.predict.sign === 1.U,-extmulti,extmulti)
  when(Rx < 0.S){
    io.predict.Rx := 0.S
  }.elsewhen(Rx > MAXVAL.S){
    io.predict.Rx := MAXVAL.S 
  }.otherwise{
    io.predict.Rx := Rx 
  }
}
/****
ModRange
combinational logic
****/
class ModRange extends Module with COMMON {
  val io = IO(new Bundle {
    val modrange = new logicIO
  })
  val add = io.modrange.in + RANGE.S 
  val sub = io.modrange.in - RANGE.S 
  when(io.modrange.in < 0.S){
    when(add >= (((RANGE+1)/2).S)){
      io.modrange.out := io.modrange.in
    }.otherwise{
      io.modrange.out := add 
    }
  }.elsewhen(io.modrange.in >= ((RANGE+1)/2).S){
    io.modrange.out := sub 
  }.otherwise{
    io.modrange.out := io.modrange.in
  }
}


class controlIO extends Bundle with COMMON{
  val start  = Input(Bool())
  val finish = Output(Bool())
}
class lgIO extends Bundle with COMMON{
  val AQ      = Input(SInt(TEXTWIDTH.W))
  val NQ      = Input(SInt(TEXTWIDTH.W))
  val lcontrol = new controlIO
  val k           = Output(SInt(SIGNWIDTH.W))
}
/*******
LG 
Sequantial logic
*******/
class LG extends Module with COMMON {
  val io = IO(new Bundle {
    val lg = new lgIO
    //val AQ      = Input(UInt(SIGNWIDTH.W))
    //val NQ      = Input(UInt(SIGNWIDTH.W))
    ////val getk    = Input(Bool())
    //val lcontrol = new controlIO
    //val k           = Output(UInt(SIGNWIDTH.W))
    //val outvalid    = Output(Bool())
  })
  val k = RegInit(0.S(SIGNWIDTH.W)) 
  val AQ = RegInit(0.S(TEXTWIDTH.W))
  val NQ = RegInit(0.S(TEXTWIDTH.W))

  val shiftval = Wire(SInt((TEXTWIDTH+1).W))
  shiftval := NQ << k.asUInt

  io.lg.k := k
  io.lg.lcontrol.finish := false.B

  val idle :: shift :: finish :: Nil = Enum(3)
  val state = RegInit(idle)
  switch(state){
    is(idle){
      when(io.lg.lcontrol.start) {
        state := shift 
        k := 0.S 
        AQ := io.lg.AQ
        NQ := io.lg.NQ
      }
    }
    is(shift){
      when((shiftval)<AQ){
        k := k + 1.S 
      }.otherwise{
        state := finish
      }
    }
    is(finish){
      io.lg.lcontrol.finish := true.B
      state := idle
    }
  }
}
/****
bitwriter 
opcode = 1 writebit 
opcode = 2 writebits
opcode = 3 flushbits

****/
class BitWriterIO extends Bundle with COMMON {
  val flag      = Input(Bool())
  val opcode    = Input(UInt(2.W))
  val bit       = Input(UInt(1.W))
  val bits      = Input(UInt(SIGNWIDTH.W))
  val number    = Input(UInt(SIGNWIDTH.W))
  val writing   = Output(Bool())
}
class BitWriter extends Module with COMMON {
  val io = IO(new Bundle {
    val writeaddr = Input(UInt(AXIADDRWIDTH.W))
    val start     = Input(Bool())
    
    val control   = new BitWriterIO
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
  val len       = RegInit(0.U(AXIADDRWIDTH.W))
  val writeaddr = RegInit(0.U(AXIADDRWIDTH.W))
  io.outaddr := len
  io.control.writing := false.B
  //io.awready :=  
  io.axi.awvalid := false.B 
  io.axi.awid    := 0.U
  io.axi.awaddr  := writeaddr
  io.axi.awlen   := 0.U 
  io.axi.awsize  := 0.U 
  io.axi.awburst := 1.U

  //io.wready  
  io.axi.wvalid  :=  false.B 
  io.axi.wdata   :=  byte
  io.axi.wstrb   :=  1.U
  io.axi.wlast   :=  false.B

  io.axi.bready  :=  false.B

  io.axi.arvalid :=  false.B 
  io.axi.arid    :=  0.U 
  io.axi.araddr  :=  0.U 
  io.axi.arlen   :=  0.U 
  io.axi.arsize  :=  0.U 
  io.axi.arburst :=  1.U
  
  io.axi.rready  :=  false.B
  val idle :: writing :: writeBit :: writeBits :: flushBits :: awwrite :: write :: brwite :: finish :: Nil = Enum(9)
 //   0    :: 1       :: 2        :: 3         :: 4         :: 5       :: 6     :: 7      :: 8
  val state = RegInit(idle)

// value for writevalue
  //val value     = RegInit(0.U(SIGNWIDTH.W))
  //val bytecnt   = RegInit(0.U(SIGNWIDTH.W))
// value for writebit writebits
  val bit       = RegInit(0.U(1.W))
  val bits      = RegInit(0.U(SIGNWIDTH.W))
  val reversebits = RegInit(0.U(SIGNWIDTH.W))//Reverse(bits)
  val number    = RegInit(0.U(SIGNWIDTH.W))
  val shiftn    = number-1.U
  val shifval   = (bits >> shiftn)


  switch(state){
    is(idle){
      when(io.start){
        state := writing 
        writeaddr := io.writeaddr
        len := 0.U 
      }
    }
    is(writing){
      //io.control.writing := true.B
      when(io.control.flag){
        when(io.control.opcode === 1.U){
          state := writeBit
          bit := io.control.bit 
        }.elsewhen(io.control.opcode === 2.U){
          state   := writeBits
          bits    := io.control.bits 
          reversebits :=  Reverse(io.control.bits)
          number  := io.control.number 
        }.elsewhen(io.control.opcode === 3.U){
          state := flushBits 
        }.elsewhen(io.control.opcode === 0.U){
          state := idle
        }
      }
    }
    is(writeBit){
      when(bit === 1.U){
        byte := byte | bitmask
      }
      bitmask := bitmask >> 1.U
      when(bitmask === 1.U){
        state := awwrite
        bitmask := "h80".U 
      }.elsewhen(number =/= 0.U){
        state := writeBits
      }.otherwise{
        state := finish
      }
    }
    is(writeBits){
      when(number =/= 0.U){
        number := number - 1.U
        //bits := bits >> 1.U 
        //reversebits := reversebits >> 1.U
        bit  := shifval(0) //reversebits(0)
        state := writeBit
      }.otherwise{
        state := finish
      }
    }
    is(flushBits){
      when(bitmask < "h80".U){
        state := awwrite
        bitmask := "h80".U
      }.otherwise{
        state := finish 
      }
    }
    is(awwrite){
      io.axi.awvalid := true.B 
      when(io.axi.awready){
        state := write
      }
    }
    is(write){
      io.axi.wvalid := true.B
      io.axi.wlast  := true.B
      when(io.axi.wready){
        state := brwite
      }
    }
    is(brwite){
      io.axi.bready := true.B 
      when(io.axi.bvalid){
        writeaddr := writeaddr + ADDRADD.U 
        len := len + 1.U 
        byte := 0.U
        when(number =/= 0.U) {
          state := writeBits
        }.otherwise{
          state := finish
        }
      }
    }
    is(finish){
      io.control.writing := true.B
      state := writing
    }
  }

}
/****
GolombCoding 
Sequatial
****/
class golombIO extends Bundle with COMMON{
  //val limit     = Input(UInt(SIGNWIDTH.W))
  val lim       = Input(UInt(SIGNWIDTH.W))
  val value     = Input(UInt(SIGNWIDTH.W))
  val k         = Input(UInt(SIGNWIDTH.W))
  val gcontrol  = new controlIO
}
class GolombCoding extends Module with COMMON {
  val io = IO(new Bundle {
    val golomb = new golombIO
    val control   = Flipped(new BitWriterIO)
  })

  val idle :: compare :: write1 :: write2 :: write3 :: write4 :: write5 :: write6 :: finish :: Nil = Enum(9)
  val state = RegInit(idle) 
  val value1 = RegInit(0.U(SIGNWIDTH.W))
  val value2 = RegInit(0.U(SIGNWIDTH.W))
  val value  = RegInit(0.U(SIGNWIDTH.W))
  val k      = RegInit(0.U(SIGNWIDTH.W))
  val flag    = RegInit(false.B)  
  val opcode  = RegInit(0.U(2.W))  
  val bit     = RegInit(0.U(1.W))  
  val bits    = RegInit(0.U(SIGNWIDTH.W))  
  val number  = RegInit(0.U(SIGNWIDTH.W))

  io.golomb.gcontrol.finish := state === finish
  io.control.flag   := flag    
  io.control.opcode := opcode  
  io.control.bit    := bit     
  io.control.bits   := bits    
  io.control.number := number  

  //val controlflag = RegInit(0.U.asTypeOf())  
  //val writing =   
  switch(state){
    is(idle){
      when(io.golomb.gcontrol.start) {
        state := compare
        value1 := io.golomb.value >> io.golomb.k
        value2 := io.golomb.lim-1.U-qbpp.U 
        value  := io.golomb.value 
        k      := io.golomb.k
      }    
    }
    is(compare){
      when(value1 < value2 ){
        state := write1 
        flag   := true.B
        opcode := 2.U
        //bit    :=
        bits   := 0.U
        number := value1
      }.otherwise{
        state := write4
        flag   := true.B
        opcode := 2.U
        //bit    :=
        bits   := 0.U
        number := value2
      }
    }
    is(write1){
      //when(!io.control.writing){
      //  flag := false.B
      //}
      when(io.control.writing ){
        state := write2
        opcode := 1.U 
        bit   := 1.U 
        flag := true.B
      }
    }
    is(write2){
      //when(!io.control.writing){
      //  flag := false.B
      //}
      when(io.control.writing ){
        state := write3
        opcode := 2.U 
        bits   := value
        number := k
        flag := true.B
      }
    }
    is(write3){
      //when(!io.control.writing){
      //  flag := false.B
      //}
      when(io.control.writing){
        state := finish
        flag := false.B  
      }
    }
    is(write4){
      //when(!io.control.writing){
      //  flag := false.B
      //}
      when(io.control.writing ){
        state := write5
        opcode := 1.U 
        bit   := 1.U 
        flag := true.B
      }
    }
    is(write5){
      //when(!io.control.writing){
      //  flag := false.B
      //}
      when(io.control.writing  ){
        state := write6
        opcode := 2.U 
        bits   := value-1.U
        number := qbpp.U
        flag := true.B
      }
    }
    is(write6){
      //when(!io.control.writing){
      //  flag := false.B
      //}
      when(io.control.writing ){
        state := finish
        flag := false.B
      }
    }
    is(finish){
      //io.finish := true.B
      state := idle 
    }
  }

}
class RAMVERILOG extends BlackBox with HasBlackBoxInline with COMMON{
    val io = IO(new Bundle {
        val clock	=	Input(Clock())	
        val rst   = Input(Bool())	      
        val awready = Output(Bool()) 
        val awvalid = Input(Bool())
        val awid    = Input(UInt(AXIIDWIDTH.W))
        val awaddr  = Input(UInt(AXIADDRWIDTH.W))
        val awlen   = Input(UInt(AXILENWIDTH.W))
        val awsize  = Input(UInt(AXISIZEWIDTH.W))
        val awburst = Input(UInt(AXIBURSTWIDTH.W))
        val wready  = Output(Bool())
        val wvalid  = Input(Bool())
        val wdata   = Input(UInt(AXIDATAWIDTH.W))
        val wstrb   = Input(UInt(AXISTRBWIDTH.W))
        val wlast   = Input(Bool())
        val bready  = Input(Bool())
        val bvalid  = Output(Bool())
        val bid     = Output(UInt(AXIIDWIDTH.W))
        val bresp   = Output(UInt(AXIRESPWIDTH.W))
        val arready = Output(Bool())
        val arvalid = Input(Bool())
        val arid    = Input(UInt(AXIIDWIDTH.W))
        val araddr  = Input(UInt(AXIADDRWIDTH.W))
        val arlen   = Input(UInt(AXILENWIDTH.W))
        val arsize  = Input(UInt(AXISIZEWIDTH.W))
        val arburst = Input(UInt(AXIBURSTWIDTH.W))
        val rready  = Input(Bool())
        val rvalid  = Output(Bool())
        val rid     = Output(UInt(AXIIDWIDTH.W))
        val rresp   = Output(UInt(AXIRESPWIDTH.W))
        val rdata   = Output(UInt(AXIDATAWIDTH.W))
        val rlast   = Output(Bool())  
    })
    setInline("/RAMVERILOG.v",
           """
           |module RAMVERILOG
           |    (  clock				      ,
           |       rst                ,
           |	     awready            ,
           |       awvalid            ,
           |       awid               ,
           |       awaddr             ,
           |       awlen              ,
           |       awsize             ,
           |       awburst            ,
           |       wready             ,
           |       wvalid             ,
           |       wdata              ,
           |       wstrb              ,
           |       wlast              ,
           |       bready             ,
           |       bvalid             ,
           |       bid                ,
           |       bresp              ,
           |       arready            ,
           |       arvalid            ,
           |       arid               ,
           |       araddr             ,
           |       arlen              ,
           |       arsize             ,
           |       arburst            ,
           |       rready             ,
           |       rvalid             ,
           |       rid                ,
           |       rresp              ,
           |       rdata              ,
           |       rlast              
           |    );
           |      input   clock				  ;
           |	    input   rst			      ;
           |      output  awready ;           
           |      input  awvalid ;           
           |      input [3:0] awid    ;           
           |      input [31:0] awaddr  ;           
           |      input [7:0] awlen   ;           
           |      input [2:0] awsize  ;           
           |      input [1:0] awburst ;           
           |      output  wready  ;           
           |      input  wvalid  ;           
           |      input  [7:0]wdata   ;           
           |      input  [7:0] wstrb   ;           
           |      input  wlast   ;           
           |      input  bready  ;           
           |      output  bvalid  ;           
           |      output [3:0] bid     ;           
           |      output [1:0] bresp   ;           
           |      output  arready ;           
           |      input  arvalid ;           
           |      input  [3:0] arid    ;           
           |      input  [31:0] araddr  ;           
           |      input  [7:0] arlen   ;           
           |      input  [2:0] arsize  ;           
           |      input  [1:0] arburst ;           
           |      input  rready  ;           
           |      output  rvalid  ;           
           |      output [3:0] rid     ;           
           |      output [1:0] rresp   ;           
           |      output [7:0] rdata   ;           
           |      output  rlast   ;           
           |    
           |SRAMSIM sramsim_1(
           |	.clock               (clock ),   
           |  .reset               (rst   ),   
           |  .io_Sram_ar_valid    (arvalid),   
           |  .io_Sram_ar_bits_addr(araddr),   
           |  .io_Sram_r_ready     (rready),   
           |  .io_ar_len           (arlen),   
           |  .io_ar_size          (arsize),   
           |  .io_ar_burst         (arburst),   
           |  .io_Sram_aw_valid    (awvalid),   
           |  .io_Sram_aw_bits_addr(awaddr),   
           |  .io_aw_len           (awlen),   
           |  .io_aw_size          (awsize),   
           |  .io_aw_burst         (awburst),   
           |  .io_Sram_w_valid     (wvalid),   
           |  .io_Sram_w_bits_data (wdata),   
           |  .io_Sram_w_bits_strb (wstrb),   
           |  .io_b_ready          (bready), 	
           |  .io_b_valid          (bvalid), 	
           |  .io_b_bresp          (bresp), 	 
           |  .io_Sram_ar_ready    (arready),   
           |  .io_Sram_r_valid     (rvalid),   
           |  .io_Sram_r_bits_data (rdata),   
           |  .io_Sram_r_rresp     (rresp), 	
           |  .io_Sram_r_bits_last (rlast),   
           |  .io_Sram_aw_ready    (awready),   
           |  .io_Sram_w_ready     (wready) 
           |);
           |endmodule
           """.stripMargin)
}


class ramtop extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val axi = Flipped(new AXIIO)
  })
  val ram = Module(new RAMVERILOG)
  ram.io.awready <> io.axi.awready  
  ram.io.awvalid <> io.axi.awvalid  
  ram.io.awid    <> io.axi.awid     
  ram.io.awaddr  <> io.axi.awaddr   
  ram.io.awlen   <> io.axi.awlen    
  ram.io.awsize  <> io.axi.awsize   
  ram.io.awburst <> io.axi.awburst  
  ram.io.wready  <> io.axi.wready   
  ram.io.wvalid  <> io.axi.wvalid   
  ram.io.wdata   <> io.axi.wdata    
  ram.io.wstrb   <> io.axi.wstrb    
  ram.io.wlast   <> io.axi.wlast    
  ram.io.bready  <> io.axi.bready   
  ram.io.bvalid  <> io.axi.bvalid   
  ram.io.bid     <> io.axi.bid      
  ram.io.bresp   <> io.axi.bresp    
  ram.io.arready <> io.axi.arready  
  ram.io.arvalid <> io.axi.arvalid  
  ram.io.arid    <> io.axi.arid     
  ram.io.araddr  <> io.axi.araddr   
  ram.io.arlen   <> io.axi.arlen    
  ram.io.arsize  <> io.axi.arsize   
  ram.io.arburst <> io.axi.arburst  
  ram.io.rready  <> io.axi.rready   
  ram.io.rvalid  <> io.axi.rvalid   
  ram.io.rid     <> io.axi.rid      
  ram.io.rresp   <> io.axi.rresp    
  ram.io.rdata   <> io.axi.rdata    
  ram.io.rlast   <> io.axi.rlast    
  ram.io.clock := clock 
  ram.io.rst   := reset.asBool

}