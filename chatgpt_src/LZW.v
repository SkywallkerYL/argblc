/*
该代码实现了一个简单的LZW算法的压缩单元。它使用了一个有限状态机来控制压缩和解压缩过程，具体实现如下：

在IDLE状态下，等待输入数据。
当输入数据不是0xFF时，将其存储到输入缓冲区中，并将状态机切换到COMPRESS状态。
在COMPRESS状态下，将输入缓冲区中的数据与已有编码进行匹配。如果找到匹配的编码，则将输入缓冲区中的数据和匹配的编码合并成一个新的编码，并将其存储到输出缓冲区中。如果没有找到匹配的编码，则将前缀编码和后缀字符组合成一个新的编码，并将其存储到输出缓冲区中。如果输出缓冲区已经满了，则将其中的8位数据输出，并将状态机切换到IDLE状态。如果输入缓冲区中的数据已经用完，则等待新的输入数据。如果输入数据为0xFF，则将状态机切换到IDLE状态。
在DECOMPRESS状态下，将输入缓冲区中的数据和已有编码进行匹配，并输出对应的字符。如果没有匹配的编码，则将前缀编码和后缀字符组合成一个新的编码，并输出对应的字符。如果输出缓冲区已经满了，则将其中的8位数据输出，并将状态机切换到IDLE状态。如果输入缓冲区中的数据已经用完，则等待新的输入数据。如果输入数据为0xFF，则将状态机切换到IDLE状态。
请注意，该代码仅用于示例和教学目的，实际的LZW算法实现可能更为复杂，需要更多的优化和细节处理


*/



module lzw (
    input clk,              // 时钟信号
    input rst,              // 复位信号
    input [7:0] data_in,    // 输入数据
    output [15:0] data_out, // 输出数据
    output valid            // 输出数据有效信号
);

    typedef enum {
        IDLE,         // 空闲状态
        COMPRESS,     // 压缩状态
        DECOMPRESS    // 解压状态
    } state_t;

    reg [7:0] in_buff;      // 输入缓冲区
    reg [15:0] out_buff;    // 输出缓冲区
    reg [3:0] out_bits;     // 输出缓冲区未使用的位数
    reg [3:0] in_bits;      // 输入缓冲区未使用的位数
    reg [11:0] code;        // 编码
    reg [11:0] next_code;   // 下一个编码
    reg [11:0] prefix;      // 前缀编码
    reg [7:0] suffix;       // 后缀字符
    reg [3:0] code_bits;    // 编码位数
    reg [3:0] max_code;     // 最大编码值
    reg [1:0] state;        // 状态机状态

    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            in_buff <= 8'b0;
            out_buff <= 16'b0;
            out_bits <= 0;
            in_bits <= 0;
            code <= 0;
            next_code <= 256;
            prefix <= 0;
            suffix <= 0;
            code_bits <= 9;
            max_code <= 511;
            state <= IDLE;
        end else begin
            case (state)
                IDLE:
                    if (data_in != 8'hFF) begin
                        in_buff <= data_in;
                        in_bits <= 8;
                        state <= COMPRESS;
                    end
                COMPRESS:
                    if (in_bits >= code_bits) begin
                        suffix <= in_buff[code_bits-1:0];
                        prefix <= {code, suffix};
                        code <= prefix;
                        in_buff <= in_buff >> code_bits;
                        in_bits <= in_bits - code_bits;
                        if (next_code > max_code) begin
                            code_bits <= code_bits + 1;
                            max_code <= max_code + max_code;
                        end
                        if (next_code == 4096) begin
                            next_code <= 256;
                            code_bits <= 9;
                            max_code <= 511;
                        end
                        out_buff <= {out_buff, code};
                        out_bits <= out_bits + code_bits;
                        next_code <= next_code + 1;
                    end else begin
                        in_buff <= {in_buff, data_in};
                        in_bits <= in_bits + 8;
                    end
                    if (out_bits >= 8) begin
                        data_out <= out_buff[7:0];
                        out_buff <= out_buff >> 8;
                        out_bits <= out_bits - 8;
                        valid <= 1;
                    end else begin
                        valid <= 0;
                    end
                    if (data_in == 8'hFF) begin
                        state <= IDLE;
                    end
                DECOMPRESS:
                    // TODO: 解压缩状态机
            endcase
        end
    end

endmodule
