module main ( rnd , rndx );

input [`RNDSIZE-1:0] rnd;

output [`BITMAP_NB_SEGMENTS-1:0] rndx;

wire [3:0] count; // for counter bc in lc

LFSR_comb lc(
    .seed(rnd),
    .probability(`PROBA),
    .rnd(rndx)
);

endmodule

