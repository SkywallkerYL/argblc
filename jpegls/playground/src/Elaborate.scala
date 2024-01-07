package  decoder 
import circt.stage._
import decoder._

object Elaborate extends App {
  // isNear GetNextSample gradientQuantize getQ predict errvalquanti computeRx
  //ModRange LG BitWriter GolombCoding RunMode RegularMode jpeglsencode jpeglsencodesimtop
  // BitReader GolombdeCoding updateSample RegularModeDecoding RunModedecoding
  // jpeglsdecode 
  //jpeglsencodesimtop jpeglsdecodesimtop
  // tilecompresssimtop tiledecompresssimtop
  def top = new tiledecompresssimtop
  val useMFC = false // use MLIR-based firrtl compiler
  val generator = Seq(
    chisel3.stage.ChiselGeneratorAnnotation(() => top),
    //firrtl.stage.RunFirrtlTransformAnnotation(new AddModulePrefix()),
    //ModulePrefixAnnotation("ysyx_22050550_")
  )
  if (useMFC) {
    (new ChiselStage).execute(args, generator :+ CIRCTTargetAnnotation(CIRCTTarget.Verilog))
  } else{
    (new chisel3.stage.ChiselStage).execute(args, generator)
  }
}
