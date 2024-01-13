package decoder 
import chisel3._
import chisel3.util._
import scala.util.Random
class INFIO extends Bundle with COMMON{
  val A  = Output(SInt((TEXTWIDTH).W))
  val B  = Output(SInt((TEXTWIDTH).W))
  val C  = Output(SInt((TEXTWIDTH).W))
  val N  = Output(SInt((TEXTWIDTH).W))
  val Nn = Output(SInt((TEXTWIDTH).W))
  //val EOLine = Output(Bool())
  //val Ra = Output(SInt(SIGNWIDTH.W))
  //val Rb = Output(SInt(SIGNWIDTH.W))
  //val Rc = Output(SInt(SIGNWIDTH.W))
  //val Rd = Output(SInt(SIGNWIDTH.W))
}
class INFIOEN extends Bundle with COMMON{
  val Aupdate  = Output(Bool())
  val Bupdate  = Output(Bool())
  val Cupdate  = Output(Bool())
  val Nupdate  = Output(Bool())
  val Nnupdate = Output(Bool())
  //val EOLine = Output(Bool())
  //val Ra = Output(SInt(SIGNWIDTH.W))
  //val Rb = Output(SInt(SIGNWIDTH.W))
  //val Rc = Output(SInt(SIGNWIDTH.W))
  //val Rd = Output(SInt(SIGNWIDTH.W))
}
/****
RegularMode Run
 

Sequantial logic 
****/

class RegularMode extends Module with COMMON {
  val io = IO(new Bundle {
    //input 
    val infoin  = Flipped(new INFIO)
    val Q       = Input(SInt((SIGNWIDTH+1).W))
    val sign    = Input(UInt(1.W))
    //from predict 
    val Px      = Input(SInt(SIGNWIDTH.W))
    //Ix 
    val inpix   = Input(SInt(SIGNWIDTH.W))

    val outQ    = Output(SInt((SIGNWIDTH+1).W))
    val infoout = new INFIO
    val infen   = new INFIOEN

    //TO quantize 
    //ERRVAL 
    val quanti = Flipped(new logicIO)

    //TO computeRx
    // Px : predict out 
    // errval : CQ 
    // sign : sign
    // Rx : Px 
    val predict = Flipped(new predictIO) 

    //ModRange 
    // 
    val modrange = Flipped(new logicIO)
    //LG 
    val lg = Flipped(new lgIO)
    //Golombcoding
    val golomb = Flipped(new golombIO)

    //control io 
    val rcontrol = new controlIO
  })
  val AQ = RegInit(0.S(TEXTWIDTH.W))
  val BQ = RegInit(0.S(TEXTWIDTH.W))
  val BQ2= RegInit(0.S(TEXTWIDTH.W))
  val CQ = RegInit(0.S(TEXTWIDTH.W))
  val NQ = RegInit(0.S(TEXTWIDTH.W))
  val Qin= RegInit(0.S(TEXTWIDTH.W)) 
  val Nn = RegInit(0.S(TEXTWIDTH.W))

  val Prex = RegInit(0.S(SIGNWIDTH.W)) 
  val Px = RegInit(0.S(SIGNWIDTH.W)) 
  val Ix = RegInit(0.S(SIGNWIDTH.W))
  val Rx = RegInit(0.S(SIGNWIDTH.W))
  val sign = RegInit(0.U(SIGNWIDTH.W))

  val Errval = RegInit(0.S((SIGNWIDTH+1).W))

  val k = RegInit(0.S(SIGNWIDTH.W))
  val MErrval = RegInit(0.S((SIGNWIDTH.W)))

  val idle :: getPx :: getErrval :: quanti :: mod :: lg :: getMErrval :: golomb :: update1 :: update2 :: update3 :: writeback :: Nil = Enum(12) 
 //   0    :: 1     :: 2         :: 3      :: 4   :: 5  :: 6          :: 7      :: 8       :: 9       :: 10      :: 11        :: 12
  val state = RegInit(idle)


  io.infoout.A      := AQ
  io.infoout.B      := BQ
  io.infoout.C      := CQ
  io.infoout.N      := NQ
  io.infoout.Nn     := 0.S 
  io.infen.Aupdate  := false.B 
  io.infen.Bupdate  := false.B 
  io.infen.Cupdate  := false.B 
  io.infen.Nupdate  := false.B 
  io.infen.Nnupdate := false.B 
  io.outQ           := Qin

  io.predict.Px     := Prex
  io.predict.errval := CQ
  io.predict.sign   := sign 

  io.quanti.in := Errval

  io.modrange.in := Errval

  io.lg.AQ := AQ 
  io.lg.NQ := NQ 
  //io.lg.k  := k 
  io.lg.lcontrol.start := false.B 

  io.golomb.lim := limit.U
  io.golomb.value := MErrval.asUInt 
  io.golomb.k := k.asUInt
  io.golomb.gcontrol.start := false.B 

  io.rcontrol.finish := false.B 

  switch(state){
    is(idle){
      when(io.rcontrol.start){
        state := getPx 
        Prex  := io.Px
        AQ := io.infoin.A 
        BQ := io.infoin.B 
        CQ := io.infoin.C 
        NQ := io.infoin.N 
        Ix := io.inpix
        sign := io.sign
        Qin := io.Q
      }
    }
    is(getPx){
      state := getErrval 
      Px    := io.predict.Rx 
      Rx    := Px 
      BQ2   := BQ +BQ
      //state := getErrval
    }
    is(getErrval){
      state := quanti 
      Errval := Mux(sign === 0.U,Ix - Px,Px - Ix) 
    }
    is(quanti){
      state := mod 
      Errval := io.quanti.out
    }
    is(mod){
      state := lg 
      Errval := io.modrange.out
      io.lg.lcontrol.start := true.B 
    }
    is(lg){
      io.lg.lcontrol.start := true.B 
      when(io.lg.lcontrol.finish){
        io.lg.lcontrol.start := false.B
        k := io.lg.k 
        state := getMErrval  
      }
    }
    is(getMErrval){
      when((NEAR.U === 0.U) && (k === 0.S) && (BQ2 <= -NQ)){
        when(Errval >= 0.S){
          MErrval := (Errval << 1.U).asSInt  + 1.S
        }.otherwise{
          MErrval := -((Errval+1.S) << 1.U)
        }
      }.otherwise{
        when(Errval >= 0.S){
          MErrval := Errval << 1.U  
        }.otherwise{
          MErrval := -((Errval) << 1.U) - 1.S 
        }
      }
      state := golomb
    }
    is(golomb){
      io.golomb.gcontrol.start := true.B 
      when(io.golomb.gcontrol.finish){
        state := update1
      }
    }
    is(update1){
      BQ := BQ + Errval*(2.S*NEAR.S+1.S)
      AQ := AQ + ABSH(Errval)
      state := update2
    }
    is(update2){
      when(NQ === RESET.S ){
        AQ := AQ >> 1.U
        when(BQ >= 0.S){
          BQ := BQ >>1.U
        }.otherwise{
          BQ := -((1.S-BQ)>>1.U)
        }
        NQ := (NQ >> 1.U) + 1.S  
      }.otherwise{
        NQ := NQ + 1.S 
      }
      state := update3
    }
    is(update3){
      when(BQ <= (-NQ)){
        //BQ := BQ + NQ 
        when(CQ > MIN_C.S ){
          CQ := CQ - 1.S 
        }
        when((BQ+NQ) <= -NQ) {
          BQ := -NQ + 1.S 
        }.otherwise{
          BQ := BQ + NQ
        }
      }.elsewhen(BQ > 0.S){
        when(CQ < MAX_C.S){
          CQ := CQ + 1.S  
        }
        when((BQ-NQ)>0.S){
          BQ := 0.S 
        }.otherwise{
          BQ := BQ -NQ 
        }
      }
      state := writeback
    }
    is(writeback){
      state := idle 
      io.rcontrol.finish := true.B 
      io.infen.Aupdate  := true.B 
      io.infen.Bupdate  := true.B 
      io.infen.Cupdate  := true.B 
      io.infen.Nupdate  := true.B 
      io.infen.Nnupdate := false.B 


    }
  }
  

}

/****
RunMode 

Sequantial logic 
****/

class RunMode extends Module with COMMON {
  val io = IO(new Bundle {
    //input 
    val infoin  = Flipped(new INFIO)
    val Q       = Input(SInt((SIGNWIDTH+1).W))
    //val sign    = Input(UInt(1.W))
    //from predict 
    //val Px      = Input(SInt(SIGNWIDTH.W))
    //Ix 
    //val inpix   = Input(SInt(SIGNWIDTH.W))
    val inaddr  = Flipped(new addrIO )
    val outaddr = (new addrIO)
    val updateen = Output(Bool())
    val runwrite = Output(Bool())
    // addr 
    // for getnext sample 
    val sample  = Flipped(new SampleIO)
    // for get pix 
    val getpix  = new GetpixIO

    // for bit write 

    val bitwrite = Flipped(new BitWriterIO)
    //val inaddr  = Flipped(new addrIO)
    //val EOLine = Output(Bool())
    //val addrvalid = Output(Bool())
    //val getvalid = Output(Bool())
    //val outaddr = (new addrIO)

    val outQ    = Output(SInt((SIGNWIDTH+1).W))
    val infoout = new INFIO
    val infen   = new INFIOEN

    //TO quantize 
    //ERRVAL 
    val quanti = Flipped(new logicIO)

    //TO computeRx
    // Px   : predict out 
    // errval : CQ 
    // sign : sign
    // Rx : Rx 
    val predict = Flipped(new predictIO) 

    //ModRange 
    // 
    val modrange = Flipped(new logicIO)
    //LG 
    val lg = Flipped(new lgIO)
    //Golombcoding
    val golomb = Flipped(new golombIO)

    //control io 
    val rcontrol = new controlIO
    val runreset = Input(Bool())
  })
  val AQ = RegInit(0.S(TEXTWIDTH.W))
  val BQ = RegInit(0.S(TEXTWIDTH.W))
  val BQ2= RegInit(0.S(TEXTWIDTH.W))
  val CQ = RegInit(0.S(TEXTWIDTH.W))
  val NQ = RegInit(0.S(TEXTWIDTH.W))
  val Qin= RegInit(0.S((SIGNWIDTH+1).W)) 
  val Nn = RegInit(0.S(TEXTWIDTH.W))
  val Nn2= RegInit(0.S(TEXTWIDTH.W))

  val RUNindex = RegInit(0.U(COLORWIDTH.W))
  val RUNval   = RegInit(0.S(SIGNWIDTH.W))
  val RUNcnt   = RegInit(0.U(COLORWIDTH.W))

  val JOUT = RegInit(0.U(COLORWIDTH.W))
  for(i <- 0 until 32){
    when(RUNindex === i.U){
      JOUT :=  J(i).U 
    }
  }
  
  val Jshift  =  1.U << JOUT


  val writerunflag = RegInit(0.U(1.W))


  val x = RegInit(0.U(XADDRWIDTH.W))
  val y = RegInit(0.U(YADDRWIDTH.W))


  //val Prex = RegInit(0.S(SIGNWIDTH.W)) 
  val Px = RegInit(0.S(SIGNWIDTH.W)) 
  val Ix = RegInit(0.S(SIGNWIDTH.W))
  val Rx = RegInit(0.S(SIGNWIDTH.W))
  val sign = RegInit(0.U(SIGNWIDTH.W))
  
  val Ritype = RegInit(0.U(1.W))


  val Errval = RegInit(0.S((SIGNWIDTH+1).W))
  val ErrvalABS =  ABSH(Errval)

  val Merrval1 = (2.S)*ErrvalABS 


  val k = RegInit(0.S(SIGNWIDTH.W))
  val MErrval = RegInit(0.S((SIGNWIDTH.W)))
  val MErrvalext = Wire(SInt(TEXTWIDTH.W))
  MErrvalext := MErrval
  val shift1 = (MErrvalext + 2.S ) >> 1.U 
  val shift2 = (MErrvalext + 1.S ) >> 1.U 
  val glimit = RegInit(0.S((SIGNWIDTH.W)))
  val map = Wire(UInt(1.W))
  map := false.B 

  val nearlocal = ABSH(Ix -(RUNval))
  val AMINSB    = ABSH((io.sample.pix.Ra).asSInt-(io.sample.pix.Rb).asSInt)



  val idle :: initial :: determination :: updateix :: getnext :: writerun :: writebit1 :: writebit0 :: writebits :: interrupt :: getErrval :: quanti :: mod :: lg :: getMErrval :: golomb :: update1 :: update2 :: writeback :: Nil = Enum(19) 
  //  0    :: 1       :: 2             :: 3        :: 4       :: 5        :: 6         :: 7         :: 8         :: 9         :: 10        :: 11     :: 12 :: 13  :: 14         :: 15     :: 16      :: 17      :: 18        :: 19        ::      
  val state = RegInit(idle)


  io.infoout.A      := AQ
  io.infoout.B      := BQ
  io.infoout.C      := CQ
  io.infoout.N      := NQ
  io.infoout.Nn     := Nn 
  io.infen.Aupdate  := false.B 
  io.infen.Bupdate  := false.B 
  io.infen.Cupdate  := false.B 
  io.infen.Nupdate  := false.B 
  io.infen.Nnupdate := false.B 
  io.outQ           := Qin

  io.predict.Px     := Px
  io.predict.errval := Errval
  io.predict.sign   := sign 

  io.quanti.in := Errval

  io.modrange.in := Errval

  io.lg.AQ := AQ 
  io.lg.NQ := NQ 
  //io.lg.k  := k 
  io.lg.lcontrol.start := false.B 

  io.golomb.lim := glimit.asUInt
  io.golomb.value := MErrval.asUInt 
  io.golomb.k := k.asUInt
  io.golomb.gcontrol.start := false.B 

  io.rcontrol.finish := false.B 

  io.bitwrite.flag := false.B 
  io.bitwrite.opcode := 4.U 
  io.bitwrite.bit  := 0.U 
  io.bitwrite.bits := 0.U 
  io.bitwrite.number  := 0.U 
  io.runwrite := false.B 
  io.outaddr.x := x 
  io.outaddr.y := y 
  io.updateen := false.B 

  io.sample.update := false.B 
  io.getpix.addrvalid := false.B 
  
  switch(state){
    is(idle){
      RUNindex := 0.U 
      when(io.rcontrol.start){
        state := initial 
        AQ  := 0.S
        BQ  := 0.S
        BQ2 := 0.S
        CQ  := 0.S
        NQ  := 0.S
        Qin := 0.S
        Nn  := 0.S
        Nn2 := 0.S
        writerunflag := 0.U 
        Px   := 0.S 
        Ix   := 0.S 
        Rx   := 0.S 
        Errval := 0.S 
        k := 0.S 
        MErrval := 0.S 
        glimit := 0.S
      }
    }
    is(initial){
      when(io.runreset){
        state := idle 
      }.elsewhen(io.rcontrol.start){
        RUNcnt := 0.U
        state := determination
        x  := io.inaddr.x 
        y  := io.inaddr.y
        Ix := (0.U(1.W)##io.getpix.inpix).asSInt 
        RUNval := io.sample.pix.Ra
      }
    }
    is(determination){
      when(nearlocal <= NEAR.S){
        RUNcnt := RUNcnt + 1.U 
        Rx := RUNval 
        when(io.sample.EOLine){
          state := writerun 
          
        }.otherwise{
          x := x + 1.U 
          state := updateix
        }
      }.otherwise{
        state := writerun
      }
    }
    is(updateix){
      io.getpix.addrvalid := true.B 
      when(io.getpix.pixvalid){
        io.getpix.addrvalid := false.B
        Ix := (0.U(1.W)##io.getpix.inpix).asSInt
        state := getnext 
      }
    }
    is(getnext){
      io.sample.update := true.B 
      when(io.sample.getvalid){
        io.sample.update := false.B 
        state := determination
      }
    }
    is(writerun){
      io.runwrite := true.B 
      when(RUNcnt >= Jshift ){
        state := writebit1 
        writerunflag := 1.U 
        io.bitwrite.flag := true.B 
        io.bitwrite.opcode := 1.U 
        io.bitwrite.bit  := 1.U 
        RUNcnt := RUNcnt -Jshift 
        when(RUNindex < 31.U){
          RUNindex := RUNindex + 1.U 
        }
      }.otherwise{
        state := interrupt
      }
    }
    is(writebit1){
      io.runwrite := true.B 
      //writerunflag := 1.U 
      io.bitwrite.flag := true.B 
      io.bitwrite.opcode := 1.U 
      io.bitwrite.bit  := 1.U 
      when(io.bitwrite.writing){
        //writerunflag := 1.U 
        io.bitwrite.flag := false.B 
        when(writerunflag === 1.U){
          state := writerun
          writerunflag := 0.U  
        }.otherwise{
          state := writeback
        }
      }
    }
    is(writebit0){
      io.runwrite := true.B 
      io.bitwrite.flag := true.B 
      io.bitwrite.opcode := 1.U 
      io.bitwrite.bit  := 0.U 
      when(io.bitwrite.writing){
        //writerunflag := 1.U 
        //io.bitwrite.flag := false.B 
        io.bitwrite.opcode := 2.U 
        io.bitwrite.bits   := RUNcnt 
        io.bitwrite.number := JOUT
        state := writebits
      }
    }
    is(writebits){
      io.runwrite := true.B 
      io.bitwrite.flag := true.B 
      io.bitwrite.opcode := 2.U 
      io.bitwrite.bits   := RUNcnt 
      io.bitwrite.number := JOUT
      when(io.bitwrite.writing){
        //writerunflag := 1.U 
        io.bitwrite.flag := false.B 
        state := getErrval
        when(RUNindex > 0.U ){
           RUNindex := RUNindex -1.U
        }
        //RUNindex := RUNindex -1.U
      }
    }
    is(interrupt){
      when(nearlocal > NEAR.S){
        state := writebit0
      }.elsewhen(RUNcnt > 0.U){
        state := writebit1
      }.otherwise{
        state := writeback
      }      
    }
    is(getErrval){
      when(AMINSB <= NEAR.S){
        Ritype := 1.U 
        Qin := 366.S 
        Px := io.sample.pix.Ra 
      }.otherwise{
        Qin := 365.S 
        Ritype := 0.U 
        Px := io.sample.pix.Rb 
      }
      when(AMINSB > NEAR.S &&(io.sample.pix.Ra > io.sample.pix.Rb)){
        Errval := (io.sample.pix.Rb).asSInt - Ix 
        sign := 1.U 
      }.otherwise{
        when(AMINSB > NEAR.S){
          Errval := Ix - (io.sample.pix.Rb).asSInt   
        }.otherwise{
          Errval := Ix - (io.sample.pix.Ra).asSInt 
        }
        sign := 0.U 
      }
      state := quanti
    }
    is(quanti){
      when(Ritype === 0.U ){
        AQ := io.infoin.A
      }.otherwise{
        AQ := io.infoin.A + (io.infoin.N >> 1.U)
      }
      BQ := io.infoin.B
      CQ := io.infoin.C
      NQ := io.infoin.N
      Nn := io.infoin.Nn
      state := mod
      when(NEAR.S > 0.S){
        Errval := io.quanti.out
      } 
    }
    is(mod){
      Nn2 := 2.S * Nn
      when(NEAR.S > 0.S){
        Rx := io.predict.Rx 
      }.otherwise{
        Rx := Ix 
      }
      state := lg 
      Errval := io.modrange.out
      io.lg.lcontrol.start := true.B 
    }
    is(lg){
      io.lg.lcontrol.start := true.B 
      when(io.lg.lcontrol.finish){
        io.lg.lcontrol.start := false.B
        k := io.lg.k 
        state := getMErrval  
      }
    }
    is(getMErrval){
      when((k === 0.S) && (Errval > 0.S) &&(Nn2 < (NQ))){
        //map := 1.U 
        when(Ritype === 1.U){
          MErrval := Merrval1 - 1.S - 1.S
        }.otherwise{
          MErrval := Merrval1 - 1.S
        }
        
      }.elsewhen((Errval < 0.S) && (Nn2 >= NQ)){
        //map := 1.U
        when(Ritype === 1.U){
          MErrval := Merrval1 - 1.S - 1.S
        }.otherwise{
          MErrval := Merrval1 - 1.S
        }  
      }.elsewhen((Errval < 0.S) && (k =/= 0.S)){
        //map := 1.U
        when(Ritype === 1.U){
          MErrval := Merrval1 - 1.S - 1.S
        }.otherwise{
          MErrval := Merrval1 - 1.S
        } 
      }.otherwise{
        when(Ritype === 1.U){
          MErrval := Merrval1 - 1.S
        }.otherwise{
          MErrval := Merrval1 
        } 
        //map := 0.U 
      }
      //MErrval := 2.S*ABSH(Errval) - Ritype.asSInt - map.asSInt 
      glimit := limit.S - JOUT.asSInt - 1.S 
      state := golomb
    }
    is(golomb){
      io.golomb.gcontrol.start := true.B 
      when(io.golomb.gcontrol.finish){
        state := update1
      }
    }
    is(update1){
      when(Errval < 0.S){
        Nn := Nn + 1.S 
      }
      when(Ritype === 1.U){
        AQ := AQ + shift1 
      }.otherwise{
        AQ := AQ + shift2
      }
     // AQ := AQ + ((MErrval+1.S + Ritype.asSInt) >> 1.U)
      state := update2
    }
    is(update2){
      when(NQ === RESET.S ){
        AQ := AQ >> 1.U
        NQ := (NQ >> 1.U) 
        Nn := Nn >> 1.U   
      }
      state := writeback
    }
    is(writeback){
      state := initial 
      io.rcontrol.finish := true.B 
      io.infen.Aupdate  := true.B 
      //io.infen.Bupdate  := true.B 
      //io.infen.Cupdate  := true.B 
      io.infen.Nupdate  := true.B 
      io.infen.Nnupdate := false.B 
      io.updateen := true.B 

    }
  }
  

}
