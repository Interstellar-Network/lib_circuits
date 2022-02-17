// random switch to select segments to display in one frame


module rndswitch(s, r, z, o);
input [`BITMAP_NB_SEGMENTS-1:0] s; // segments array
input [`RNDSIZE*(`RNDSIZE-1)/2-1:0] r; // expanded randoms
input z; // input which should be set to 0 (kludge to keep BUF gates)
output [`BITMAP_NB_SEGMENTS-1:0] o; // output array
wire [`BITMAP_NB_SEGMENTS-1:0] t; // temporary array

integer i;
for (i=0; i<`BITMAP_NB_SEGMENTS; i=i+1) begin
    assign t[i] = s[i] & r[i];
end
integer j;
for (i=0; i<`BITMAP_NB_SEGMENTS; i=i+1) begin
    assign o[i] = t[i] ^ z; // force XOR/BUF gate (otherwise they are optimized out)
end
endmodule
