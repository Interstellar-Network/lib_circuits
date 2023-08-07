module rndswitch ( s , r , z , o );
input [`BITMAP_NB_SEGMENTS-1:0] s;
input [`RNDSIZE-1:0] r;
input z;
output [`BITMAP_NB_SEGMENTS-1:0] o;
wire [`BITMAP_NB_SEGMENTS-1:0] t;

genvar i;
for (i=0; i<`BITMAP_NB_SEGMENTS; i=i+1) begin
    assign t[i] = s[i] & r[i];
end
for (i=0; i<`BITMAP_NB_SEGMENTS; i=i+1) begin
    assign o[i] = t[i] ^ z;
end
endmodule 


/*module rndswitch (
  input [`BITMAP_NB_SEGMENTS-1:0] s,
  input [`RNDSIZE*(`RNDSIZE-1)/2-1:0] r,
  input z,
  input force_on,
  output [`BITMAP_NB_SEGMENTS-1:0] o
);
  wire [`BITMAP_NB_SEGMENTS-1:0] t;
  wire [`RNDSIZE*(`RNDSIZE-1)/2-1:0] override_r;

  genvar i;
  assign override_r = (force_on) ? {`RNDSIZE*(`RNDSIZE-1)/2{1'b1}} : r;

  for (i = 0; i < `BITMAP_NB_SEGMENTS; i = i + 1) begin
    assign t[i] = s[i] & override_r[i];
  end

  for (i = 0; i < `BITMAP_NB_SEGMENTS; i = i + 1) begin
    assign o[i] = t[i] ^ z;
  end
endmodule */







