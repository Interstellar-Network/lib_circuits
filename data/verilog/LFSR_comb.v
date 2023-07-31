module LFSR_comb (
    input [`RNDSIZE*(`RNDSIZE-1)/2-1:0] seed,
    input [1:0] probability, // 2-bit input to choose the probability
    output [`RNDSIZE*(`RNDSIZE-1)/2-1:0] rnd
);
    wire feedback = seed[`RNDSIZE*(`RNDSIZE-1)/2-1] ^ seed[3] ^ seed[2] ^ seed[0];
    wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] lfsr_output = {seed[`RNDSIZE*(`RNDSIZE-1)/2-2:0], feedback};

    // Create masks with the desired probabilities (apprixmating RNDSIZE of 13 bits) need to be recalculate if RNDSIZE change
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_05 = 14'h7F << (`RNDSIZE*(`RNDSIZE-1)/2 -`RNDSIZE );
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_06 = 14'hFF  << (`RNDSIZE*(`RNDSIZE-1)/2 -`RNDSIZE );
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_07 = 14'h1FF << (`RNDSIZE*(`RNDSIZE-1)/2 -`RNDSIZE);
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_08 = 14'h3FF << (`RNDSIZE*(`RNDSIZE-1)/2 -`RNDSIZE);
    localparam [`RNDSIZE*(`RNDSIZE-1)/2-1:0] MASK_09 = 14'hFFF << (`RNDSIZE*(`RNDSIZE-1)/2 -`RNDSIZE);


    wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] selected_mask;

    // Select the mask based on the input probability value
    assign selected_mask = (probability == 2'b00) ? MASK_05 :
                           (probability == 2'b01) ? MASK_06 :
                           (probability == 2'b10) ? MASK_07 :
                           (probability == 2'b11) ? MASK_08 :
                                                    MASK_09;

    assign rnd = lfsr_output ^ selected_mask;
endmodule

