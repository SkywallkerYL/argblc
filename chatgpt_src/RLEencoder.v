module rle_encoder (
    input clk, // 时钟信号
    input rst, // 复位信号
    input [7:0] data_in, // 待压缩的数据
    output reg [7:0] data_out, // 压缩后的数据
    output reg [7:0] count_out, // 压缩后的计数
    output reg [0:0] is_done // 是否已经压缩完所有数据
);

    // 上一个输入的数据
    reg [7:0] last_data;
    // 上一个输入的数据出现的次数
    reg [7:0] count;
    // 是否是第一次输入数据
    reg [0:0] is_first;

    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            // 复位状态机和输出数据
            last_data <= 8'h0;
            count <= 8'h0;
            is_first <= 1'b1;
            data_out <= 8'h0;
            count_out <= 8'h0;
            is_done <= 1'b0;
        end else begin
            // 如果还有数据未压缩
            if (!is_done) begin
                // 如果是第一次输入数据，则直接输出
                if (is_first) begin
                    data_out <= data_in;
                    count_out <= 8'h1;
                    is_first <= 1'b0;
                end else begin
                    // 如果当前输入的数据和上一个输入的数据相同，则累加出现次数
                    if (data_in == last_data) begin
                        count <= count + 8'h1;
                    end else begin
                        // 如果当前输入的数据和上一个输入的数据不同，则输出上一个输入的数据和出现次数
                        data_out <= last_data;
                        count_out <= count;
                        // 更新上一个输入的数据和出现次数
                        last_data <= data_in;
                        count <= 8'h1;
                    end
                    // 如果已经输入完所有数据
                    if (count_out == 8'h0) begin
                        // 输出最后一个数据和出现次数
                        data_out <= last_data;
                        count_out <= count;
                        // 标记已经压缩完所有数据
                        is_done <= 1'b1;
                    end
                end
            end
        end
    end

endmodule

module rle_decoder (
    input clk, // 时钟信号
    input rst, // 复位信号
    input [7:0] data_in, // 压缩后的数据
    input [7:0] count_in, // 压缩后的计数
    output reg [7:0] data_out // 解压后的数据
);

    // 当前解压的数据
    reg [7:0] current_data;
    // 当前解压的数据出现的次数
    reg [7:0] current_count;
    // 压缩后的数据剩余数量
    reg [7:0] remaining_count;
    // 是否已经解压完所有数据
    reg [0:0] is_done;

    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            // 复位状态机和输出数据
            current_data <= 8'h0;
            current_count <= 8'h0;
            remaining_count <= 8'h0;
            is_done <= 1'b0;
            data_out <= 8'h0;
        end else begin
            // 如果还有数据未解压
            if (!is_done) begin
                // 如果当前解压的数据出现次数还没解压完
                if (current_count > 8'h0) begin
                    // 输出当前解压的数据
                    data_out <= current_data;
                    // 更新当前解压的数据出现次数
                    current_count <= current_count - 8'h1;
                end else begin
                    // 如果当前解压的数据出现次数已经解压完，需要解压下一个数据
                    if (remaining_count > 8'h0) begin
                        // 解压下一个数据
                        current_data <= data_in;
                        current_count <= count_in;
                        remaining_count <= remaining_count - 8'h1;
                    end else begin
                        // 如果已经解压完所有数据
                        is_done <= 1'b1;
                    end
                end
            end
        end
    end

endmodule
