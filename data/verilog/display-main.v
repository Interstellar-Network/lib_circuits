module main ( z , msg , watmk , rnd , pix );
// garbler inputs
input z;
input [`BITMAP_NB_SEGMENTS-1:0] msg;
input [`WIDTH*`HEIGHT-1:0] watmk;

// evaluator inputs
input [`RNDSIZE-1:0] rnd;

output [`WIDTH*`HEIGHT-1:0] pix;

wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] rndx;
wire [`BITMAP_NB_SEGMENTS-1:0] selseg;
wire [`WIDTH*`HEIGHT-1:0] pixsegments;

// Instantiate LFSR_comb module
// Available probabilities:
// - 2'b00: 0.5
// - 2'b01: 0.7
// - 2'b10: 0.8
// - 2'b11: 0.9
LFSR_comb lc(
    .seed(rnd),
    .probability(2'b11), // Choose 0.9 probability
    .rnd(rndx)
);

rndswitch rs(.s (msg), .r (rndx), .z (z), .o (selseg));
segment2pixel sp(.s (selseg), .p (pixsegments));
watermark wm(.pixsegments (pixsegments), .pixwatermark (watmk), .pix (pix));
endmodule
