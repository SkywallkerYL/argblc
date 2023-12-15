module ysyx_22050550_Reg #(
    WIDTH = 1, RESET_VAL = 0
) (
    input                   clock,
    input                   reset,
    input                   wen,
    input      [WIDTH-1:0]  din,
    output reg [WIDTH-1:0] dout
);
    always @(posedge clock) begin
        if (reset) dout <= RESET_VAL;
        else if (wen) dout <= din;
    end
endmodule
