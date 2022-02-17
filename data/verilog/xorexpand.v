module xorexpand ( r , p );
input [`RNDSIZE-1:0] r;
output [`RNDSIZE*(`RNDSIZE-1)/2-1:0] p;

genvar i,j;
for (i=0; i<`RNDSIZE; i=i+1) begin
    for (j=i+1; j<`RNDSIZE; j=j+1) begin
        localparam integer k = i*`RNDSIZE-i*(i+1)/2+j-i-1;
        assign p[k] = r[i] ^ r[j];
    end
end
endmodule
