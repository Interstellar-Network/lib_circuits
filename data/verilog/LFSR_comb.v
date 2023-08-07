//Linear Feedback Shift Register
// to generate probabilties of displaying digits segments

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
    
    wire feedback = seed[`RNDSIZE-1] ^ seed[3] ^ seed[2] ^ seed[0];// polynomial of LFSR X^3+X^2+1
    wire [`RNDSIZE-1:0] lfsr_output = {seed[`RNDSIZE-2:0], feedback};

// Select the mask based on the input probability value
    assign selected_mask = (probability == 2'b00) ? MASK_05  :
                           (probability == 2'b01) ? MASK_06  :
                           (probability == 2'b10) ? MASK_07  :
                           (probability == 2'b11) ? MASK_09  :
                                                    MASK_08;
        genvar i;
        for (i = 0; i < `BITMAP_NB_SEGMENTS; i = i+1) begin 
            assign rnd[i] = |(lfsr_output[i:i+10] | selected_mask);
            //assign rnd[i] = (lfsr_output[i:i+10] ^ selected_mask);
        end
    
endmodule


/*
module LFSR_comb (
    input [`RNDSIZE-1:0] seed,
    input [1:0] probability, // 2-bit input to choose the probability
    output [`RNDSIZE-1:0] rnd
);
    wire feedback = seed[`RNDSIZE-1] ^ seed[3] ^ seed[2] ^ seed[0];
    wire [`RNDSIZE-1:0] lfsr_output = {seed[`RNDSIZE-2:0], feedback};

    // Create masks of 10 bits with the desired probabilities 

    localparam [`RNDSIZE-1:0] MASK_05 = {10'b1111111111};
    localparam [`RNDSIZE-1:0] MASK_06 = {10'b1111111100};
    localparam [`RNDSIZE-1:0] MASK_07 = {10'b1111111000};
    localparam [`RNDSIZE-1:0] MASK_08 = {10'b1111110000};
    localparam [`RNDSIZE-1:0] MASK_09 = {10'b1111100000};
   
    wire [`RNDSIZE-1:0] selected_mask;

    // Select the mask based on the input probability value
    assign selected_mask = (probability == 2'b00) ? MASK_05  :
                           (probability == 2'b01) ? MASK_06  :
                           (probability == 2'b10) ? MASK_07  :
                           (probability == 2'b11) ? MASK_09  :
                                                    MASK_08;

    assign rnd = lfsr_output ^ selected_mask;
endmodule

/*

module LFSR_comb (
    input [`RNDSIZE*(`RNDSIZE-1)/2-1:0] seed,
    input [1:0] probability, // 2-bit input to choose the probability
    output [`RNDSIZE*(`RNDSIZE-1)/2-1:0] rnd
);
    wire feedback = seed[`RNDSIZE*(`RNDSIZE-1)/2-1] ^ seed[3] ^ seed[2] ^ seed[0];
    wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] lfsr_output = {seed[`RNDSIZE*(`RNDSIZE-1)/2-2:0], feedback};

    // Create masks of 16 bits with the desired probabilities 
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_05 = 16'hFFFF << (`RNDSIZE*(`RNDSIZE-1)/2 - 16);
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_06 = 16'h3FF  << (`RNDSIZE*(`RNDSIZE-1)/2 - 16);
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_07 = 16'h7FF  << (`RNDSIZE*(`RNDSIZE-1)/2 - 16);
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_08 = 16'h1FFF << (`RNDSIZE*(`RNDSIZE-1)/2 - 16);
    //localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_09 = 16'h7FFF << (`RNDSIZE*(`RNDSIZE-1)/2 - 16);

    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_09 = 16'h7FFFFFFFFFFFFFFF << (`BITMAP_NB_SEGMENTS-1);
   
    wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] selected_mask;

    // Select the mask based on the input probability value
    assign selected_mask = (probability == 2'b00) ? MASK_05  :
                           (probability == 2'b01) ? MASK_06  :
                           (probability == 2'b10) ? MASK_07  :
                           (probability == 2'b11) ? MASK_09  :
                                                    MASK_08;

    assign rnd = lfsr_output ^ selected_mask;
endmodule



    localparam [`RNDSIZE-1:0] MASK_05 = 10'hFFFF << (`RNDSIZE-10);
    localparam [`RNDSIZE-1:0] MASK_06 = 10'h3FF  << (`RNDSIZE-10);
    localparam [`RNDSIZE-1:0] MASK_07 = 10'h7FF  << (`RNDSIZE-10);
    localparam [`RNDSIZE-1:0] MASK_08 = 10'h1FFF << (`RNDSIZE-10);
    localparam [`RNDSIZE-1:0] MASK_09 = 10'h7FFF << (`RNDSIZE-10);





*/
