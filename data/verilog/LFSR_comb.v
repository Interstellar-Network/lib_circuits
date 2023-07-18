module LFSR_comb (
    input [`RNDSIZE*(`RNDSIZE-1)/2-1:0] seed,
    input [1:0] probability, // 2-bit input to choose the probability
    output [`RNDSIZE*(`RNDSIZE-1)/2-1:0] rnd
);
    wire feedback = seed[`RNDSIZE*(`RNDSIZE-1)/2-1] ^ seed[3] ^ seed[2] ^ seed[0];
    wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] lfsr_output = {seed[`RNDSIZE*(`RNDSIZE-1)/2-2:0], feedback};

    // Create masks with the desired probabilities
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_05 = 10'h1FF << (`RNDSIZE*(`RNDSIZE-1)/2 - 9);
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_07 = 10'h2BB << (`RNDSIZE*(`RNDSIZE-1)/2 - 9);
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_08 = 10'h333 << (`RNDSIZE*(`RNDSIZE-1)/2 - 9);
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_09 = 10'h3FF << (`RNDSIZE*(`RNDSIZE-1)/2 - 9);

    wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] selected_mask;

    // Select the mask based on the input probability value
    assign selected_mask = (probability == 2'b00) ? MASK_05 :
                           (probability == 2'b01) ? MASK_07 :
                           (probability == 2'b10) ? MASK_08 :
                                                    MASK_09;

    assign rnd = lfsr_output ^ selected_mask;
endmodule

