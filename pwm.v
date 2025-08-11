module pwm (
    input  wire        clk,
    input  wire        reset,       // active-high reset

    // Avalon-MM slave interface
    input  wire [4:0]  address,     // 5-bit address (0–15)
    input  wire        write,
    input  wire        chipselect,
    input  wire [31:0] writedata,
    output wire [31:0] readdata,

    // 8 PWM outputs
    output wire [7:0]  pwm_out
);

    // Registers for each of the 8 PWM channels
    reg [31:0] div  [7:0];    // Divider (period)
    reg [31:0] duty [7:0];    // Duty cycle

    // Internal read data register
    reg [31:0] readdata_reg;
    assign readdata = readdata_reg;

    // Counter shared among all channels
    reg [31:0] counter;

    // PWM output register
    reg [7:0] pwm_out_reg;
    assign pwm_out = ~pwm_out_reg;  // Inverted output

    integer i;

    // Register write logic
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            // Reset all DIV and DUTY to defaults
            for (i = 0; i < 8; i = i + 1) begin
                div[i]  <= 32'd100;
                duty[i] <= 32'd50;
            end
        end
        else if (chipselect && write) begin
            // address[4:1] = channel (0–7)
            // address[0]   = 0 → DIV, 1 → DUTY
            if (address[0] == 1'b0)
                div[address[4:1]]  <= writedata;
            else
                duty[address[4:1]] <= writedata;
        end
    end

    // Register read logic
    always @(posedge clk) begin
        if (address[4:1] < 8) begin
            if (address[0] == 1'b0)
                readdata_reg <= div[address[4:1]];
            else
                readdata_reg <= duty[address[4:1]];
        end
        else begin
            readdata_reg <= 32'd0;
        end
    end

    // Shared counter logic
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            counter <= 0;
        end
        else if (counter >= div[0]) begin
            // You can modify this to use max of all div[i]
            counter <= 0;
        end
        else begin
            counter <= counter + 1;
        end
    end

    // PWM generation logic
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            pwm_out_reg <= 8'b0;
        end
        else begin
            for (i = 0; i < 8; i = i + 1) begin
                pwm_out_reg[i] <= (counter < duty[i]) ? 1'b1 : 1'b0;
            end
        end
    end

endmodule
