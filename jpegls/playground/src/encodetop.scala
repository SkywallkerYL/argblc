package decoder 
import chisel3._
import chisel3.util._
import scala.util.Random
/****
jlsencode top

Sequantial logic 
****/

class jpeglsencode extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    val axi = new AXIIO 
    //input 
    val econtrol = new controlIO
    val flush =  Input(Bool())
    val len = Output(UInt(AXIADDRWIDTH.W))
  })

  val A  =  RegInit(VecInit(Seq.fill(367)(0.S(TEXTWIDTH.W))))
  val N  =  RegInit(VecInit(Seq.fill(367)(0.S(TEXTWIDTH.W))))
  val B  =  RegInit(VecInit(Seq.fill(365)(0.S(TEXTWIDTH.W))))
  val C  =  RegInit(VecInit(Seq.fill(365)(0.S(TEXTWIDTH.W))))
  val Nn =  RegInit(VecInit(Seq.fill(2)  (0.S(TEXTWIDTH.W))))

  val chooseQ = Wire(SInt((SIGNWIDTH+1).W))

  val outA  =  Wire(SInt(TEXTWIDTH.W))
  val outN  =  Wire(SInt(TEXTWIDTH.W))
  val outB  =  Wire(SInt(TEXTWIDTH.W))
  val outC  =  Wire(SInt(TEXTWIDTH.W))
  val outNn =  Wire(SInt(TEXTWIDTH.W))

  outA  := 0.S 
  outN  := 0.S 
  outB  := 0.S 
  outC  := 0.S 
  outNn := 0.S 
  for(i <- 0 until 367){
    when(i.S === chooseQ){
      outA := A(i)  
    }
    when(i.S === chooseQ){
      outN := N(i)  
    }
  }
  for(i <- 0 until 365){
    when(i.S === chooseQ){
      outB := B(i) 
    }
    when(i.S === chooseQ){
      outC := C(i)
    }
  }
  when(chooseQ(0).asUInt === 1.U){
    outNn := Nn(0) //:= inNn
  }.otherwise{
    outNn := Nn(1) //:= inNn
  }



  val inA  =  Wire(SInt(TEXTWIDTH.W))
  val inN  =  Wire(SInt(TEXTWIDTH.W))
  val inB  =  Wire(SInt(TEXTWIDTH.W))
  val inC  =  Wire(SInt(TEXTWIDTH.W))
  val inNn =  Wire(SInt(TEXTWIDTH.W))

  val Aen =  Wire(Bool())
  val Nen =  Wire(Bool())
  val Ben =  Wire(Bool())
  val Cen =  Wire(Bool())
  val Nnen =  Wire(Bool())
  Aen  := false.B 
  Nen  := false.B 
  Ben  := false.B 
  Cen  := false.B 
  Nnen := false.B  
  for(i <- 0 until 367){
    when(i.S === chooseQ && Aen){
      A(i) := inA 
    }
    when(i.S === chooseQ && Nen){
      N(i) := inN 
    }
  }
  for(i <- 0 until 365){
    when(i.S === chooseQ && Ben){
      B(i) := inB
    }
    when(i.S === chooseQ && Cen){
      C(i) := inC
    }
  }
  when( Nnen){
    when(chooseQ(0).asUInt === 1.U){
      Nn(0) := inNn
    }.otherwise{
      Nn(1) := inNn
    }
  }

  // pix addr 
  val x = RegInit(0.U(XADDRWIDTH.W))
  val y = RegInit(0.U(YADDRWIDTH.W))
  val xyupdate = Wire(Bool())
  val newx = Wire(UInt(XADDRWIDTH.W))
  val newy = Wire(UInt(YADDRWIDTH.W))
  xyupdate := false.B 
  when(xyupdate){
    x := newx 
    y := newy 
  }
  //D 
  val D1 = RegInit(0.S(SIGNWIDTH.W))
  val D2 = RegInit(0.S(SIGNWIDTH.W))
  val D3 = RegInit(0.S(SIGNWIDTH.W))

  
  //
  val length = RegInit(0.U(32.W))


  //read addr    write addr 
  //read data    write data 
  val readaddr  = RegInit(0.U(AXIADDRWIDTH.W))
  val writeaddr = RegInit(0.U(AXIADDRWIDTH.W))
  val readdata  = RegInit(0.U(BYTEWIDTH.W))
  val writedata = RegInit(0.U(BYTEWIDTH.W))

  //Ix 
  val Ix = RegInit(0.U(BYTEWIDTH.W))

  val defaultaddr = x+y*X_SIZE.U
  // Module inst 
  val getnextsample = Module(new GetNextSample)
  val getq          = Module(new getQ) 
  val pre           = Module(new predict)
  val quanti        = Module(new errvalquanti)
  val rxcompute     = Module(new computeRx)
  val pxcompute     = Module(new computePx)
  val modrange      = Module(new ModRange)
  val lg            = Module(new LG) 
  val bitwrite      = Module(new BitWriter)
  val golomb        = Module(new GolombCoding)

  val regularmode   = Module(new RegularMode)
  val runmode       = Module(new RunMode)

  val runmodeflag = RegInit(0.U(1.W))
  val flushflag   = RegInit(0.U(1.W))

  D1 := getnextsample.io.sample.pix.Ra - getnextsample.io.sample.pix.Rb 
  D2 := getnextsample.io.sample.pix.Rb - getnextsample.io.sample.pix.Rc 
  D3 := getnextsample.io.sample.pix.Rc - getnextsample.io.sample.pix.Ra 

  //module connect 

  //getnextsample 
  getnextsample.io.sample.update := false.B 
  getnextsample.io.getpix.inpix := io.axi.rdata 
  getnextsample.io.getpix.pixvalid := io.axi.rvalid
  getnextsample.io.inaddr.x := x 
  getnextsample.io.inaddr.y := y 
      
  //getq
  getq.io.pix := getnextsample.io.sample.pix
  val Q = RegNext(getq.io.Q)
  chooseQ := Q 
  val sign = RegNext(getq.io.sign) 
  //pre 
  pre.io.a := getnextsample.io.sample.pix.Ra 
  pre.io.b := getnextsample.io.sample.pix.Rb 
  pre.io.c := getnextsample.io.sample.pix.Rc 
  //quanti
  quanti.io.quanti <> regularmode.io.quanti
  //rxcompute 
  rxcompute.io.predict <> runmode.io.predict 
  //pxcomput
  pxcompute.io.predict <> regularmode.io.predict
  //modrange
  modrange.io.modrange <> regularmode.io.modrange
  //lg 
  lg.io.lg <> regularmode.io.lg 
  //bitwrite 
  bitwrite.io.writeaddr := "x80000000".U 
  bitwrite.io.start := false.B 
  bitwrite.io.control.flag   := false.B 
  bitwrite.io.control.opcode := 0.U    
  bitwrite.io.control.bit    := 0.U    
  bitwrite.io.control.bits   := 0.U    
  bitwrite.io.control.number := 0.U    
  io.axi <>  bitwrite.io.axi 
  io.axi.arvalid := getnextsample.io.getpix.addrvalid
  io.axi.araddr  := getnextsample.io.outaddr.x +getnextsample.io.outaddr.y *X_SIZE.U
  io.len := bitwrite.io.outaddr
  //golomb 
  golomb.io.golomb <> regularmode.io.golomb
  // 
  //regularmode 
  regularmode.io.infoin.A  :=  outA 
  regularmode.io.infoin.N  :=  outN 
  regularmode.io.infoin.B  :=  outB 
  regularmode.io.infoin.C  :=  outC 
  regularmode.io.infoin.Nn :=  outNn
  regularmode.io.Q  :=  chooseQ
  regularmode.io.sign := sign 
  regularmode.io.Px := pre.io.Px
  regularmode.io.inpix := (0.U(1.W)##Ix).asSInt
  Aen  := runmode.io.infen.Aupdate 
  Ben  := runmode.io.infen.Bupdate 
  Cen  := runmode.io.infen.Cupdate 
  Nen  := runmode.io.infen.Nupdate 
  Nnen := runmode.io.infen.Nnupdate
  inA  := runmode.io.infoout.A 
  inB  := runmode.io.infoout.B 
  inC  := runmode.io.infoout.C 
  inN  := runmode.io.infoout.N 
  inNn := runmode.io.infoout.Nn
  regularmode.io.rcontrol.start := false.B 
  //runmode 
  runmode.io.infoin.A  :=  outA 
  runmode.io.infoin.N  :=  outN 
  runmode.io.infoin.B  :=  outB 
  runmode.io.infoin.C  :=  outC 
  runmode.io.infoin.Nn :=  outNn
  runmode.io.Q  :=  chooseQ

  runmode.io.inaddr.x := x 
  runmode.io.inaddr.y := y 

  newx := runmode.io.outaddr.x 
  newy := runmode.io.outaddr.y 
  xyupdate := runmode.io.updateen

  runmode.io.sample <> getnextsample.io.sample
  runmode.io.getpix.inpix := Ix 
  runmode.io.getpix.pixvalid := false.B 

  //run mode     golomb
  golomb.io.control <> bitwrite.io.control
  when(runmode.io.runwrite){
    runmode.io.bitwrite <> bitwrite.io.control
  }
  runmode.io.rcontrol.start := false.B 
  //other default 
  //quanti
  runmode.io.quanti.out := quanti.io.quanti.out 
  runmode.io.modrange.out := modrange.io.modrange.out
  runmode.io.lg.k := lg.io.lg.k 
  runmode.io.lg.lcontrol.finish := lg.io.lg.lcontrol.finish
        //golomb 
  runmode.io.golomb.gcontrol.finish := golomb.io.golomb.gcontrol.finish
  //golomb.io.golomb <> runmode.io.golomb
  runmode.io.bitwrite.writing := bitwrite.io.control.writing
  runmode.io.runreset := false.B 
  io.econtrol.finish := false.B

  



  val idle :: updateaddr :: updatesample :: getpix :: runmoding :: regularmoding :: flush :: finish :: Nil = Enum(8) 
  //  0    :: 1          :: 2            :: 3      :: 4         :: 5             :: 6     :: 7      ::
  val state = RegInit(idle)


  switch(state){
    is(idle){
      when(io.econtrol.start){
        state := updatesample 
        x := 0.U 
        y := 0.U 
        for(i <- 0 until 367){
          A(i) := a_init.S  
          N(i) := 1.S 
        }
        for(i <- 0 until 365){
          B(i) := 0.S 
          C(i) := 0.S 
        }
        Nn(0) := 0.S 
        Nn(1) := 0.S 
        runmode.io.rcontrol.start := true.B 
        bitwrite.io.start := true.B 
      }
      when(io.flush){
        bitwrite.io.control.flag   := true.B 
        bitwrite.io.control.opcode := 0.U   
      }
      
    }
    is(updateaddr){
      //when(x =/= 0.U && y =/= 0.U){ 
      state := updatesample 
      when(x === (X_SIZE-1).U){
        x := 0.U 
        when(y =/= (Y_SIZE-1).U){
          y := y + 1.U 
        }.otherwise{
          y := 0.U 
          state := flush
          flushflag := 1.U  
        }
      }.otherwise{
        x := x + 1.U 
      }
      //}
      //state := getErrval
    }
    is(updatesample){
      getnextsample.io.sample.update := true.B 
      getnextsample.io.inaddr.x := x 
      getnextsample.io.inaddr.y := y 
      io.axi.arvalid := getnextsample.io.getpix.addrvalid
      io.axi.araddr  := getnextsample.io.outaddr.x +getnextsample.io.outaddr.y *X_SIZE.U
      //getnextsample.io.getpix.pixvalid := 
      when(getnextsample.io.sample.getvalid){
        getnextsample.io.sample.update := false.B 
        state := getpix
      }
    }
    is(getpix){
      
      io.axi.arvalid := true.B 
      when(runmodeflag === 1.U){
        io.axi.araddr := runmode.io.outaddr.x + runmode.io.outaddr.y*X_SIZE.U 
      }.otherwise{
        io.axi.araddr := defaultaddr
      }
      when(io.axi.rvalid){
        Ix := io.axi.rdata
        when(runmodeflag === 1.U) {
          runmode.io.getpix.pixvalid := true.B 
          state := runmoding 
          runmode.io.getpix.inpix := io.axi.rdata 
        }.elsewhen(ABSH(D1) <= 0.S && ABSH(D2) <= 0.S && ABSH(D3) <= 0.S){
          state := runmoding
          //runmode.io.rcontrol.start := true.B 
          runmodeflag := 1.U 
        }.otherwise{
          state := regularmoding
          //regularmode.io.rcontrol.start := true.B 
        }
      }
      when(runmodeflag === 1.U){
        //quanti
        quanti.io.quanti <> runmode.io.quanti
        //modrange
        modrange.io.modrange <> runmode.io.modrange
        //lg 
        lg.io.lg <> runmode.io.lg 
        //golomb 
        golomb.io.golomb <> runmode.io.golomb
      }
    }
    is(runmoding){
      runmode.io.rcontrol.start := true.B 
      //quanti
      quanti.io.quanti <> runmode.io.quanti
      //modrange
      modrange.io.modrange <> runmode.io.modrange
      //lg 
      lg.io.lg <> runmode.io.lg 
      //golomb 
      golomb.io.golomb <> runmode.io.golomb
      //get nextsample 
      getnextsample.io.inaddr <> runmode.io.outaddr
      chooseQ := runmode.io.outQ
      when(runmode.io.rcontrol.finish === true.B){
        runmode.io.rcontrol.start := false.B 
        state := updateaddr 
        x := runmode.io.outaddr.x 
        y := runmode.io.outaddr.y
        //flushflag := 1.U  
        runmodeflag := 0.U 
      }.elsewhen(runmode.io.getpix.addrvalid){
        state := getpix 
      }
    }
    is(regularmoding){
      regularmode.io.rcontrol.start := true.B
      Aen  := regularmode.io.infen.Aupdate 
      Ben  := regularmode.io.infen.Bupdate 
      Cen  := regularmode.io.infen.Cupdate 
      Nen  := regularmode.io.infen.Nupdate 
      Nnen := regularmode.io.infen.Nnupdate
      inA  := regularmode.io.infoout.A 
      inB  := regularmode.io.infoout.B 
      inC  := regularmode.io.infoout.C 
      inN  := regularmode.io.infoout.N 
      inNn := regularmode.io.infoout.Nn
      when(regularmode.io.rcontrol.finish){
        state := updateaddr 
        regularmode.io.rcontrol.start := false.B
        //flushflag := 1.U 
      }
    }
    is(flush){
      bitwrite.io.control.flag := true.B 
      bitwrite.io.control.opcode := 3.U 
      flushflag := 0.U 
      when(bitwrite.io.control.writing){
        state := finish 
      }
      runmode.io.runreset := true.B 
    }
    is(finish){
      state := idle 
      io.econtrol.finish := true.B 
    }
  }
  //for read pix    // get sample || get pix 
  val arread :: rread :: Nil = Enum(2)
  val readstate = RegInit(arread)

  switch(readstate){
    is(arread){
      when(io.axi.arready && io.axi.arvalid){
        readstate := rread 
      }
    }
    is(rread){
      io.axi.rready := true.B  
      when(io.axi.rvalid){
        readstate := arread
      }
    }
  }  

  val awwrite :: wwrite :: wresp :: Nil = Enum(3)
  val writestate = RegInit(awwrite)
  switch(writestate){
    is(awwrite){
      when(io.axi.awvalid && io.axi.awready){
        writestate := wwrite
      }
    }
    is(wwrite){
      io.axi.wvalid := true.B 
      when(io.axi.wready ){
        writestate := wresp 
      }
    }
    is(wresp){
      io.axi.bready := true.B 
      when(io.axi.bvalid){
        writestate := awwrite
      }
    }
  }




}



class jpeglsencodesimtop extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val econtrol = new controlIO
    val flush = Input(Bool())
    val len = Output(UInt(AXIADDRWIDTH.W)) 
  })
  val jls = Module(new jpeglsencode)
  val ram = Module(new  ramtop)
  jls.io.axi <> ram.io.axi 
  jls.io.econtrol <> io.econtrol 
  jls.io.flush <> io.flush
  io.len := jls.io.len
  //io.econtrol.finish :=  jls.io.econtrol.finish
}