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

wire [`BITMAP_NB_SEGMENTS-1:0] rndx;
wire [`BITMAP_NB_SEGMENTS-1:0] selseg;
wire [3:0] count; // for counter bc in lc


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
// Desired probability of displaying segments ON/OFF
// probability 0.5  = 4'b0110 approx 1/2
// probability 0.6  = 4'b0101
// probability 0.7  = 4'b0100
// probability 0.8  = 4'b0011
// probability 0.9  = 4'b0010
// probability +0.9 = 4'b0001 approx. 0.99
// probability 1    = 4'b0000 all segments ON

LFSR_comb lc(
    .seed(rnd),
    .probability(4'b0100), // select probability of displaying segments
    .rnd(rndx),
    .count(count)
);

rndswitch rs(.s (msg), .r (rndx), .z (z),.o (selseg));
segment2pixel sp(.s (selseg), .p (pixsegments));
`ifdef HAS_WATERMARK
  watermark wm(.pixsegments (pixsegments), .pixwatermark (watmk), .pix (pix));
`endif

endmodule

