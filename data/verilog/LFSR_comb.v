// Linear Shift Feedback Register Combinational cicrcuit
// to define the probabilities of displaying a digit segment ON/OFF

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
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_09 = 16'h3FFF << (`RNDSIZE*(`RNDSIZE-1)/2 - 16);


    wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] selected_mask;

    // Select the mask based on the input probability value
    assign selected_mask = (probability == 2'b00) ? MASK_05 :
                           (probability == 2'b01) ? MASK_06 :
                           (probability == 2'b10) ? MASK_07 :
                           (probability == 2'b11) ? MASK_08 :
                                                    MASK_09;

    assign rnd = lfsr_output ^ selected_mask;
endmodule

