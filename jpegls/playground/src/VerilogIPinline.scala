package decoder

import chisel3._
import chisel3.util._
import chisel3.experimental._

class meminline extends BlackBox with HasBlackBoxInline  {
    val io = IO(new Bundle with COMMON{
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
    setInline("/meminline.v",
           """
           |module meminline(
           |  clock				      ,
           |  rst                ,
           |  awready            ,
           |  awvalid            ,
           |  awid               ,
           |  awaddr             ,
           |  awlen              ,
           |  awsize             ,
           |  awburst            ,
           |  wready             ,
           |  wvalid             ,
           |  wdata              ,
           |  wstrb              ,
           |  wlast              ,
           |  bready             ,
           |  bvalid             ,
           |  bid                ,
           |  bresp              ,
           |  arready            ,
           |  arvalid            ,
           |  arid               ,
           |  araddr             ,
           |  arlen              ,
           |  arsize             ,
           |  arburst            ,
           |  rready             ,
           |  rvalid             ,
           |  rid                ,
           |  rresp              ,
           |  rdata              ,
           |  rlast              
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
           |      input  [63:0]wdata   ;           
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
           |      output [63:0] rdata   ;           
           |      output  rlast   ;       
           |
           |blk_mem_gen_0  meminst(
           | .rsta_busy(),
           | .rstb_busy(),
           | .s_aclk       (clock)   ,
           | .s_aresetn    (!rst)   ,
           | .s_axi_awid   (awid)   ,
           | .s_axi_awaddr (awaddr)   ,
           | .s_axi_awlen  (awlen)   ,
           | .s_axi_awsize (awsize)   ,
           | .s_axi_awburst(awburst)   ,
           | .s_axi_awvalid(awvalid)   ,
           | .s_axi_awready(awready)   ,
           | .s_axi_wdata  (wdata)   ,
           | .s_axi_wstrb  (wstrb)   ,
           | .s_axi_wlast  (wlast)   ,
           | .s_axi_wvalid (wvalid)   ,
           | .s_axi_wready (wready)   ,
           | .s_axi_bid    (bid)   ,
           | .s_axi_bresp  (bresp)   ,
           | .s_axi_bvalid (bvalid)   ,
           | .s_axi_bready (bready)   ,
           | .s_axi_arid   (arid)   ,
           | .s_axi_araddr (araddr)   ,
           | .s_axi_arlen  (arlen)   ,
           | .s_axi_arsize (arsize)   ,
           | .s_axi_arburst(arburst)   ,
           | .s_axi_arvalid(arvalid)   ,
           | .s_axi_arready(arready)   ,
           | .s_axi_rid    (rid)   ,
           | .s_axi_rdata  (rdata)   ,
           | .s_axi_rresp  (rresp)   ,
           | .s_axi_rlast  (rlast)   ,
           | .s_axi_rvalid (rvalid)   ,
           | .s_axi_rready (rready) 
           |);
           |
           |
           |
           |endmodule
           """.stripMargin)
}
class raminline extends BlackBox with HasBlackBoxInline {
    val io = IO(new Bundle with COMMON {
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
    setInline("/raminline.v",
           """
           |module raminline(
           |  clock				      ,
           |  rst                ,
           |  awready            ,
           |  awvalid            ,
           |  awid               ,
           |  awaddr             ,
           |  awlen              ,
           |  awsize             ,
           |  awburst            ,
           |  wready             ,
           |  wvalid             ,
           |  wdata              ,
           |  wstrb              ,
           |  wlast              ,
           |  bready             ,
           |  bvalid             ,
           |  bid                ,
           |  bresp              ,
           |  arready            ,
           |  arvalid            ,
           |  arid               ,
           |  araddr             ,
           |  arlen              ,
           |  arsize             ,
           |  arburst            ,
           |  rready             ,
           |  rvalid             ,
           |  rid                ,
           |  rresp              ,
           |  rdata              ,
           |  rlast              
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
           |      input  [63:0]wdata   ;           
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
           |      output [63:0] rdata   ;           
           |      output  rlast   ;       
           |
           |blk_mem_gen_1  meminst(
           | .rsta_busy(),
           | .rstb_busy(),
           | .s_aclk       (clock)   ,
           | .s_aresetn    (!rst)   ,
           | .s_axi_awid   (awid)   ,
           | .s_axi_awaddr (awaddr)   ,
           | .s_axi_awlen  (awlen)   ,
           | .s_axi_awsize (awsize)   ,
           | .s_axi_awburst(awburst)   ,
           | .s_axi_awvalid(awvalid)   ,
           | .s_axi_awready(awready)   ,
           | .s_axi_wdata  (wdata)   ,
           | .s_axi_wstrb  (wstrb)   ,
           | .s_axi_wlast  (wlast)   ,
           | .s_axi_wvalid (wvalid)   ,
           | .s_axi_wready (wready)   ,
           | .s_axi_bid    (bid)   ,
           | .s_axi_bresp  (bresp)   ,
           | .s_axi_bvalid (bvalid)   ,
           | .s_axi_bready (bready)   ,
           | .s_axi_arid   (arid)   ,
           | .s_axi_araddr (araddr)   ,
           | .s_axi_arlen  (arlen)   ,
           | .s_axi_arsize (arsize)   ,
           | .s_axi_arburst(arburst)   ,
           | .s_axi_arvalid(arvalid)   ,
           | .s_axi_arready(arready)   ,
           | .s_axi_rid    (rid)   ,
           | .s_axi_rdata  (rdata)   ,
           | .s_axi_rresp  (rresp)   ,
           | .s_axi_rlast  (rlast)   ,
           | .s_axi_rvalid (rvalid)   ,
           | .s_axi_rready (rready) 
           |);
           |
           |
           |
           |endmodule
           """.stripMargin)
}
class RamwithIP extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val axi = Flipped(new AXIIO)
  })
  val ram = Module(new raminline)
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

class RomwithIP extends Module with COMMON {
  val io = IO(new Bundle {
    // read pix 
    // write bit 
    //input 
    val axi = Flipped(new AXIIO)
  })
  val ram = Module(new meminline)
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


