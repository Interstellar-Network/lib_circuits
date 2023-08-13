//Linear Feedback Shift Register (combinational)
// used as a random generator 
// to generate the selected probabilty of displaying the digits segments

module LFSR_comb #(
    parameter BITSIZE = 10,
    parameter COUNTERSIZE = 4
    )(
    input [`RNDSIZE-1:0] seed,
    input [COUNTERSIZE-1:0] probability, // 4-bit input to choose the probability
    input [COUNTERSIZE-1:0] count,
    output [`BITMAP_NB_SEGMENTS-1:0] rnd
);
    //Desired probability match the threshold value to test the number of bits sets to 1 in bitslice
    // probability 0.5  = 4'b0101;  // 0.5 if mask applied slightly < 0.4 either
    // probability 0.6  = 4'b0100;  // 0.6
    // probability 0.7  = 4'b0100;  // 0.7 if mask applied
    // probability 0.8  = 4'b0011;  // 0.8
    // probability 0.9  = 4'b0010;  // 0.9
    // probability +0.9 = 4'b0001;  // 0.98
    // probability 1    = 4'b0000;  // 1 all bits set
    // Decimal number of bits set to 1 in bitslice
    // 0: 4'b0000 1: 4'b0001 2: 4'b0010 3: 4'b0011 4: 4'b0100 5: 4'b0101
    // 6: 4'b0110 7: 4'b0111 8: 4'b1000 9: 4'b1001 10: 4'b1010

    // mask different from 0 to add bits in bitslice to refine specific probability
    localparam MASK_10  = 10'b0000000000;  
    localparam MASK_09P = 10'b0000000000; 
    localparam MASK_09  = 10'b0000000000; 
    localparam MASK_08  = 10'b0000000000;
    localparam MASK_07  = 10'b0000000001; // to apply for 0,7
    localparam MASK_06  = 10'b0000000000;  
    localparam MASK_05  = 10'b0000000001; // to apply for 0.5
    //TO DO refine probabilities by increasing threshold and set more bits to 1 with masks.

    // Ensure the declarations of this wires otherwise i.e slected_threshold assigment works but with the wrong value
    // cf: Yosys "Warning: Identifier \selected_threshold' is implicitly declared"
    wire [COUNTERSIZE-1:0] selected_threshold;  
    wire [COUNTERSIZE-1:0] selected_mask;
    // Determine the threshold based i.e input probability value = threshold
    assign selected_threshold = probability;

    // Define the mask used to fine-tune probability
    assign selected_mask =  (selected_threshold == 4'b0000) ? MASK_10  :
                            (selected_threshold == 4'b0001) ? MASK_09P :
                            (selected_threshold == 4'b0010) ? MASK_09  : 
                            (selected_threshold == 4'b0011) ? MASK_08  :                          
                            (selected_threshold == 4'b0100) ? MASK_07  :
                            (selected_threshold == 4'b0011) ? MASK_06  :
                            (selected_threshold == 4'b0101) ? MASK_05  :
                                                              MASK_09;
    
    wire feedback = seed[`RNDSIZE-1] ^ seed[3] ^ seed[2] ^ seed[0];// polynomial of LFSR X^3+X^2+X
    wire [`RNDSIZE-1:0] lfsr_output = {seed[`RNDSIZE-2:0], feedback};

    wire [BITSIZE-1:0] temp_lfsr_ouput;

    wire [COUNTERSIZE-1:0] count[`BITMAP_NB_SEGMENTS-1:0]; // Create an array of count signals

    genvar i;
    generate
        for (i = 0; i < `BITMAP_NB_SEGMENTS; i = i+1) begin
            wire [BITSIZE-1:0] temp_lfsr_output = lfsr_output[i*BITSIZE+:BITSIZE];
            BitCounter bc ( .bitslice(temp_lfsr_output | selected_mask), .count (count[i]) );
            assign rnd[i] = (count[i] > selected_threshold) ? 1:0;
        end
    endgenerate
    // IMPORTANT: In Verilog, all bit selections and range selections must be determined at compile time.
    // can't use variables to dynamically select ranges of bits from a signal. 
    // The bit selection or part selection range should be a constant expression.
    // => +: operator to select a slice of the lfsr_output signal combined with generated blocks
endmodule

//Desired probability that match the threshold probability input -to kept for test
//localparam THRESH_05  = 4'b0101;  // 0.5 if mask applied slightly < 0.4 either
//localparam THRESH_06  = 4'b0100;  // 0.6
//localparam THRESH_07  = 4'b0100;  // 0.7 if mask applied
//localparam THRESH_08  = 4'b0011;  // 0.8
//localparam THRESH_09  = 4'b0010;  // 0.9
//localparam THRESH_9+  = 4'b0001;  // 0.97
//localparam THRESH_10  = 4'b0000;  // 1 all bits set

//BitCounter bc ( .bitslice(10'b1111100000), .count (count) ); keep to test BitCounter
// count = 4'b0101 expected

// BitCounter combinational
module BitCounter #(
    parameter BITSIZE = 10,
    parameter COUNTERSIZE = 4
    )(
    input [BITSIZE-1:0] bitslice,
    output [COUNTERSIZE-1:0] count
);
    wire [COUNTERSIZE-1:0] tempCount;
  
    assign tempCount = bitslice[0] + bitslice[1] + bitslice[2] + bitslice[3] + bitslice[4] + bitslice[5] + bitslice[6] + bitslice[7] + bitslice[8] + bitslice[9];
  
    assign count = tempCount;
  
endmodule
//see tb-Bitcounter.sv in testbenchs folder
//# Allocation: Simulator allocated 4670 kB (elbread=427 elab2=4108 kernel=134 sdf=0)
//# KERNEL: ASDB file was created in location /home/runner/dataset.asdb
//# KERNEL: bitslice: 1010101010
//# KERNEL: count: 0101
//# RUNTIME: Info: RUNTIME_0068 testbench.sv (39): $finish called.


/*

module BitCounter #(
    parameter BITSIZE = 10,
    parameter COUNTERSIZE = 4
)(
    input [BITSIZE-1:0] bitslice,
    output [COUNTERSIZE-1:0] count
);

    wire [9:0] shiftedBitslice;
    wire [3:0] tempCount;
  
    assign shiftedBitslice = bitslice >> 1;

    assign tempCount[0] = bitslice[0] & (~(|bitslice[9:1]));
    assign tempCount[1] = bitslice[1] & (~(|bitslice[9:2]));
    assign tempCount[2] = bitslice[2] & (~(|bitslice[9:3]));
    assign tempCount[3] = bitslice[3] + bitslice[4] + bitslice[5] + bitslice[6] + bitslice[7] + bitslice[8] + bitslice[9];

    assign count = (tempCount[3:2] == 2'b11) ? {2'b1, tempCount[1:0]} : tempCount;
endmodule








/*

// BitCounter combinational
module BitCounter #(
    parameter BITSIZE =10,
    parameter COUNTERSIZE =4

    )(input [BITSIZE-1:0] bitslice, output [COUNTERSIZE-1:0] count);

    wire [9:0] shiftedBitslice;
    wire [3:0] tempCount;
  
    assign shiftedBitslice = bitslice >> 1;

    assign tempCount[0] = bitslice[0] & (~(|bitslice[9:1]));
    assign tempCount[1] = bitslice[1] & (~(|bitslice[9:2]));
    assign tempCount[2] = bitslice[2] & (~(|bitslice[9:3]));
    assign tempCount[3] = bitslice[3] | bitslice[4] | bitslice[5] | bitslice[6] | bitslice[7] | bitslice[8] | bitslice[9];

    assign count = (tempCount[3:2] == 2'b11) ? {2'b1, tempCount[1:0]} : tempCount;
endmodule

/// Tested with https://www.edaplayground.com/x/9 TEST WRONG
///# KERNEL: bitslice: 1010101010
///# KERNEL: count: 1000 --should get 0101 i.e 5 bits


/*
module LFSR_comb #(
    parameter BITSIZE = 10
    )(
    input [`RNDSIZE-1:0] seed,
    input [1:0] probability, // 2-bit input to choose the probability
    input [BITSIZE-1:0] counter,
    output [`BITMAP_NB_SEGMENTS-1:0] rnd
);

    localparam THRESH_10  = 10'b1111111111;  // Probability: 1.0 segment always ON
    localparam THRESH_085 = 10'b1101111111;  // Probability: 0.85
    localparam THRESH_07  = 10'b1001111111;  // Probability: 0.7
    localparam THRESH_055 = 10'b0111111111;  // Probability: 0.55
    localparam THRESH_TST = 10'b0000000011;  
    

    localparam MASK_10    = 10'b1111111111;  // Probability: 1.0 segment always ON
    localparam MASK_09    = 10'b1111111110;  // Probability: 0.9
    localparam MASK_07    = 10'b1111111100;  // Probability: 0.75
    localparam MASK_06    = 10'b1111111000;  // Probability: 0.5625
    localparam MASK_05    = 10'b1111100000;  // Probability: 0.53125
    localparam MASK_TST   = 10'b0000000001; 



    // Chosen polynomial for LFSR: X^3+X^2+X
    wire feedback = seed[`RNDSIZE-1] ^ seed[3] ^ seed[2] ^ seed[0]; 
    wire [`RNDSIZE-1:0] lfsr_output = {seed[`RNDSIZE-2:0], feedback};

    wire [BITSIZE-1:0] count = counter;

// Select the threshold based on the input probability value
    assign selected_threshold = (probability == 2'b00) ? THRESH_055 :
                                (probability == 2'b01) ? THRESH_07  :
                                (probability == 2'b10) ? THRESH_TST :
                                                         THRESH_10;

    genvar i,j;
    for (i = 0; i < `BITMAP_NB_SEGMENTS; i = i+1) begin 
        assign lfsr_output[i:i+BITSIZE] = lfsr_output[i:i+BITSIZE] | MASK_TST; //add bits at 1 to bitslice
        /*for (j = 1; j < BITSIZE; j = j + 1) begin
            wire bit_1 = lfsr_output[i:i+j];
            assign count[j] = count[j-1] + bit_1;
        end
        assign rnd[i] = count[BITSIZE-1] > selected_threshold ? 1 : 0;
        //assign rnd[i] = (lfsr_output[i:i+10] ^ selected_mask);
    end  
endmodule

module LFSR_comb (
    input [`RNDSIZE-1:0] seed,
    input [1:0] probability, // 2-bit input to choose the probability
    output [`BITMAP_NB_SEGMENTS-1:0] rnd
);

    localparam MASK_10 = 10'b1111111111;  // Probability: 1.0 segment always ON
    localparam MASK_09 = 10'b1111111110;  // Probability: 0.9
    localparam MASK_07 = 10'b1111111100;  // Probability: 0.75
    localparam MASK_06 = 10'b1111111000;  // Probability: 0.5625
    localparam MASK_05 = 10'b1111100000;  // Probability: 0.53125 
    
    wire feedback = seed[`RNDSIZE-1] ^ seed[3] ^ seed[2] ^ seed[0];// polynomial of LFSR X^3+X^2+X
    wire [`RNDSIZE-1:0] lfsr_output = {seed[`RNDSIZE-2:0], feedback};

// Select the mask based on the input probability value
    assign selected_mask = (probability == 2'b00) ? MASK_05  :
                           (probability == 2'b01) ? MASK_06  :
                           (probability == 2'b10) ? MASK_07  :
                           (probability == 2'b11) ? MASK_09  :
                                                    MASK_08;
        genvar i;
        for (i = 0; i < `BITMAP_NB_SEGMENTS; i = i+1) begin 
            assign rnd[i] = (lfsr_output[i:i+10] ^ selected_mask);
        end  
endmodule
*/