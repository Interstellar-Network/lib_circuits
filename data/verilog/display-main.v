// module to create an bitmap frame
// using visual crypto to display random segments
// input: segments to display, random bits
// output: pixels
// (c) Skeyecode

// main module integrating sub-circuits (xorexpand, rndswitch and segment2pixel)
module main(msg, rnd, z, pix);
input [`BITMAP_NB_SEGMENTS-1:0] msg; // message to display
input [`RNDSIZE-1:0] rnd;      // random bits
input z; // input which must be set to 0 (kludge to keep BUF gates)
output [`WIDTH*`HEIGHT-1:0] pix;  // pixels output
wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] rndx; // expanded randoms
wire [`BITMAP_NB_SEGMENTS-1:0] selseg;       // segments selected for frame display

xorexpand xe(.r (rnd), .p (rndx)); // expand random bits
rndswitch rs(.s (msg), .r (rndx), .z (z), .o (selseg));  // randomly select segments
segment2pixel sp(.s (selseg), .p (pix));  // convert segments to pixels
endmodule
