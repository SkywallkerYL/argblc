module huffman_decoder (
    input clk, // 时钟信号
    input rst, // 复位信号
    input [7:0] encoded_data, // 压缩后的数据
    input [7:0] encoded_length, // 压缩后的数据长度
    output reg [7:0] decoded_data // 解压后的数据
);

    // 编码表，用于将编码转换为字符
    reg [7:0] code_table [0:255];
    // 编码表的长度
    reg [7:0] code_table_length;
    // 当前解码的编码
    reg [31:0] current_code;
    // 当前解码的编码长度
    reg [7:0] current_length;
    // 当前解码的字符
    reg [7:0] current_char;
    // 压缩后的数据剩余长度
    reg [7:0] remaining_length;
    // 是否已经解压完所有数据
    reg [0:0] is_done;

    // 初始化编码表
    initial begin
        // TODO: 将编码表存储在ROM中，这里使用硬编码的方式
        code_table[8'h41] = 8'h00;
        code_table[8'h42] = 8'h01;
        code_table[8'h43] = 8'h10;
        code_table_length = 3;
    end

    // 解压缩状态机
    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            // 复位状态机和输出数据
            current_code <= 32'h0;
            current_length <= 8'h0;
            current_char <= 8'h0;
            remaining_length <= 8'h0;
            is_done <= 1'b0;
            decoded_data <= 8'h0;
        end else begin
            // 如果还有数据未解压
            if (!is_done) begin
                // 如果当前解码的编码长度小于压缩后的数据长度
                if (current_length < encoded_length) begin
                    // 将压缩后的数据拼接到当前解码的编码中
                    current_code <= {current_code, encoded_data};
                    // 更新当前解码的编码长度
                    current_length <= current_length + 8'h8;
                end else begin
                    // 如果当前解码的编码长度已经等于压缩后的数据长度，说明已经解压完所有数据
                    is_done <= 1'b1;
                end

                // 解码当前编码
                for (int i = 0; i < code_table_length; i = i + 1) begin
                    // 如果当前编码和编码表中的某个编码匹配
                    if (current_code[31:32-code_table[i][7:0]] == code_table[i][7:0]) begin
                        // 更新当前解码的字符
                        current_char <= code_table[i][7:0];
                        // 更新当前解码的编码
                        current_code <= current_code >> code_table[i][7:0];
                        // 更新当前解码的编码长度
                        current_length <= current_length - code_table[i][7:0];
                        // 输出解压后的数据
                        decoded_data <= current_char;
                        // 退出循环
                        break;
                    end
                end
            end
        end
    end

endmodule
