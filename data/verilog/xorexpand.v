// Linear Feedback Shift Register
// to generate probabilties of displaying digits segments
//
// probability 0.5 -> 2'b00
// probability 0.6 -> 2'b01
// probability 0.7 -> 2'b10
// probability 0.9 -> 2'b11
module xorexpand ( r , probability, p );
input [`RNDSIZE-1:0] r;
input [1:0] probability; // 2-bit input to choose the probability
output [`RNDSIZE*(`RNDSIZE-1)/2-1:0] p;

localparam MASK_10 = 10'b1111111111;  // Probability: 1.0 segment always ON
localparam MASK_09 = 10'b1111111110;  // Probability: 0.9
localparam MASK_07 = 10'b1111111100;  // Probability: 0.75
localparam MASK_06 = 10'b1111111000;  // Probability: 0.5625
localparam MASK_05 = 10'b1111100000;  // Probability: 0.53125

wire feedback = r[`RNDSIZE-1] ^ r[3] ^ r[2] ^ r[0];// polynomial of LFSR X^3+X^2+X
wire [`RNDSIZE-1:0] lfsr_output = {r[`RNDSIZE-2:0], feedback};

// Select the mask based on the input probability value
assign selected_mask = (probability == 2'b00) ? MASK_05  :
                        (probability == 2'b01) ? MASK_06  :
                        (probability == 2'b10) ? MASK_07  :
                        (probability == 2'b11) ? MASK_09  :
                                                MASK_08;

genvar i;
for (i = 0; i < `BITMAP_NB_SEGMENTS; i = i+1) begin
    assign p[i] = |(lfsr_output[i:i+10] | selected_mask);
end

// genvar i,j;
// for (i=0; i<`RNDSIZE; i=i+1) begin
//     for (j=i+1; j<`RNDSIZE; j=j+1) begin
//         localparam integer k = i*`RNDSIZE-i*(i+1)/2+j-i-1;
//         assign p[k] = r[i] ^ r[j];
//     end
// end
endmodule
