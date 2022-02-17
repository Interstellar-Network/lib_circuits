// xor expand pseudo random generator
// input  n bits
// output n*(n-1)/2 bits

module xorexpand(r, p);
input [`RNDSIZE-1:0] r; // random array
output [`RNDSIZE*(`RNDSIZE-1)/2-1:0] p; // output array

// generate every possible XOR combination between input bits
genvar i, j;
generate
    for (i=0; i<`RNDSIZE; i=i+1) begin
        for (j=i+1; j<`RNDSIZE; j=j+1) begin
            localparam integer k = i*`RNDSIZE-i*(i+1)/2+j-i-1;
            assign p[k] = r[i] ^ r[j];
        end
    end
endgenerate
endmodule
