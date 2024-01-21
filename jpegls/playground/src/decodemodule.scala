package decoder 
import chisel3._
import chisel3.util._
import scala.util.Random

/****
RegularMode Run
 

Sequantial logic 
****/

class RegularModeDecoding extends Module with COMMON {
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
    //val quanti = Flipped(new logicIO)

    //TO computeRx
    // Px : predict out 
    // errval : CQ 
    // sign : sign
    // Rx : Px 
    val predict = Flipped(new predictIO) 

    //ModRange 
    // 
    //val modrange = Flipped(new logicIO)
    //LG 
    val lg = Flipped(new lgIO)
    //Golombcoding
    val golomb = Flipped(new golombdeIO)
    //updatesample 
    val ucontrol = Flipped(new controlIO)
    val outpix  = Output(UInt(COLORWIDTH.W))
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
  val MErrval = RegInit(0.U((SIGNWIDTH.W)))

  val idle :: getPx :: lg :: golomb ::getErrval:: update1 :: update2 :: update3 :: getRx ::updatesample:: writeback :: Nil = Enum(11) 
 //   0    :: 1     :: 2  :: 3      :: 4       :: 5       :: 6       :: 7       :: 8     :: 9          :: 10                       
  val regularstate = RegInit(idle)


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

  //io.quanti.in := Errval

  //io.modrange.in := Errval

  io.lg.AQ := AQ 
  io.lg.NQ := NQ 
  //io.lg.k  := k 
  io.lg.lcontrol.start := false.B 

  io.golomb.lim := limit.U
  //io.golomb.value := MErrval
  io.golomb.k := k.asUInt
  io.golomb.gcontrol.start := false.B 

  io.ucontrol.start := false.B 
  io.outpix := Rx(COLORWIDTH-1,0) 

  io.rcontrol.finish := false.B 

  switch(regularstate){
    is(idle){
      when(io.rcontrol.start){
        regularstate := getPx 
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
      regularstate := lg 
      Px    := io.predict.Rx 

      BQ2   := BQ +BQ
      //state := getErrval
    }
    is(lg){
      io.lg.lcontrol.start := true.B 
      when(io.lg.lcontrol.finish){
        io.lg.lcontrol.start := false.B
        k := io.lg.k 
        regularstate := golomb  
      }
    }
    is(golomb){
      io.golomb.gcontrol.start := true.B 
      when(io.golomb.gcontrol.finish){
        regularstate := getErrval 
        MErrval :=  io.golomb.value 
      }
    }
    is(getErrval){
      when((NEAR.U === 0.U) && (k === 0.S) && (BQ2 <= -NQ)){
        when(MErrval(0) === 1.U){
          Errval := ((MErrval-1.U) >> 1.U).asSInt 
        }.otherwise{
          Errval := -((MErrval >> 1.U).asSInt)-1.S
        }
      }.otherwise{
        when(MErrval(0) === 0.U){
          Errval := (MErrval >> 1.U).asSInt 
        }.otherwise{
          Errval := -(((MErrval+1.U) >> 1.U).asSInt)
        }
      }
      regularstate := update1
    }
    is(update1){
      BQ := BQ + Errval*(2.S*NEAR.S+1.S)
      AQ := AQ + ABSH(Errval)
      //updateErrval 
      when(sign === 1.U){
        Errval := -Errval *((2*NEAR).S+1.S)
      }.otherwise{
        Errval := Errval *((2*NEAR).S+1.S)
      }
      
      regularstate := update2
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

      //update Rx 
      Rx := (Errval+Px)%(RANGE*(2*NEAR+1)).S


      regularstate := update3
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
      //update Rx 
      when(Rx < (-NEAR).S){
        Rx := Rx + (RANGE*(2*NEAR+1)).S 
      }.elsewhen(Rx > (MAXVAL+NEAR).S){
        Rx := Rx - (RANGE*(2*NEAR+1)).S 
      }

      regularstate := getRx 
    }
    is(getRx){
      when(Rx < (0).S){
        Rx := 0.S
      }.elsewhen(Rx > (MAXVAL).S){
        Rx := MAXVAL.S
      }
      regularstate := updatesample
    }
    is(updatesample){
      io.ucontrol.start:=true.B 
      when(io.ucontrol.finish){
        regularstate := writeback
        io.ucontrol.start:=false.B 
      }
    }
    is(writeback){
      regularstate := idle 
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

class RunModedecoding extends Module with COMMON {
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
    val runread = Output(Bool())
    // addr 
    // for getnext sample 
    val sample  = Flipped(new SampleIO)
    // for get pix 
    //val getpix  = new GetpixIO

    // for bit read 

    val reader = Flipped(new BitReaderIO)
    //val bcontrol   = Flipped (new controlIO)
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
    //val quanti = Flipped(new logicIO)

    //TO computeRx
    // Px   : predict out 
    // errval : CQ 
    // sign : sign
    // Rx : Rx 
    //val predict = Flipped(new predictIO) 

    //ModRange 
    // 
    //val modrange = Flipped(new logicIO)
    //LG 
    val lg = Flipped(new lgIO)
    //Golombcoding
    val golomb = Flipped(new golombdeIO)

    //sampleupdate
    val ucontrol = Flipped(new controlIO)
    val outpix  = Output(UInt(COLORWIDTH.W))
    //control io 
    val rcontrol = new controlIO
    val runreset = Input(Bool())
  })
  val AQ = RegInit(0.S(TEXTWIDTH.W))
  val BQ = RegInit(0.S(TEXTWIDTH.W))
  val BQ2= RegInit(0.S(TEXTWIDTH.W))
  val CQ = RegInit(0.S(TEXTWIDTH.W))
  val NQ = RegInit(0.S(TEXTWIDTH.W))
  val temp = RegInit(0.S(TEXTWIDTH.W))
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


  val readrunflag = RegInit(0.U(1.W))

  val localbit = RegInit(0.U(1.W))
  val count    = RegInit(0.U(16.W))
  val number   = RegInit(0.U(16.W))
  //这个用来指示updatesample后应该跳回的状态，其他同理
  val updatesampleflag = RegInit(0.U(2.W))
  //
  val getnextsampleflag = RegInit(0.U(2.W))

  val readbitflag     = RegInit(0.U(2.W))


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

  val tempErr = RegInit(0.S((SIGNWIDTH+1).W))

  val Merrval1 = (2.S)*ErrvalABS 


  val k = RegInit(0.S(SIGNWIDTH.W))
  val MErrval = RegInit(0.S((SIGNWIDTH.W)))
  val EMErrval = RegInit(0.U((SIGNWIDTH.W)))

  val MErrvalext = Wire(SInt(TEXTWIDTH.W))
  MErrvalext := MErrval
  val shift1 = (MErrvalext + 2.S ) >> 1.U 
  val shift2 = (MErrvalext + 1.S ) >> 1.U 
  val glimit = RegInit(0.S((SIGNWIDTH.W)))
  val map = Wire(UInt(1.W))
  map := 0.U 

  val computErr =(( EMErrval + Ritype.asUInt() + map.asUInt())>>1.U).asSInt


  val nearlocal = ABSH(Ix -(RUNval))
  val AMINSB    = ABSH((io.sample.pix.Ra).asSInt-(io.sample.pix.Rb).asSInt)


  val EOLine = io.sample.EOLine


  val idle :: initial :: readbit :: compare :: rundecoding :: updatesample :: getnextsample :: indexadd :: readbits :: compare2 :: getEMErrval :: quanti :: lg ::golomb:: getErrval ::getRx :: update2 :: update3 :: writeback :: finish    :: Nil = Enum(20) 
  //  0    :: 1       :: 2       :: 3       :: 4           :: 5            :: 6             :: 7        :: 8        :: 9        :: 10          :: 11     :: 12 :: 13   :: 14        :: 15   :: 16      :: 17      :: 18        :: 19        ::      
  val rundecodestate = RegInit(idle)


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

  //io.predict.Px     := Px
  //io.predict.errval := Errval
  //io.predict.sign   := sign 

  //io.quanti.in := Errval

  //io.modrange.in := Errval

  io.lg.AQ := AQ 
  io.lg.NQ := NQ 
  //io.lg.k  := k 
  io.lg.lcontrol.start := false.B 

  io.golomb.lim := glimit.asUInt
  //io.golomb.value := MErrval.asUInt 
  io.golomb.k := k.asUInt
  io.golomb.gcontrol.start := false.B 

  io.rcontrol.finish := false.B 

  io.reader.flag := false.B 
  io.reader.opcode := 4.U 

  io.reader.number  := JOUT
  io.runread := readrunflag
  io.outaddr.x := x 
  io.outaddr.y := y 
  io.updateen := false.B 

  io.ucontrol.start := false.B 
  val outpix = RegInit(0.U(COLORWIDTH.W))
  io.outpix := outpix
  //RUNval(COLORWIDTH-1,0)

  io.sample.update := false.B 
  //io.getpix.addrvalid := false.B 
  
  switch(rundecodestate){
    is(idle){
      RUNindex := 0.U 
      when(io.rcontrol.start){
        rundecodestate := initial 
        AQ  := 0.S
        BQ  := 0.S
        BQ2 := 0.S
        CQ  := 0.S
        NQ  := 0.S
        Qin := 0.S
        Nn  := 0.S
        Nn2 := 0.S
        Px   := 0.S 
        Ix   := 0.S 
        Rx   := 0.S 
        Errval := 0.S 
        temp :=0.S
        k := 0.S 
        MErrval := 0.S  
        EMErrval := 0.U
        glimit := 0.S
        //Prex  := io.Px
        //AQ := io.infoin.A 
        //BQ := io.infoin.B 
        //CQ := io.infoin.C 
        //NQ := io.infoin.N 
        //Ix := io.inpix
        //RUNval := io.sample.pix.Ra 
        //sign := io.sign
        //Qin := io.Q
      }
    }
    is(initial){
      when(io.runreset){
        rundecodestate := idle 
      }.elsewhen(io.rcontrol.start){
        RUNcnt := 0.U
        rundecodestate := readbit
        readbitflag := 0.U 
        x  := io.inaddr.x 
        y  := io.inaddr.y
        //Ix := (0.U(1.W)##io.getpix.inpix).asSInt 
        RUNval := io.sample.pix.Ra
        readrunflag := true.B
      }
    }
    is(readbit){
      io.reader.flag := true.B 
      io.reader.opcode := 1.U 
      when(io.reader.finish){
        localbit := io.reader.bit
        when(readbitflag === 0.U){
          rundecodestate := compare
        }.elsewhen(readbitflag === 1.U){
          rundecodestate := getnextsample
          getnextsampleflag := 1.U 
        }
        
      }
    }
    is(compare){
      when(localbit === 1.U){
        rundecodestate := rundecoding
      }.otherwise{
        rundecodestate := readbits
      }
      count := 0.U 
    }
    is(rundecoding){
      when(count < Jshift){
        count := count+1.U 
        //io.ucontrol.start := true.B 
        rundecodestate := updatesample 
        outpix := RUNval(COLORWIDTH-1,0)
        updatesampleflag := 0.U  
      }.otherwise{
        rundecodestate := indexadd
      }
    }
    is(updatesample){
      io.ucontrol.start := true.B
      when(io.ucontrol.finish){
        when(updatesampleflag === 0.U){
          when(EOLine){
            rundecodestate := indexadd
          }.otherwise{
            rundecodestate := getnextsample 
            getnextsampleflag := 0.U 
            when(count < Jshift){
              x := x + 1.U 
            }
          }
        }.elsewhen(updatesampleflag === 1.U){
          when(x === (X_SIZE-1).U){
            x := 0.U 
            y := y + 1.U 
          }.otherwise{
            x := x + 1.U 
          }
          rundecodestate := getnextsample
          getnextsampleflag := 2.U
        }.elsewhen(updatesampleflag === 2.U){
          rundecodestate := writeback
        
        }
      }
    }
    is(getnextsample){
      io.sample.update := true.B 
      when(io.sample.getvalid){
        when(getnextsampleflag===0.U){
          rundecodestate := rundecoding
        }.elsewhen(getnextsampleflag === 1.U){
          rundecodestate := compare
        }.elsewhen(getnextsampleflag === 2.U){
          rundecodestate := compare2
        }
      }
    }
    is(indexadd){
      when((count === (Jshift))&&(RUNindex <31.U)){
        RUNindex := RUNindex + 1.U
      }
      when(EOLine === 0.U){
        rundecodestate := readbit
        readbitflag := 1.U 
        x := x + 1.U
      }.otherwise{
        rundecodestate := finish
      }
    }
    is(readbits){
      io.reader.flag := true.B 
      io.reader.opcode := 2.U 
      when(io.reader.finish){
        number := io.reader.bits
        rundecodestate := compare2
        count:=0.U        
      }
    }
    is(compare2){
      when(count < number){
        count := count + 1.U 
        rundecodestate := updatesample
        outpix := RUNval(COLORWIDTH-1,0)
        updatesampleflag := 1.U
      }.otherwise{
        when(RUNindex > 0.U){
          RUNindex := RUNindex - 1.U 
         
        } 
        rundecodestate := getEMErrval
      }
    }
   
    is(getEMErrval){
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
        //Errval := (io.sample.pix.Rb).asSInt - Ix 
        sign := 1.U 
      }.otherwise{
        //when(AMINSB > NEAR.S){
        //  Errval := Ix - (io.sample.pix.Rb).asSInt   
        //}.otherwise{
        //  Errval := Ix - (io.sample.pix.Ra).asSInt 
        //}
        sign := 0.U 
      }
      rundecodestate := quanti
    }
    is(quanti){
      when(Ritype === 0.U ){
        AQ := io.infoin.A
      }.otherwise{
        AQ := io.infoin.A + (io.infoin.N >> 1.U)
      }
      temp := io.infoin.A
      BQ := io.infoin.B
      CQ := io.infoin.C
      NQ := io.infoin.N
      Nn := io.infoin.Nn
      rundecodestate := lg
    }
    is(lg){
      io.lg.lcontrol.start := true.B 
      Nn2 := 2.S * Nn
      glimit := limit.S - JOUT.asSInt - 1.S 
      when(io.lg.lcontrol.finish){
        io.lg.lcontrol.start := false.B
        k := io.lg.k 
        rundecodestate := golomb
        readrunflag := false.B 
      }
    }
    is(golomb){
      io.golomb.gcontrol.start := true.B 
      when(io.golomb.gcontrol.finish){
        rundecodestate := getErrval
        EMErrval := io.golomb.value
        readrunflag:= true.B 
      }
    }
    is(getErrval){
      //get map 
      when((((EMErrval(0)))===0.U)&&(Ritype===1.U)){
		    map:=1.U
      }.elsewhen(((EMErrval(0))===1.U)&&(Ritype===1.U)){
		    map:=0.U
      }.elsewhen(((EMErrval(0))===1.U)&&(Ritype===0.U)){
		    map:=1.U
      }.elsewhen(((EMErrval(0))===0.U)&&(Ritype===0.U)){
		    map:=0.U
      }
      when((map===0.U)&&(!((k.asUInt===0.U)&&Nn2<NQ))||((map===1.U)&&(k.asUInt===0.U)&&(Nn2<NQ))){
        when(sign === 1.U){
          Errval := -computErr*(2*NEAR+1).S
        }.otherwise{
          Errval := computErr*(2*NEAR+1).S
        }
        tempErr := computErr
      }.otherwise{
         when(sign === 1.U){
          Errval := computErr*(2*NEAR+1).S
        }.otherwise{
          Errval := -computErr*(2*NEAR+1).S
        }
        tempErr := -computErr
      }
      AQ := temp
      rundecodestate := getRx
    }
    is(getRx){
      Rx := (Errval+Px)%(RANGE*(2*NEAR+1)).S
      when(tempErr < 0.S){
        Nn := Nn + 1.S 
      }
      AQ := AQ + ((EMErrval+1.U+Ritype.asUInt())>>1.U).asSInt
      
     // AQ := AQ + ((MErrval+1.S + Ritype.asSInt) >> 1.U)
      rundecodestate := update2
    }
    is(update2){
      when(Rx < (-NEAR).S){
        Rx := Rx + (RANGE*(2*NEAR+1)).S 
      }.elsewhen(Rx > (MAXVAL+NEAR).S){
        Rx := Rx - (RANGE*(2*NEAR+1)).S 
      }
      when(NQ === RESET.S ){
        AQ := AQ >> 1.U
        NQ := (NQ >> 1.U) 
        Nn := Nn >> 1.U   
      }
      rundecodestate := update3
    }
    is(update3){
      when(Rx < (0).S){
        Rx := 0.S
      }.elsewhen(Rx > (MAXVAL).S){
        Rx := MAXVAL.S
      }
      rundecodestate := updatesample 
      outpix := Rx(COLORWIDTH-1,0)
      updatesampleflag := 2.U
    }
    is(writeback){
      rundecodestate := initial 
      io.rcontrol.finish := true.B 
      io.infen.Aupdate  := true.B 
      //io.infen.Bupdate  := true.B 
      //io.infen.Cupdate  := true.B 
      io.infen.Nupdate  := true.B 
      io.infen.Nnupdate := false.B 
      io.updateen := true.B 
      readrunflag := false.B 
    }
    is(finish){
      rundecodestate := initial
      io.rcontrol.finish := true.B
      io.updateen := true.B  
      readrunflag := false.B 
    }
  }
  

}
