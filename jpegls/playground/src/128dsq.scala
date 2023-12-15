package decoder 
import chisel3._
import chisel3.util._
import scala.util.Random
// 
//128dsq mapper
/*
Input:  2048个编码码字
        1536个未编码码字
输出：1024个Symbol符号  512*2 
每个符号是19bit，13bit表示小数  6bit表示整数

*/
class dsqmapper(num : Int = 128) extends Module with COMMON {
  val io = IO(new Bundle{
    val coded   = Input(Vec(num,UInt(1.W))) 
    val uncoded = Input(Vec(num/4*3,UInt(1.W)))  
    val a1 = Output(Vec(num/4,UInt(19.W))) 
    val a2 = Output(Vec(num/4,UInt(19.W))) 
  })
  val x1 = (Seq.fill(num/4)(Wire(UInt(4.W))))
  val x2 = (Seq.fill(num/4)(Wire(UInt(4.W))))

  val sum1 = (Seq.fill(num/4)(Wire(UInt(6.W))))
  val sum2 = (Seq.fill(num/4)(Wire(UInt(6.W))))
  //Mapping

  for(i <- 0 until num/4){
    val c1 = io.coded(i*4+0)
    val c2 = io.coded(i*4+1)
    val c3 = io.coded(i*4+2)
    val c4 = io.coded(i*4+3)
    
    val u1 = io.uncoded(i*3+0)
    val u2 = io.uncoded(i*3+1)
    val u3 = io.uncoded(i*3+2)

    val x11 = c1^c2 
    val x12 = c1 
    val x13 = u1^u3
    val x14 = (~u1)&u3

    val x21 = c3^c4 
    val x22 = c3 
    val x23 = u2^u3
    val x24 = (u2&u3)|((~u2)&u1)
    //x1(i) := 0.U
    //x2(i) := 0.U 
    x1(i) := Cat(x14,x13,x12,x11)
    x2(i) := Cat(x24,x23,x22,x21) 

    sum1(i) := ((x1(i) + x2(i))<<1.U) - 15.U(6.W)
    sum2(i) := ((x2(i) - x1(i))<<1.U) - 15.U(6.W)

    io.a1(i) := sum1(i) ## 0.U(13.W)//(Fill(13,sum1(i)(4)) ## sum1(i)) << 13.U 
    io.a2(i) := sum2(i) ## 0.U(13.W)//(Fill(13,sum2(i)(4)) ## sum2(i)) << 13.U 
  }
  /*
  val upperBond = BigInt(2).pow(64) - 1 
  val random    = new Random() 
  val randomBigInts = Seq.fill(num * 3) (BigInt(upperBond.bitLength,random).min(upperBond))

  val paramsList = randomBigInts.grouped(3).map{
    case Seq(z1_SEED,z2_SEED,z3_SEED) => RandSeedParams(z1_SEED,z2_SEED,z3_SEED)  
  }.toList
  val Gng = paramsList.map(params => Module(new gng(params)))
  for (i <- 0 until num) {
    Gng(i).io.din := io.din(i) 
    Gng(i).io.dinvalid := io.dinvalid 
    Gng(i).io.p0 := io.p0   
    if (PNUM == 3) {
      io.dout(i) := table3(Gng(i).io.dout).asUInt
    }else if (PNUM == 5) {
      io.dout(i) := table5(Gng(i).io.dout).asUInt
    }else if (PNUM == 15){
      io.dout(i) := Gng(i).io.dout//table15(Gng(i).io.dout).asUInt
    }
    io.doutvalid := Gng(i).io.doutvalid 
  }
  */
}

// llr count

class llrcomput extends Module with COMMON {
  val io = IO(new Bundle{
    val x = Input(UInt(19.W)) 
    val llr = Output(UInt(19.W)) 
    //val LLR   = Input(Vec(2048,UInt(19.W)))  
  })
  val bound1 = (1.U(7.W) ## 0.U(12.W))//0.5
  val bound2 = (5.U(7.W) ## 0.U(12.W))//2.5

  val out1 = (3.U(7.W) ## 0.U(12.W))//1.5
  val out2 = (7.U(7.W) ## 0.U(12.W))//3.5
  when(io.x <= bound1){
    io.llr := io.x + bound1
  }.elsewhen(io.x > bound1 && io.x <= bound2){
    io.llr := out1 - io.x
  }.otherwise{
    io.llr := io.x - out2
  }
}
//dsq channel llr 

class dsqllr extends Module with COMMON {
  val io = IO(new Bundle{
    val x1 = Input(UInt(19.W)) 
    val x2 = Input(UInt(19.W))
    val llrc1 = Output(UInt(19.W)) 
    val llrc2 = Output(UInt(19.W)) 
    val llrc3 = Output(UInt(19.W)) 
    val llrc4 = Output(UInt(19.W)) 
    //val LLR   = Input(Vec(2048,UInt(19.W)))  
  })

  val getx1 = io.x1 & ((3.U(6.W)##Fill(13,1.U(1.W)))) // 将其限制在0-4的范围
  val getx2 = (io.x1 + (1.U(6.W) ## 0.U(13.W)))  &((3.U(6.W)##Fill(13,1.U(1.W))))
  val getx3 = io.x2 & ((3.U(6.W)##Fill(13,1.U(1.W))))
  val getx4 = (io.x2 + (1.U(6.W) ## 0.U(13.W)))  &((3.U(6.W)##Fill(13,1.U(1.W))))

  val llr1 = Module(new llrcomput)
  val llr2 = Module(new llrcomput)
  val llr3 = Module(new llrcomput)
  val llr4 = Module(new llrcomput)

  llr1.io.x := getx1
  io.llrc1  := llr1.io.llr

  llr2.io.x := getx2
  io.llrc2  := llr2.io.llr
  
  llr3.io.x := getx3
  io.llrc3  := llr3.io.llr
  
  llr4.io.x := getx4
  io.llrc4  := llr4.io.llr

  //io.a1 := x1 
  //io.a2 := x2 
  //val sum1 = (Seq.fill(512)(Wire(UInt(6.W))))
  
}

/*
llr quanti
*/
class llrquanti extends Module with COMMON {
  val io = IO(new Bundle{ 
    val llrin  = Input(UInt(19.W)) 
    val llrout = Output(UInt(4.W))  
  })
  val value = io.llrin.asUInt()
  val signed = io.llrin(18)
  val out = Wire(UInt(19.W))
  val bound1  = Wire(UInt(19.W))
  val bound2  = Wire(UInt(19.W))
  val bound3  = Wire(UInt(19.W))
  val bound4  = Wire(UInt(19.W))
  val bound5  = Wire(UInt(19.W))
  val bound6  = Wire(UInt(19.W))
  val bound7  = Wire(UInt(19.W))
  val bound_1 = Wire(UInt(19.W))
  val bound_2 = Wire(UInt(19.W))
  val bound_3 = Wire(UInt(19.W))
  val bound_4 = Wire(UInt(19.W))
  val bound_5 = Wire(UInt(19.W))
  val bound_6 = Wire(UInt(19.W))
  val bound_7 = Wire(UInt(19.W))
  bound1  := "b1111111111011011011".U
  bound2  := "b1111111110010010010".U
  bound3  := "b1111111101001001001".U
  bound4  := "b1111111100000000000".U
  bound5  := "b1111111010110110111".U
  bound6  := "b1111111001101101110".U
  bound7  := "b1111111000100100101".U
  bound_1 := "b0000000000100100100".U
  bound_2 := "b0000000001101101101".U
  bound_3 := "b0000000010110110110".U
  bound_4 := "b0000000100000000000".U
  bound_5 := "b0000000101001001001".U
  bound_6 := "b0000000110010010010".U
  bound_7 := "b0000000111011011011".U
  val minusvalue1  = Wire(UInt(4.W))
  val minusvalue2  = Wire(UInt(4.W))
  val minusvalue3  = Wire(UInt(4.W))
  val minusvalue4  = Wire(UInt(4.W))
  val minusvalue5  = Wire(UInt(4.W))
  val minusvalue6  = Wire(UInt(4.W))
  val minusvalue7  = Wire(UInt(4.W))
  minusvalue1   := (15).S.asUInt
  minusvalue2   := (14).S.asUInt
  minusvalue3   := (13).S.asUInt
  minusvalue4   := (12).S.asUInt
  minusvalue5   := (11).S.asUInt
  minusvalue6   := (10).S.asUInt
  minusvalue7   := (9).S.asUInt
  when(signed === 1.U){
    when(value <= bound7) {
      out := minusvalue7
    }.elsewhen(value <= bound6){
      out := minusvalue6
    }.elsewhen(value <= bound5){
      out := minusvalue5
    }.elsewhen(value <= bound4){
      out := minusvalue4
    }.elsewhen(value <= bound3){
      out := minusvalue3
    }.elsewhen(value <= bound2){
      out := minusvalue2
    }.elsewhen(value <= bound1){
      out := minusvalue1
    }.otherwise{
      out := (0.S).asUInt
    }
  }.otherwise{
    when(value >= bound_7) {
      out := (7.S).asUInt
    }.elsewhen(value >= bound_6){
      out := (6.S).asUInt
    }.elsewhen(value >= bound_5){
      out := (5.S).asUInt
    }.elsewhen(value >= bound_4){
      out := (4.S).asUInt
    }.elsewhen(value >= bound_3){
      out := (3.S).asUInt
    }.elsewhen(value >= bound_2){
      out := (2.S).asUInt
    }.elsewhen(value >= bound_1){
      out := (1.S).asUInt
    }.otherwise{
      out := (0.S).asUInt
    }
  }



  //when(signed <= bound7.asSInt){
  //  io.llrout := (-7.S).asUInt
  //}.elsewhen(signed <= bound6.asSInt){
  //  io.llrout := (-6.S).asUInt
  //}.elsewhen(signed <= bound5.asSInt){
  //  io.llrout := (-5.S).asUInt
  //}.elsewhen(signed <= bound4.asSInt){
  //  io.llrout := (-4.S).asUInt
  //}.elsewhen(signed <= bound3.asSInt){
  //  io.llrout := (-3.S).asUInt
  //}.elsewhen(signed <= bound2.asSInt){
  //  io.llrout := (-2.S).asUInt
  //}.elsewhen(signed <= bound1.asSInt){
  //  io.llrout := (-1.S).asUInt
  //}.elsewhen(signed <= 0.S){
  //  io.llrout := 0.S.asUInt
  //}.else
  io.llrout := out
  //val sum1 = (Seq.fill(512)(Wire(UInt(6.W))))
}




//128 dsq demapper
/*
Input: 1024个Symbol符号  512*2   
输出：2048个bit对应的LLR
每个符号是19bit，13bit表示小数  6bit表示整数

*/ 
class dsqdemapper (num : Int = 128) extends Module with COMMON {
  val io = IO(new Bundle{
    val s1  = Input(Vec(num/4,UInt(19.W))) 
    val s2  = Input(Vec(num/4,UInt(19.W))) 
    val llr = Output(Vec(num,UInt(4.W)))  
  })
  val getx1 = (Seq.fill(num/4)(Wire(UInt(19.W))))
  val getx2 = (Seq.fill(num/4)(Wire(UInt(19.W))))

  val x1 = (Seq.fill(num/4)(Wire(UInt(19.W))))
  val x2 = (Seq.fill(num/4)(Wire(UInt(19.W))))
  //val sum1 = (Seq.fill(512)(Wire(UInt(5.W))))
  //val sum2 = (Seq.fill(512)(Wire(UInt(5.W))))
  //Mapping

  val llrgroup  =  Seq.fill(num/4)(Module(new dsqllr))  
  val quantillr =  Seq.fill(num)(Module(new llrquanti))  
  for(i <- 0 until num/4){
    //x1(i) := 0.U
    //x2(i) := 0.U 
    getx1(i) := (io.s1(i)-io.s2(i))
    getx2(i) := (io.s1(i)+io.s2(i)+(30.U(6.W)##0.U(13.W)))
    x1(i) :=  (Fill(2,getx1(i)(18)) ## getx1(i)(18,2)) //进行一个➗4的操作
    x2(i) :=  (Fill(2,getx2(i)(18)) ## getx2(i)(18,2)) //
    llrgroup(i).io.x1 := x1(i)
    llrgroup(i).io.x2 := x2(i)
    quantillr(i*4+0).io.llrin := llrgroup(i).io.llrc1
    quantillr(i*4+1).io.llrin := llrgroup(i).io.llrc2
    quantillr(i*4+2).io.llrin := llrgroup(i).io.llrc3
    quantillr(i*4+3).io.llrin := llrgroup(i).io.llrc4
    io.llr(i*4+0) := quantillr(i*4+0).io.llrout
    io.llr(i*4+1) := quantillr(i*4+1).io.llrout
    io.llr(i*4+2) := quantillr(i*4+2).io.llrout
    io.llr(i*4+3) := quantillr(i*4+3).io.llrout
    //io.a1(i) := sum1(i) ## 0.U(13.W)//(Fill(13,sum1(i)(4)) ## sum1(i)) << 13.U 
    //io.a2(i) := sum2(i) ## 0.U(13.W)//(Fill(13,sum2(i)(4)) ## sum2(i)) << 13.U 
  }
  //val sum1 = (Seq.fill(512)(Wire(UInt(6.W))))
}

