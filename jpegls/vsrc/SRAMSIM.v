module SRAMSIM(	
  input         clock                   ,
                reset                   ,
                io_Sram_ar_valid        ,
  input  [63:0] io_Sram_ar_bits_addr    ,
  input         io_Sram_r_ready         ,
  input  [7:0]  io_ar_len               ,
  input  [2:0]  io_ar_size              ,
  input  [1:0]  io_ar_burst             ,
  input         io_Sram_aw_valid        ,
  input  [63:0] io_Sram_aw_bits_addr    ,
  input  [7:0]  io_aw_len               ,
  input  [2:0]  io_aw_size              ,   
  input  [1:0]  io_aw_burst             ,
  input         io_Sram_w_valid         ,
  input  [63:0] io_Sram_w_bits_data     ,
  input  [7:0]  io_Sram_w_bits_strb     ,
  input			io_b_ready				,
  output		io_b_valid				,
  output [1:0]  io_b_bresp				,
  output        io_Sram_ar_ready        ,
                io_Sram_r_valid         ,
  output [63:0] io_Sram_r_bits_data     ,
  output [ 1:0] io_Sram_r_rresp			,
  output        io_Sram_r_bits_last     ,
                io_Sram_aw_ready        ,
                io_Sram_w_ready
);
    wire Dpi_wflag ,Dpi_rflag;
    wire [63:0] Dpi_raddr, Dpi_waddr ,Dpi_rdata, Dpi_wdata;
    wire [7:0]  Dpi_wmask; 
    reg [63:0] raddrReg, waddrReg;
	wire raddren =( io_Sram_ar_valid && io_Sram_ar_ready )||(io_Sram_r_valid && io_Sram_r_ready);
	wire [63:0] raddrin = ( io_Sram_ar_valid && io_Sram_ar_ready ) ? io_Sram_ar_bits_addr 
						: ( io_Sram_r_valid  && io_Sram_r_ready  ) ? raddrReg + 64'd8 : raddrReg ;
    ysyx_22050550_Reg # (64,64'd0)Regraddr(
    .clock(clock),.reset(reset),.wen(raddren),
    .din(raddrin),.dout(raddrReg));
	wire waddren =( io_Sram_aw_valid && io_Sram_aw_ready )||(io_Sram_w_valid && io_Sram_w_ready);
	wire [63:0] waddrin = ( io_Sram_aw_valid && io_Sram_aw_ready ) ? io_Sram_aw_bits_addr 
						: ( io_Sram_w_valid  && io_Sram_w_ready  ) ? waddrReg + 64'd8 : waddrReg ; 
    ysyx_22050550_Reg # (64,64'd0)Regwaddr(
    .clock(clock),.reset(reset),.wen(waddren),
    .din(waddrin),.dout(waddrReg));
 
    //实现类似chisel 只不过用DPI访问内存
    //仅支持INCR型突发传输   size 设定为 128//CACHE那边的突发传输类型只能是这个
    //读状态机
	//read 的response 信号跟rdata是在一起的 
	//write 的response 需要一个新的状态，因为是一个新的通道
    localparam readwait = 1'b0 , read = 1'b1;
    reg ReadState,ReadNext;
    always @(posedge clock) begin
        if (reset) ReadState <= readwait;
        else ReadState <= ReadNext;
    end
    reg [7:0] Reglen;
    always @(*) begin
        case (ReadState)
            readwait: begin
                if(io_Sram_ar_valid&&io_Sram_ar_ready) begin
                    ReadNext = read;
                end
                else ReadNext = readwait;
            end 
            read: begin
                if(Reglen!=ARLEN || (!io_Sram_r_ready)) begin
                    ReadNext = read;
                end 
                else ReadNext = readwait;
            end
           default: ReadNext = readwait;
        endcase
    end
    //faster
    
    wire [6:0] ReadAddrAdd;
    //DPI最多只支持64位，更大的size一次也只传64个
    assign ReadAddrAdd = io_ar_size ==  3'b000 ? 7'd1
                        :io_ar_size ==  3'b001 ? 7'd2   
                        :io_ar_size ==  3'b010 ? 7'd4   
                        :io_ar_size ==  3'b011 ? 7'd8 : 7'd8;
     
    wire [63:0] ReadData ;
    assign ReadData = io_ar_size == 3'b000 ? {{56{1'b0}},Dpi_rdata[ 7:0]} :
                      io_ar_size == 3'b001 ? {{48{1'b0}},Dpi_rdata[15:0]} :
                      io_ar_size == 3'b010 ? {{32{1'b0}},Dpi_rdata[31:0]} :
                      io_ar_size == 3'b011 ? Dpi_rdata : Dpi_rdata;
                      
    
    assign io_Sram_ar_ready = ReadState == readwait;
    wire ReglenEn = (ReadState == readwait && io_Sram_ar_valid) 
                 || (ReadState == read && io_Sram_r_valid&& io_Sram_r_ready) ;
    wire [7:0] RegLenIn = (ReadState == readwait && io_Sram_ar_valid) ? 0
                        : (ReadState == read && io_Sram_r_ready) ? Reglen+1 : Reglen;
    ysyx_22050550_Reg # (8,8'd0) RegLen (
        .clock(clock),
        .reset(reset),
        .wen(ReglenEn),
        .din(RegLenIn),
        .dout(Reglen)
    );
	reg [7:0] ARLEN ; 
	ysyx_22050550_Reg # (8,8'd0) arlen (
        .clock(clock),
        .reset(reset),
        .wen(ReadState == readwait &&io_Sram_ar_valid),
        .din(io_ar_len),
        .dout(ARLEN)
    );

    /*
		00 : OKAY   : NORMAL access success
		01 : EXOKAY : Exclusive accsess OKAY 
		10 : SLEVRR : Slave error 
		11 : DECERR : Decode error 
	*/ 
    assign io_Sram_r_bits_last = ReadState == read && Reglen == ARLEN;
    //目前只有两种情况，先这样写了  作为设备内存 arlen本身就是0
    assign Dpi_raddr =  raddrReg ; 
    assign io_Sram_r_bits_data = ReadData;	
    assign io_Sram_r_rresp = 2'b00;			
    assign io_Sram_r_valid = ReadState == read;
    assign Dpi_rflag = io_Sram_r_valid && io_Sram_r_ready;
    //写状态机
    localparam writewait = 2'b00 , write = 2'b01 , writeresponse = 2'b10;
    reg [1:0]WriteState,WriteNext;
    always @(posedge clock) begin
        if (reset) WriteState <= writewait ;
        else WriteState <= WriteNext;
 
    end
    reg [7:0] WReglen;
    always @(*) begin
        case (WriteState)
            writewait: begin
                if(io_Sram_aw_valid&&io_Sram_aw_ready) begin
                    WriteNext = write;
                end
                else WriteNext = writewait;
            end 
            write: begin
                if(WReglen!=AwLEN) begin
                    WriteNext = write;
                end
				else begin 
					WriteNext = writeresponse; 
				end
            end
			writeresponse: begin 
				if (io_b_ready && io_b_valid) begin 
					if(io_b_bresp == 2'b00) begin 
						WriteNext = writewait; 
					end 
					else writeresponse = writewait ;
				end
				else WriteNext = writeresponse ; 
			end 
            default: WriteNext = writewait;
        endcase
    end
    wire writeOkay = 1'b1; 
	assign io_b_bresp = 2'b00;
    assign io_b_valid = WriteState == writeresponse;
    wire [6:0] WriteAddrAdd;
    //DPI最多只支持64位，更大的size一次也只传64个
    assign WriteAddrAdd =  io_aw_size == 3'b000 ?  7'd1 :
                           io_aw_size == 3'b001 ?  7'd2 :
                           io_aw_size == 3'b010 ?  7'd4 :
                           io_aw_size == 3'b011 ?  7'd8 :7'd8;
        
    
    wire [63:0] WriteData ;
    assign WriteData = io_aw_size == 3'b000 ? {{56{1'b0}},io_Sram_w_bits_data[ 7:0]}: 
                       io_aw_size == 3'b001 ? {{48{1'b0}},io_Sram_w_bits_data[15:0]}: 
                       io_aw_size == 3'b010 ? {{32{1'b0}},io_Sram_w_bits_data[31:0]}: 
                       io_aw_size == 3'b011 ? io_Sram_w_bits_data :io_Sram_w_bits_data;
    
  
    assign io_Sram_aw_ready = WriteState == writewait;
    wire WReglenEn = (WriteState == writewait && io_Sram_aw_valid) 
                 || (WriteState == write && io_Sram_w_valid);
    wire [7:0] WRegLenIn = (WriteState == writewait && io_Sram_aw_valid) ? 0
                        : (WriteState == write && io_Sram_w_valid) ? WReglen+1 : WReglen;
    ysyx_22050550_Reg # (8,8'd0) WRegLen (
        .clock(clock),
        .reset(reset),
        .wen(WReglenEn),
        .din(WRegLenIn),
        .dout(WReglen)
    );
	reg [7:0] AwLEN ; 
	ysyx_22050550_Reg # (8,8'd0) awlen (
        .clock(clock),
        .reset(reset),
        .wen(WriteState == writewait && io_Sram_aw_valid),
        .din(io_aw_len),
        .dout(AwLEN)
    );

    //目前只有两种情况，先这样写了
    assign Dpi_waddr =  waddrReg ;
    assign Dpi_wdata =  WriteData;
    assign Dpi_wmask =  io_Sram_w_bits_strb;
    assign io_Sram_w_ready = WriteState == write;
    assign Dpi_wflag =  io_Sram_w_valid && io_Sram_w_ready;
    
import "DPI-C" function void pmem_read(input longint Dpi_raddr, output longint Dpi_rdata);
import "DPI-C" function void pmem_write(input longint Dpi_waddr, input longint Dpi_wdata,input byte Dpi_wmask);

    always@(negedge clock )begin
       if(Dpi_wflag) pmem_write(Dpi_waddr,Dpi_wdata,Dpi_wmask);
    end
    always@(negedge clock)begin
       if(Dpi_rflag) pmem_read(Dpi_raddr,Dpi_rdata);
    end
    /*
    always@(Dpi_wflag)begin
       if(Dpi_wflag) pmem_write(Dpi_waddr,Dpi_wdata,Dpi_wmask);
    end
    always@(Dpi_rflag)begin
       if(Dpi_rflag) pmem_read(Dpi_raddr,Dpi_rdata);
    end
    */
endmodule

