`ifdef HAS_WATERMARK
module main ( z , msg , watmk , rnd , pix );
`else
module main ( z , msg , rnd , pix );
`endif

// garbler inputs
input z;
input [`BITMAP_NB_SEGMENTS-1:0] msg;
`ifdef HAS_WATERMARK
input [`WIDTH*`HEIGHT-1:0] watmk;
`endif

// evaluator inputs
input [`RNDSIZE-1:0] rnd;

output [`WIDTH*`HEIGHT-1:0] pix;

wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] rndx;
wire [`BITMAP_NB_SEGMENTS-1:0] selseg;
// TODO do we need an intermediate? ninja && time ./tests/cli_display_skcd --width=120 --height=52
// Entered genlib library with 16 gates from file "/home/pratn/Documents/interstellar/api_circuits/lib_circuits_wrapper/deps/lib_circuits/data/verilog/skcd.genlib".
// Warning: Constant-0 drivers added to 1 non-driven nets in network "watermark":
// pix[6239]
// Hierarchy reader flattened 4 instances of logic boxes and left 0 black boxes.
// BlifParser: inputs : 6264, outputs : 6240
// real    0m8.884s
// user    0m8.837s
// sys     0m0.047s
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

//xorexpand xe(.r (rnd), .p (rndx));
rndswitch rs(.s (msg), .r (rndx), .z (z), .o (selseg));
segment2pixel sp(.s (selseg), .p (pixsegments));
`ifdef HAS_WATERMARK
  watermark wm(.pixsegments (pixsegments), .pixwatermark (watmk), .pix (pix));
`endif

endmodule
