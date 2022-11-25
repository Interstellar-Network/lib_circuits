module main ( z , msg , rnd , pix );
input [`BITMAP_NB_SEGMENTS-1:0] msg;
input [`RNDSIZE-1:0] rnd;
input z;
output [`WIDTH*`HEIGHT-1:0] pix;
wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] rndx;
wire [`BITMAP_NB_SEGMENTS-1:0] selseg;

xorexpand xe(.r (rnd), .p (rndx));
rndswitch rs(.s (msg), .r (rndx), .z (z), .o (selseg));
segment2pixel sp(.s (selseg), .p (pix));
endmodule
