/*
Halfman算法是一种基于数据的频率分析压缩方法，它通过将出现频率较高的字符用较短的编码表示，出现频率较低的字符用较长的编码表示，从而实现压缩的目的。

Halfman算法的实现步骤如下：

统计待压缩数据中每个字符出现的频率，并按照频率从小到大排序。

将出现频率最低的两个字符合并，形成一个新的节点，该节点的权值为这两个字符的权值之和。

将新节点插入到频率列表中，并重新按照频率从小到大排序。

重复步骤2和步骤3，直到只剩下一个节点为止。

从根节点开始，对每个字符进行编码，左子树为0，右子树为1，将编码表存储起来。

遍历待压缩数据，将每个字符用对应的编码表示，并将编码拼接起来，形成压缩后的数据。

解压缩时，根据编码表将压缩后的数据解码成原始数据。

Halfman算法可以用Verilog硬件语言实现，具体实现步骤与软件实现类似，只是需要将步骤转化为硬件电路。例如，可以使用FPGA实现Halfman算法，其中统计频率的部分可以使用计数器电路，合并节点的部分可以使用加法器和比较器电路，编码表的存储可以使用存储器电路，编码和解码可以使用状态机电路等。


*/


module huffman_encoder (
  input clk,         // 输入时钟
  input rst,         // 输入复位信号
  input [7:0] data_in,   // 输入数据
  output reg [7:0] data_out,  // 输出编码后的数据
  output reg [7:0] bit_count  // 输出编码后数据的位数
);

// 定义符号表
parameter SYMBOL_COUNT = 256;
parameter [7:0] SYMBOLS [SYMBOL_COUNT] = '{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

// 定义节点结构体
typedef struct {
  logic [7:0] symbol;   // 符号
  logic [7:0] count;    // 出现次数
  logic [7:0] code;     // 编码
  logic [7:0] length;   // 编码长度
  logic [7:0] left;     // 左子节点
  logic [7:0] right;    // 右子节点
} node_t;

// 定义节点数组
node_t nodes [SYMBOL_COUNT*2-1];

// 初始化节点数组
initial begin
  for (int i = 0; i < SYMBOL_COUNT; i = i + 1) begin
    nodes[i].symbol = SYMBOLS[i];
    nodes[i].count = 0;
    nodes[i].code = 0;
    nodes[i].length = 0;
    nodes[i].left = 0;
    nodes[i].right = 0;
  end
end

// 定义计数器
reg [7:0] symbol_count = 0;

// 定义编码器状态机
typedef enum {IDLE, COUNT, BUILD, ENCODE} state_t;
reg [1:0] state = IDLE;

// 定义计数器
reg [7:0] count = 0;

// 定义堆栈
reg [7:0] stack [SYMBOL_COUNT*2-1];
reg [7:0] stack_pointer = 0;

// 定义编码器逻辑
always @ (posedge clk or posedge rst) begin
  if (rst) begin
    // 复位状态机和计数器
    state <= IDLE;
    count <= 0;
    bit_count <= 0;
    symbol_count <= 0;
    data_out <= 0;
  end else begin
    case (state)
      IDLE: begin
        // 等待输入数据
        if (data_in != 0) begin
          // 切换到统计状态
          state <= COUNT;
        end
      end
      
      COUNT: begin
        // 统计数据出现次数
        nodes[data_in].count <= nodes[data_in].count + 1;
        symbol_count <= symbol_count + 1;
        count <= count + 1;
        
        // 判断是否统计完毕
        if (count == SYMBOL_COUNT) begin
          // 切换到构建状态
          state <= BUILD;
        end
      end
      
      BUILD: begin
        // 构建哈夫曼树
        for (int i = 0; i < SYMBOL_COUNT-1; i = i + 1) begin
          // 查找出现次数最少的两个节点
          int min1_index = -1;
          int min2_index = -1;
          for (int j = 0; j < SYMBOL_COUNT+i; j = j + 1) begin
            if (nodes[j].count > 0 && (min1_index == -1 || nodes[j].count < nodes[min1_index].count)) begin
              min2_index = min1_index;
              min1_index = j;
            end else if (nodes[j].count > 0 && (min2_index == -1 || nodes[j].count < nodes[min2_index].count)) begin
              min2_index = j;
            end
          end
          
          // 合并两个节点
          nodes[SYMBOL_COUNT+i].count <= nodes[min1_index].count + nodes[min2_index].count;
          nodes[SYMBOL_COUNT+i].left <= min1_index;
          nodes[SYMBOL_COUNT+i].right <= min2_index;
          
          // 清除被合并的节点
          nodes[min1_index].count <= 0;
          nodes[min2_index].count <= 0;
        end
        
        // 切换到编码状态
        state <= ENCODE;
      end
      
      ENCODE: begin
        // 编码输入数据
        int current_node_index = SYMBOL_COUNT*2-2;
        int bit_index = 0;
        while (current_node_index >= 0) begin
          if (nodes[current_node_index].left == 0 && nodes[current_node_index].right == 0) begin
            // 找到符号节点，将编码出栈并输出
            for (int i = 0; i < nodes[current_node_index].length; i = i + 1) begin
              data_out <= {data_out[6:0], stack[stack_pointer]};
              bit_count <= bit_count + 1;
              stack_pointer <= stack_pointer - 1;
            end
            
            // 重置编码器状态
            current_node_index <= SYMBOL_COUNT*2-2;
            bit_index <= 0;
          end else if (bit_index < nodes[current_node_index].length) begin
            // 继续编码
            if (data_in & (1 << bit_index)) begin
              // 当前位为1，进入右子节点
              current_node_index <= nodes[current_node_index].right;
              stack_pointer <= stack_pointer + 1;
              stack[stack_pointer] <= 1;
            end else begin
              // 当前位为0，进入左子节点
              current_node_index <= nodes[current_node_index].left;
              stack_pointer <= stack_pointer + 1;
              stack[stack_pointer] <= 0;
            end
            
            // 增加编码位数
            bit_index <= bit_index + 1;
          end else begin
            // 已完成编码
            break;
          end
        end
        
        // 切换回统计状态
        state <= COUNT;
      end
    endcase
  end
end

endmodule
